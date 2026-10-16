#define GLM_FORCE_RADIANS
#include "horse.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game.h"
#include "fp_player.h"
#include "horse_assets.h"
#include "horse_audio.h"
#include "dust.h"

namespace {

const float TAU = glm::two_pi<float>();
const float BODY_HEIGHT = 13.5f;   // body pivot above the hooves when standing
const float HOOF_HEIGHT = 1.0f;
const float PASTERN = 1.3f;
const float MOUNT_RANGE = 36.0f;
const float GRAVITY = 115.0f;
const float JUMP_SPEED = 60.0f;
const int TAIL_SEGMENTS = 5;
const int MANE_LOCKS = 10;
const glm::vec3 kUp(0.0f, 1.0f, 0.0f);
const glm::vec3 kFwd(0.0f, 0.0f, -1.0f);
const glm::vec3 SEAT(0.0f, 5.0f, -2.4f);

enum Material { MAT_COAT, MAT_POINTS, MAT_HAIR, MAT_HOOF, MAT_EYE, MAT_LEATHER, MAT_BLANKET, MAT_CLOTH, MAT_SKIN, MAT_METAL };

// footfall pattern per gait: leg order is left fore, right fore, left hind, right hind
struct GaitKey {
    float speed;
    float duty;      // fraction of the cycle each hoof is on the ground
    float stride;    // distance covered per cycle
    float lift;      // hoof lift height during swing
    float comp;      // body bob from leg loading
    float rock;      // body pitch from fore/hind loading
    float offset[4];
};

const GaitKey GAITS[] = {
    {0.0f,                 0.62f, 34.0f, 1.8f, 0.30f, 0.010f, {0.00f, 0.50f, 0.75f, 0.25f}},
    {Horse::WALK_SPEED,    0.62f, 34.0f, 2.4f, 0.30f, 0.015f, {0.00f, 0.50f, 0.75f, 0.25f}},
    {Horse::TROT_SPEED,    0.42f, 52.0f, 3.6f, 0.75f, 0.010f, {0.25f, 0.75f, 0.75f, 0.25f}},
    {Horse::CANTER_SPEED,  0.38f, 66.0f, 3.8f, 0.95f, 0.055f, {0.05f, 0.35f, 0.75f, 0.05f}},
    {Horse::GALLOP_SPEED,  0.28f, 82.0f, 3.4f, 1.05f, 0.045f, {0.20f, 0.30f, 0.75f, 0.85f}},
};
const int NUM_GAITS = sizeof(GAITS) / sizeof(GAITS[0]);

float Damp(float current, float target, float rate, float dt) {
    return glm::mix(current, target, 1.0f - std::exp(-rate * dt));
}

float Approach(float current, float target, float step) {
    if (current < target) return std::min(current + step, target);
    return std::max(current - step, target);
}

float WrapAngle(float a) {
    while (a > glm::pi<float>()) a -= TAU;
    while (a < -glm::pi<float>()) a += TAU;
    return a;
}

float Fract(float x) {
    return x - std::floor(x);
}

float Tent(float x, float lo, float mid, float hi) {
    if (x <= lo || x >= hi) return 0.0f;
    return x < mid ? (x - lo) / (mid - lo) : (hi - x) / (hi - mid);
}

float Rand01() {
    static unsigned int s = 0x9E3779B9u;
    s = s * 1664525u + 1013904223u;
    return (float)((s >> 8) & 0xFFFFFF) / 16777215.0f;
}

// rotation whose local -y points along dir, with local -z as close to forward_hint as possible
glm::quat DirRot(const glm::vec3& dir, const glm::vec3& forward_hint) {
    glm::vec3 y = -glm::normalize(dir);
    glm::vec3 f = forward_hint - y * glm::dot(forward_hint, y);
    if (glm::length(f) < 1e-4f) {
        glm::vec3 alt = std::abs(y.x) < 0.9f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, -1.0f);
        f = alt - y * glm::dot(alt, y);
    }
    glm::vec3 z = -glm::normalize(f);
    glm::vec3 x = glm::cross(y, z);
    return glm::normalize(glm::quat_cast(glm::mat3(x, y, z)));
}

glm::quat BoneRot(const glm::vec3& from, const glm::vec3& to, const glm::vec3& forward_hint) {
    glm::vec3 d = to - from;
    if (glm::length(d) < 1e-5f) d = glm::vec3(0.0f, -1.0f, 0.0f);
    return DirRot(d, forward_hint);
}

// two bone ik, returns the middle joint; end receives the reachable end point
glm::vec3 SolveTwoBone(const glm::vec3& root, const glm::vec3& target, float a, float b, const glm::vec3& pole, glm::vec3& end) {
    glm::vec3 d = target - root;
    float len = glm::length(d);
    glm::vec3 dir = len > 1e-5f ? d / len : glm::vec3(0.0f, -1.0f, 0.0f);
    len = glm::clamp(len, std::abs(a - b) + 0.01f, a + b - 0.01f);
    end = root + dir * len;
    float cos_a = glm::clamp((a * a + len * len - b * b) / (2.0f * a * len), -1.0f, 1.0f);
    float sin_a = std::sqrt(1.0f - cos_a * cos_a);
    glm::vec3 p = pole - dir * glm::dot(pole, dir);
    if (glm::length(p) < 1e-4f) p = glm::vec3(0.0f, 0.0f, -1.0f);
    p = glm::normalize(p);
    return root + dir * (a * cos_a) + p * (a * sin_a);
}

glm::vec3 Point(const glm::mat4& m, const glm::vec3& p) {
    return glm::vec3(m * glm::vec4(p, 1.0f));
}

// height of the neck crest along the neck mesh
float CrestY(float z) {
    const float zs[] = {0.0f, -2.5f, -5.0f, -7.5f, -9.5f, -11.0f};
    const float ys[] = {3.2f, 3.0f, 2.65f, 2.3f, 1.95f, 1.6f};
    for (int i = 0; i < 5; i++) {
        if (z <= zs[i] && z >= zs[i + 1]) {
            return glm::mix(ys[i], ys[i + 1], (zs[i] - z) / (zs[i] - zs[i + 1]));
        }
    }
    return z > 0.0f ? ys[0] : ys[5];
}

} // namespace

void Horse::LoadAssets(ResourceManager& resman) {
    HorseAssets::Load(resman);
}

Horse::Horse(Game* game, std::shared_ptr<Terrain> terrain, Camera* camera)
    : SceneNode("Obj_Horse", "", "S_NormalMap", ""), game_(game), terrain_(terrain), camera_(camera) {
    transform.SetOrientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    BuildModel();
}

Horse::~Horse() {
    HorseAudio::StopLoop(wind_loop_);
}

SceneNode* Horse::Part(SceneNode* parent, const std::string& mesh, const std::string& texture, const glm::vec3& pos, int material) {
    SceneNode* n = new SceneNode("Obj_HorsePart", mesh, "S_NormalMap", texture);
    n->transform.SetPosition(pos);
    n->transform.SetOrientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    bool cloth = material == MAT_CLOTH || material == MAT_BLANKET || material == MAT_SKIN;
    n->SetNormalMap(cloth ? "T_Cloth_n" : "T_HorseHair_n", cloth ? 1.0f : 2.0f);
    auto& m = n->material;
    m.ambient_additive = 0.08f;
    switch (material) {
        case MAT_COAT:    m.specular_power = 26.0f;  m.specular_coefficient = 0.3f;  m.diffuse_strength = 0.95f; break;
        case MAT_POINTS:  m.specular_power = 22.0f;  m.specular_coefficient = 0.3f;  m.diffuse_strength = 0.9f; m.ambient_additive = 0.12f; break;
        case MAT_HAIR:    m.specular_power = 18.0f;  m.specular_coefficient = 0.35f; m.diffuse_strength = 0.9f; m.ambient_additive = 0.12f; break;
        case MAT_HOOF:    m.specular_power = 40.0f;  m.specular_coefficient = 0.2f;  break;
        case MAT_EYE:     m.specular_power = 220.0f; m.specular_coefficient = 1.4f;  break;
        case MAT_LEATHER: m.specular_power = 22.0f;  m.specular_coefficient = 0.22f; break;
        case MAT_BLANKET: m.specular_power = 0.0f;   m.specular_coefficient = 0.0f; m.texture_repetition = 1.0f; break;
        case MAT_CLOTH:   m.specular_power = 0.0f;   m.specular_coefficient = 0.0f;  break;
        case MAT_SKIN:    m.specular_power = 12.0f;  m.specular_coefficient = 0.1f;  break;
        case MAT_METAL:   m.specular_power = 120.0f; m.specular_coefficient = 0.9f;  break;
    }
    parent->AddChild(n);
    return n;
}

void Horse::BuildModel() {
    body_pivot_ = Part(this, "", "", glm::vec3(0.0f, BODY_HEIGHT, 0.0f), MAT_COAT);
    body_ = Part(body_pivot_, "M_HorseBody", "T_HorseCoat", glm::vec3(0.0f), MAT_COAT);

    struct { glm::vec3 hip; bool front; float side; } defs[4] = {
        {{-2.35f, -2.2f, -7.6f}, true, -1.0f},
        {{ 2.35f, -2.2f, -7.6f}, true,  1.0f},
        {{-2.55f, -0.6f,  7.1f}, false, -1.0f},
        {{ 2.55f, -0.6f,  7.1f}, false, 1.0f},
    };
    for (int i = 0; i < 4; i++) {
        Leg& L = legs_[i];
        L.hip = defs[i].hip;
        L.front = defs[i].front;
        L.side = defs[i].side;
        L.upper_len = L.front ? 5.3f : 5.8f;
        L.lower_len = L.front ? 4.2f : 5.2f;
        L.nominal_z = L.front ? -7.6f : 7.8f;
        L.upper = Part(body_pivot_, L.front ? "M_HorseFrontUpper" : "M_HorseHindUpper", "T_HorseCoat", L.hip, MAT_COAT);
        L.lower = Part(L.upper, L.front ? "M_HorseFrontCannon" : "M_HorseHindCannon", "T_HorsePoints", glm::vec3(0.0f, -L.upper_len, 0.0f), MAT_POINTS);
        L.pastern = Part(L.lower, "M_HorsePastern", "T_HorsePoints", glm::vec3(0.0f, -L.lower_len, 0.0f), MAT_POINTS);
        L.hoof = Part(L.pastern, "M_HorseHoof", "T_HorseHoof", glm::vec3(0.0f, -PASTERN, 0.0f), MAT_HOOF);
    }

    neck_ = Part(body_pivot_, "M_HorseNeck", "T_HorseCoat", glm::vec3(0.0f, 2.3f, -8.7f), MAT_COAT);
    head_ = Part(neck_, "M_HorseHead", "T_HorseHead", glm::vec3(0.0f, 0.0f, -10.3f), MAT_COAT);
    for (int s = 0; s < 2; s++) {
        float side = s == 0 ? -1.0f : 1.0f;
        ears_[s] = Part(head_, "M_HorseEar", "T_HorseCoat", glm::vec3(0.6f * side, 1.25f, 0.25f), MAT_COAT);
        SceneNode* eye = Part(head_, "M_HorseEye", "T_HorseEye", glm::vec3(1.12f * side, 0.12f, -1.55f), MAT_EYE);
        eye->transform.SetScale(glm::vec3(1.0f, 1.0f, 1.25f));
        SceneNode* bit = Part(head_, "M_HorseEye", "T_TackMetal", glm::vec3(0.95f * side, -0.85f, -5.6f), MAT_METAL);
        bit->transform.SetScale(glm::vec3(0.7f));
    }
    Part(head_, "M_Noseband", "T_Tack", glm::vec3(0.0f, -0.25f, -5.0f), MAT_LEATHER);
    Part(head_, "M_Headstall", "T_Tack", glm::vec3(0.0f, -0.1f, 0.0f), MAT_LEATHER);
    forelock_ = Part(head_, "M_HorseManeLock", "T_HorseHair", glm::vec3(0.0f, 1.35f, 0.35f), MAT_HAIR);
    forelock_->transform.SetScale(glm::vec3(0.8f, 0.75f, 0.7f));

    for (int i = 0; i < MANE_LOCKS; i++) {
        float z = -0.4f - i * 1.0f;
        mane_root_[i] = glm::vec3(0.15f, CrestY(z) - 0.15f, z);
        mane_[i] = Part(neck_, "M_HorseManeLock", "T_HorseHair", mane_root_[i], MAT_HAIR);
        mane_[i]->transform.SetScale(glm::vec3(1.0f, 1.0f - i * 0.025f, 1.0f));
    }

    tail_len_[0] = 2.6f;
    tail_[0] = Part(body_pivot_, "M_HorseTailDock", "T_HorsePoints", glm::vec3(0.0f, 3.2f, 10.8f), MAT_POINTS);
    for (int i = 1; i < TAIL_SEGMENTS; i++) {
        bool tip = i == TAIL_SEGMENTS - 1;
        tail_len_[i] = tip ? 3.3f : 2.9f;
        tail_[i] = Part(tail_[i - 1], tip ? "M_HorseTailTip" : "M_HorseTailHair", "T_HorseHair", glm::vec3(0.0f, -tail_len_[i - 1], 0.0f), MAT_HAIR);
    }

    // tack
    Part(body_pivot_, "M_SaddleBlanket", "T_SaddleBlanket", glm::vec3(0.0f), MAT_BLANKET);
    Part(body_pivot_, "M_SaddleSkirt", "T_SaddleLeather", glm::vec3(0.0f), MAT_LEATHER);
    Part(body_pivot_, "M_SaddleSeat", "T_SaddleLeather", glm::vec3(0.0f), MAT_LEATHER);
    Part(body_pivot_, "M_SaddleHorn", "T_SaddleLeather", glm::vec3(0.0f, 5.25f, -5.0f), MAT_LEATHER);
    Part(body_pivot_, "M_Girth", "T_Tack", glm::vec3(0.0f), MAT_LEATHER);
    for (int s = 0; s < 2; s++) {
        float side = s == 0 ? -1.0f : 1.0f;
        SceneNode* strap = Part(body_pivot_, "M_StirrupStrap", "T_Tack", glm::vec3(4.3f * side, 3.4f, -2.4f), MAT_LEATHER);
        strap->transform.SetOrientation(glm::angleAxis(0.12f * side, glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    // rider
    rider_root_ = Part(body_pivot_, "", "", SEAT, MAT_CLOTH);
    rider_torso_ = Part(rider_root_, "M_RiderTorso", "T_RiderShirt", glm::vec3(0.0f), MAT_CLOTH);
    rider_head_ = Part(rider_torso_, "M_RiderHead", "T_RiderSkin", glm::vec3(0.0f, 7.3f, 0.0f), MAT_SKIN);
    Part(rider_head_, "M_RiderHat", "T_RiderHat", glm::vec3(0.0f, 1.95f, 0.05f), MAT_CLOTH);
    for (int s = 0; s < 2; s++) {
        float side = s == 0 ? -1.0f : 1.0f;
        RiderLimb& A = rider_arms_[s];
        A.root = glm::vec3(2.2f * side, 5.9f, 0.1f);
        A.upper_len = 3.0f;
        A.lower_len = 2.75f;
        A.upper = Part(rider_torso_, "M_RiderUpperArm", "T_RiderShirt", A.root, MAT_CLOTH);
        A.lower = Part(A.upper, "M_RiderForearm", "T_RiderShirt", glm::vec3(0.0f, -A.upper_len, 0.0f), MAT_CLOTH);
        A.end = Part(A.lower, "M_RiderHand", "T_DarkLeather", glm::vec3(0.0f, -A.lower_len, 0.0f), MAT_LEATHER);

        RiderLimb& L = rider_legs_[s];
        L.root = glm::vec3(1.15f * side, 0.45f, 0.2f);
        L.upper_len = 5.0f;
        L.lower_len = 4.2f;
        L.upper = Part(rider_root_, "M_RiderThigh", "T_RiderPants", L.root, MAT_CLOTH);
        L.lower = Part(L.upper, "M_RiderShin", "T_RiderPants", glm::vec3(0.0f, -L.upper_len, 0.0f), MAT_CLOTH);
        L.end = Part(L.lower, "M_RiderBoot", "T_DarkLeather", glm::vec3(0.0f, -1.7f, 0.0f), MAT_LEATHER);

        reins_[s] = Part(this, "M_Rein", "T_Tack", glm::vec3(0.0f), MAT_LEATHER);
    }
    rider_root_->visible = false;
    rider_root_->cast_shadow = false;
}

void Horse::PlaceAt(const glm::vec3& position, float yaw) {
    pos_ = glm::vec3(position.x, Ground(position.x, position.z), position.z);
    yaw_ = yaw;
    speed_ = target_speed_ = 0.0f;
    yaw_rate_ = target_yaw_rate_ = 0.0f;
    airborne_ = false;
    vertical_velocity_ = 0.0f;
    cam_focus_valid_ = false;
    transform.SetPosition(pos_);
    transform.SetOrientation(glm::angleAxis(yaw_, kUp));
}

void Horse::AddObstacles(SceneNode& instanced, float radius, float height_per_scale) {
    for (const Transform& t : instanced.GetInstances()) {
        glm::vec3 p = t.GetPosition();
        obstacles_.push_back({p.x, p.z, radius, p.y + height_per_scale * t.GetScale().y});
    }
}

void Horse::AddObstacle(const glm::vec3& position, float radius, float height) {
    obstacles_.push_back({position.x, position.z, radius, position.y + height});
}

glm::vec3 Horse::Forward() const {
    return glm::vec3(-std::sin(yaw_), 0.0f, -std::cos(yaw_));
}

glm::vec3 Horse::Right() const {
    return glm::vec3(std::cos(yaw_), 0.0f, -std::sin(yaw_));
}

glm::vec3 Horse::ToWorld(const glm::vec3& local) const {
    return pos_ + glm::angleAxis(yaw_, kUp) * local;
}

float Horse::Ground(float x, float z) const {
    return terrain_ ? terrain_->SampleHeight(x, z) : 0.0f;
}

float Horse::DistanceToRider() const {
    if (!rider_) return 1e9f;
    glm::vec3 d = rider_->transform.GetPosition() - pos_;
    return glm::length(glm::vec2(d.x, d.z));
}

// ------------------------------------------------------------------------------------------
// Player interaction
// ------------------------------------------------------------------------------------------

void Horse::Interact() {
    if (!rider_ || rider_->IsStatic()) return;
    if (ride_state_ == RideState::FREE) {
        if (DistanceToRider() < MOUNT_RANGE && !airborne_) {
            BeginMount();
        }
    } else if (ride_state_ == RideState::MOUNTED) {
        if (std::abs(speed_) < 70.0f && !airborne_) {
            BeginDismount();
        } else {
            ShowMessage("Slow down to dismount", 1.5f);
        }
    }
}

void Horse::BeginMount() {
    glm::mat4 inv = glm::inverse(camera_->GetViewMatrix());
    from_eye_ = glm::vec3(inv[3]);
    from_rot_ = glm::normalize(glm::quat_cast(glm::mat3(inv)));
    from_fov_ = 90.0f;
    fov_ = 90.0f;

    // swing the third person camera out from where the player is standing
    glm::vec3 d = from_eye_ - pos_;
    cam_yaw_ = std::atan2(d.x, d.z);
    cam_pitch_ = 0.25f;
    cam_idle_ = 0.0f;
    cam_focus_valid_ = false;
    fp_yaw_ = 0.0f;
    fp_pitch_ = -0.12f;

    ride_state_ = RideState::MOUNTING;
    transition_ = 0.0f;
    behaviour_ = Behaviour::IDLE;
    graze_ = 0.0f;
    rider_->SetMount(this);
    rider_was_visible_ = rider_->visible;
    rider_->visible = false;
    rider_root_->visible = true;
    rider_root_->cast_shadow = true;

    HorseAudio* audio = game_->GetHorseAudio();
    if (audio) audio->Play(HorseAudio::SNORT, pos_ + glm::vec3(0.0f, 18.0f, 0.0f), 0.55f, 1.0f);
    if (!ever_mounted_) {
        ever_mounted_ = true;
        hint_timer_ = 14.0f;
    }
}

void Horse::BeginDismount() {
    ride_state_ = RideState::DISMOUNTING;
    transition_ = 0.0f;
    from_eye_ = last_eye_;
    from_rot_ = last_rot_;
    from_fov_ = fov_;
    dismount_spot_ = pos_ - Right() * 13.0f;
    dismount_spot_.y = Ground(dismount_spot_.x, dismount_spot_.z);
    float view_yaw = camera_mode_ == CameraMode::FIRST_PERSON ? yaw_ + fp_yaw_ : cam_yaw_;
    dismount_yaw_ = view_yaw;
}

void Horse::FinishDismount() {
    ride_state_ = RideState::FREE;
    rider_root_->visible = false;
    rider_root_->cast_shadow = false;
    behaviour_ = Behaviour::IDLE;
    behaviour_timer_ = 6.0f;
    if (!rider_) return;
    rider_->SetMount(nullptr);
    rider_->SyncToMount(dismount_spot_ + glm::vec3(0.0f, rider_->GetHeight(), 0.0f), dismount_yaw_);
    rider_->transform.Update();
    rider_->visible = rider_was_visible_;
    camera_->Attach(&rider_->transform);
    camera_->SetPerspective(90.0f, 0.01f, 10000.0f, (float)camera_->GetWinWidth(), (float)camera_->GetWinHeight());
    fov_ = 90.0f;
}

void Horse::Call() {
    if (!rider_ || IsMounted() || rider_->IsStatic()) return;
    HorseAudio* audio = game_->GetHorseAudio();
    if (audio) audio->Play2D(HorseAudio::WHISTLE, 0.45f);
    if (DistanceToRider() > 1400.0f) {
        // too far to hear: bring it in from behind the player, out of sight
        glm::vec3 rf = rider_->transform.GetOrientation() * kFwd;
        rf.y = 0.0f;
        rf = glm::length(rf) > 1e-3f ? glm::normalize(rf) : kFwd;
        glm::vec3 p = rider_->transform.GetPosition() - rf * 650.0f;
        p.x = glm::clamp(p.x, -4900.0f, 4900.0f);
        p.z = glm::clamp(p.z, -4900.0f, 4900.0f);
        PlaceAt(p, std::atan2(rf.x, rf.z) + glm::pi<float>());
    }
    behaviour_ = Behaviour::COMING;
    behaviour_timer_ = 60.0f;
    call_whinny_timer_ = 0.7f;
}

void Horse::ToggleCamera() {
    if (ride_state_ != RideState::MOUNTED) return;
    if (camera_mode_ == CameraMode::THIRD_PERSON) {
        camera_mode_ = CameraMode::FIRST_PERSON;
        fp_yaw_ = glm::clamp(WrapAngle(cam_yaw_ - yaw_), -2.0f, 2.0f);
        fp_pitch_ = -0.12f;
    } else {
        camera_mode_ = CameraMode::THIRD_PERSON;
        cam_yaw_ = yaw_ + fp_yaw_;
        cam_focus_valid_ = false;
    }
}

void Horse::OnSceneExit() {
    HorseAudio::StopLoop(wind_loop_);
}

void Horse::RiderControl(Player::Controls c, float dt) {
    switch (c) {
        case Player::Controls::W:     in_forward_ = true; break;
        case Player::Controls::S:     in_back_ = true; break;
        case Player::Controls::A:     in_left_ = true; break;
        case Player::Controls::D:     in_right_ = true; break;
        case Player::Controls::SHIFT: in_sprint_ = true; break;
        case Player::Controls::CTRL:  in_walk_ = true; break;
        case Player::Controls::SPACE: in_jump_ = true; break;
        default: break;
    }
}

void Horse::RiderMouse(Mouse& mouse) {
    const float sens = 0.0024f;
    if (glm::length(mouse.move) > 0.0f) cam_idle_ = 0.0f;
    if (camera_mode_ == CameraMode::THIRD_PERSON) {
        cam_yaw_ -= mouse.move.x * sens;
        cam_pitch_ = glm::clamp(cam_pitch_ + mouse.move.y * sens, -0.12f, 1.2f);
    } else {
        fp_yaw_ = glm::clamp(fp_yaw_ - mouse.move.x * sens, -2.2f, 2.2f);
        fp_pitch_ = glm::clamp(fp_pitch_ - mouse.move.y * sens, -1.2f, 0.9f);
    }
}

void Horse::ShowMessage(const std::string& msg, float seconds) {
    message_ = msg;
    message_timer_ = seconds;
}

std::string Horse::PromptText() const {
    if (!rider_ || rider_->IsStatic()) return "";
    if (ride_state_ == RideState::FREE) {
        float d = DistanceToRider();
        if (d < MOUNT_RANGE) return " [F] Mount ";
        if (d > 300.0f) return " [B] Whistle for your horse ";
        return "";
    }
    if (ride_state_ != RideState::MOUNTED) return "";
    if (message_timer_ > 0.0f) return " " + message_ + " ";
    if (hint_timer_ > 0.0f) {
        return " W ride     SHIFT gallop     CTRL walk     S brake \n"
               " A/D steer  SPACE jump/rear  Z camera     F dismount ";
    }
    return "";
}

std::string Horse::StatusText() const {
    if (ride_state_ != RideState::MOUNTED) return "";
    std::string gait;
    float s = speed_;
    if (s < -2.0f) gait = "BACKING";
    else if (s < 3.0f) gait = "STANDING";
    else if (s < 75.0f) gait = "WALK";
    else if (s < 140.0f) gait = "TROT";
    else if (s < 205.0f) gait = "CANTER";
    else gait = "GALLOP";
    if (airborne_) gait = "JUMP";
    if (winded_) gait += "  - winded";

    const int bars = 24;
    int filled = (int)std::round(stamina_ / 100.0f * bars);
    std::string bar = "[" + std::string(filled, '#') + std::string(bars - filled, '-') + "]";
    std::string line1 = " " + gait;
    std::string line2 = " STAMINA " + bar + " ";
    line1 += std::string(std::max(0, (int)line2.size() - (int)line1.size()), ' ');
    return line1 + "\n" + line2;
}

// ------------------------------------------------------------------------------------------
// Update
// ------------------------------------------------------------------------------------------

void Horse::Update(double dtd) {
    float dt = std::min((float)dtd, 0.05f);
    if (dt <= 0.0f) {
        SceneNode::Update(dtd);
        return;
    }
    time_ += dt;
    message_timer_ -= dt;
    hint_timer_ -= dt;
    trauma_ = std::max(0.0f, trauma_ - dt * 1.5f);

    UpdateRideState(dt);

    bool locked = rider_ && rider_->IsStatic();
    target_speed_ = 0.0f;
    target_yaw_rate_ = 0.0f;
    braking_ = Damp(braking_, 0.0f, 6.0f, dt);
    if (ride_state_ == RideState::MOUNTED && !locked) {
        DriveFromInput(dt);
    } else if (ride_state_ == RideState::FREE) {
        UpdateBehaviour(dt);
    }
    if (rear_t_ >= 0.0f || stagger_ > 0.0f) {
        target_speed_ = 0.0f;
        target_yaw_rate_ = 0.0f;
    }
    stagger_ -= dt;

    Integrate(dt);

    // sliding stop: the hind hooves plough up a spray of sand
    skid_dust_timer_ -= dt;
    if (braking_ > 0.5f && speed_ > 80.0f && !airborne_ && skid_dust_timer_ <= 0.0f) {
        skid_dust_timer_ = 0.06f;
        SpawnDust(legs_[2].sole_world, 0.9f, 1.2f);
        SpawnDust(legs_[3].sole_world, 0.9f, 1.2f);
    }
    UpdateStamina(dt);

    // the player on foot can't walk through the horse
    if (ride_state_ == RideState::FREE && rider_) {
        glm::vec3 rp = rider_->transform.GetPosition();
        glm::vec3 f = Forward();
        for (int c = -1; c <= 1; c++) {
            glm::vec2 d(rp.x - (pos_.x + f.x * 8.0f * c), rp.z - (pos_.z + f.z * 8.0f * c));
            float dist = glm::length(d);
            float min_dist = 7.0f;
            if (dist < min_dist && dist > 1e-4f) {
                d *= (min_dist - dist) / dist;
                rp.x += d.x;
                rp.z += d.y;
            }
        }
        rider_->transform.SetPosition(rp);
    }
    Animate(dt);

    transform.SetPosition(pos_);
    transform.SetOrientation(glm::angleAxis(yaw_, kUp));
    SceneNode::Update(dtd);
    UpdateReins();

    if (rider_ && ride_state_ != RideState::FREE) {
        glm::vec3 p = pos_ + glm::vec3(0.0f, rider_->GetHeight(), 0.0f);
        rider_->SyncToMount(p, yaw_);
        last_rider_sync_ = p;
    }

    UpdateCamera(dt);
    UpdateAudio(dt);

    in_forward_ = in_back_ = in_left_ = in_right_ = in_sprint_ = in_walk_ = in_jump_ = false;
}

void Horse::UpdateRideState(float dt) {
    switch (ride_state_) {
        case RideState::MOUNTING:
            transition_ += dt / 0.9f;
            if (transition_ >= 1.0f) {
                transition_ = 1.0f;
                ride_state_ = RideState::MOUNTED;
            }
            break;
        case RideState::DISMOUNTING:
            transition_ += dt / 0.65f;
            if (transition_ >= 1.0f) {
                FinishDismount();
            }
            break;
        case RideState::MOUNTED:
            // something else moved the player (respawn, scene change): bring the horse along
            if (rider_) {
                glm::vec3 d = rider_->transform.GetPosition() - last_rider_sync_;
                if (glm::length(glm::vec2(d.x, d.z)) > 30.0f) {
                    PlaceAt(rider_->transform.GetPosition(), yaw_);
                }
            }
            break;
        default:
            break;
    }
}

void Horse::DriveFromInput(float dt) {
    bool go = in_forward_ || in_sprint_;
    if (go) {
        if (in_walk_) {
            target_speed_ = WALK_SPEED;
        } else if (in_sprint_) {
            target_speed_ = winded_ ? CANTER_SPEED : GALLOP_SPEED;
        } else {
            target_speed_ = TROT_SPEED;
        }
    } else if (in_back_) {
        if (speed_ > 8.0f) {
            braking_ = 1.0f;
            target_speed_ = 0.0f;
        } else {
            target_speed_ = -22.0f;
        }
    }

    float steer = (in_left_ ? 1.0f : 0.0f) - (in_right_ ? 1.0f : 0.0f);
    float sp = glm::clamp(std::abs(speed_) / GALLOP_SPEED, 0.0f, 1.0f);
    float turn = glm::mix(1.9f, 0.95f, sp);
    target_yaw_rate_ = steer * turn * (speed_ < -2.0f ? -1.0f : 1.0f);

    // like a real horse she picks her own way around cacti unless you steer her into them
    if ((speed_ > 20.0f || go) && steer == 0.0f) {
        float assist = AvoidObstacles(25.0f + speed_ * 0.45f);
        target_yaw_rate_ += glm::clamp(assist * 1.8f, -turn, turn);
    }

    if (in_jump_ && !airborne_ && rear_t_ < 0.0f) {
        HorseAudio* audio = game_->GetHorseAudio();
        if (speed_ > 60.0f && stamina_ > 8.0f) {
            airborne_ = true;
            air_time_ = 0.0f;
            vertical_velocity_ = JUMP_SPEED;
            stamina_ -= 10.0f;
            stamina_regen_delay_ = 1.0f;
            trauma_ += 0.12f;
            if (audio) audio->Play(HorseAudio::BREATH, pos_ + glm::vec3(0.0f, 18.0f, 0.0f), 0.9f, 1.1f);
        } else if (std::abs(speed_) < 25.0f) {
            rear_t_ = 0.0f;
            if (audio) audio->Play(HorseAudio::WHINNY, pos_ + glm::vec3(0.0f, 22.0f, 0.0f), 0.7f, 1.0f);
        }
    }
}

void Horse::SteerTowards(const glm::vec3& target, float speed, float dt) {
    glm::vec3 d = target - pos_;
    float desired = std::atan2(-d.x, -d.z);
    float err = WrapAngle(desired - yaw_) + AvoidObstacles();
    target_yaw_rate_ = glm::clamp(err * 2.2f, -1.8f, 1.8f);
    target_speed_ = speed * glm::clamp(1.0f - std::abs(err) / 1.4f, 0.15f, 1.0f);
}

float Horse::AvoidObstacles(float lookahead) {
    glm::vec3 f = Forward();
    glm::vec3 r = Right();
    float adjust = 0.0f;
    for (const Obstacle& o : obstacles_) {
        float dx = o.x - pos_.x;
        float dz = o.z - pos_.z;
        if (std::abs(dx) > o.radius + lookahead + 5.0f || std::abs(dz) > o.radius + lookahead + 5.0f) continue;
        float ahead = dx * f.x + dz * f.z;
        float lat = dx * r.x + dz * r.z;
        float range = lookahead + o.radius;
        if (ahead > 0.0f && ahead < range && std::abs(lat) < o.radius + 7.0f) {
            adjust += (lat > 0.0f ? 0.7f : -0.7f) * (1.0f - ahead / range);
        }
    }
    return glm::clamp(adjust, -1.2f, 1.2f);
}

void Horse::UpdateBehaviour(float dt) {
    float d = DistanceToRider();
    if (!met_rider_ && d < 160.0f) {
        met_rider_ = true;
        game_->HorseDiscovered();
    }

    behaviour_timer_ -= dt;
    switch (behaviour_) {
        case Behaviour::COMING: {
            float speed = d > 350.0f ? GALLOP_SPEED : d > 150.0f ? CANTER_SPEED : d > 60.0f ? TROT_SPEED : WALK_SPEED;
            SteerTowards(rider_->transform.GetPosition(), speed, dt);
            if (d < 24.0f || behaviour_timer_ < 0.0f) {
                behaviour_ = Behaviour::IDLE;
                behaviour_timer_ = 7.0f;
                HorseAudio* audio = game_->GetHorseAudio();
                if (audio) audio->Play(HorseAudio::SNORT, pos_ + glm::vec3(0.0f, 18.0f, 0.0f), 0.6f, 1.0f);
            }
            break;
        }
        case Behaviour::WANDER: {
            SteerTowards(wander_target_, WALK_SPEED * 0.7f, dt);
            glm::vec3 w = wander_target_ - pos_;
            if (glm::length(glm::vec2(w.x, w.z)) < 6.0f || behaviour_timer_ < 0.0f) {
                behaviour_ = Behaviour::IDLE;
                behaviour_timer_ = 3.0f + 4.0f * Rand01();
            }
            break;
        }
        case Behaviour::GRAZE:
            if (behaviour_timer_ < 0.0f || d < 35.0f) {
                behaviour_ = Behaviour::IDLE;
                behaviour_timer_ = 4.0f + 4.0f * Rand01();
            }
            break;
        case Behaviour::IDLE:
            if (behaviour_timer_ < 0.0f) {
                float r = Rand01();
                if (r < 0.45f && d > 40.0f) {
                    behaviour_ = Behaviour::GRAZE;
                    behaviour_timer_ = 6.0f + 6.0f * Rand01();
                } else if (r < 0.65f && d > 40.0f) {
                    float a = Rand01() * TAU;
                    float dist = 20.0f + 35.0f * Rand01();
                    wander_target_ = pos_ + glm::vec3(std::sin(a), 0.0f, std::cos(a)) * dist;
                    behaviour_ = Behaviour::WANDER;
                    behaviour_timer_ = 12.0f;
                } else if (r < 0.82f) {
                    head_toss_ = 0.3f;
                    HorseAudio* audio = game_->GetHorseAudio();
                    if (audio) audio->Play(HorseAudio::SNORT, pos_ + glm::vec3(0.0f, 18.0f, 0.0f), 0.6f, 0.95f + 0.1f * Rand01());
                    behaviour_timer_ = 3.0f + 3.0f * Rand01();
                } else {
                    tail_swish_ = 1.0f;
                    behaviour_timer_ = 4.0f + 3.0f * Rand01();
                }
            }
            break;
    }

    // keep an eye on the player when they're close
    float look = 0.0f;
    if (d < 90.0f && behaviour_ != Behaviour::GRAZE && rider_) {
        glm::vec3 to = rider_->transform.GetPosition() - pos_;
        look = glm::clamp(WrapAngle(std::atan2(-to.x, -to.z) - yaw_), -1.1f, 1.1f);
    }
    look_yaw_ = Damp(look_yaw_, look, 3.0f, dt);
}

void Horse::Integrate(float dt) {
    float old_speed = speed_;
    if (!airborne_) {
        float rate;
        if (std::abs(target_speed_) > std::abs(speed_) && target_speed_ * speed_ >= 0.0f) {
            rate = std::abs(speed_) < TROT_SPEED ? 110.0f : 75.0f;
        } else {
            rate = braking_ > 0.5f ? 210.0f : 85.0f;
        }
        speed_ = Approach(speed_, target_speed_ * slope_factor_, rate * dt);
    }
    accel_ = Damp(accel_, (speed_ - old_speed) / dt, 8.0f, dt);

    float steer_rate = airborne_ ? 2.0f : 7.0f;
    yaw_rate_ = Damp(yaw_rate_, airborne_ ? yaw_rate_ * 0.5f : target_yaw_rate_, steer_rate, dt);
    yaw_ = WrapAngle(yaw_ + yaw_rate_ * dt);
    steer_visual_ = Damp(steer_visual_, glm::clamp(yaw_rate_, -1.5f, 1.5f), 5.0f, dt);

    pos_ += Forward() * speed_ * dt;

    float impact = 0.0f;
    ResolveCollisions(impact);
    if (impact > 0.0f) OnImpact(impact);

    pos_.x = glm::clamp(pos_.x, -4920.0f, 4920.0f);
    pos_.z = glm::clamp(pos_.z, -4920.0f, 4920.0f);

    float g = Ground(pos_.x, pos_.z);
    if (airborne_) {
        air_time_ += dt;
        vertical_velocity_ -= GRAVITY * dt;
        pos_.y += vertical_velocity_ * dt;
        if (pos_.y <= g && vertical_velocity_ < 0.0f) {
            float hit = -vertical_velocity_;
            pos_.y = g;
            airborne_ = false;
            vertical_velocity_ = 0.0f;
            speed_ *= 0.94f;
            land_squash_vel_ = -hit * 0.06f;
            trauma_ += 0.25f + hit * 0.004f;
            HorseAudio* audio = game_->GetHorseAudio();
            if (audio) audio->Play(HorseAudio::LANDING, pos_, 0.75f, 0.95f + 0.1f * Rand01());
            for (Leg& L : legs_) SpawnDust(L.sole_world, 1.0f, 1.3f);
        }
    } else {
        pos_.y = g;
    }

    // climbing a dune slows the horse down, running down one speeds it up a touch
    glm::vec3 f = Forward();
    float ahead = Ground(pos_.x + f.x * 6.0f, pos_.z + f.z * 6.0f);
    float behind = Ground(pos_.x - f.x * 6.0f, pos_.z - f.z * 6.0f);
    float slope = (ahead - behind) / 12.0f * (speed_ >= 0.0f ? 1.0f : -1.0f);
    float factor = slope > 0.0f ? glm::clamp(1.0f - slope * 1.1f, 0.4f, 1.0f) : 1.0f + std::min(-slope, 0.5f) * 0.2f;
    slope_factor_ = Damp(slope_factor_, factor, 4.0f, dt);
}

void Horse::ResolveCollisions(float& impact) {
    const float R = 4.6f;
    glm::vec3 f = Forward();
    for (const Obstacle& o : obstacles_) {
        if (std::abs(o.x - pos_.x) > o.radius + 15.0f || std::abs(o.z - pos_.z) > o.radius + 15.0f) continue;
        if (pos_.y > o.top - 1.0f) continue; // sailing over it
        for (int c = -1; c <= 1; c++) {
            glm::vec2 center(pos_.x + f.x * 8.0f * c, pos_.z + f.z * 8.0f * c);
            glm::vec2 d = center - glm::vec2(o.x, o.z);
            float dist = glm::length(d);
            float min_dist = o.radius + R;
            if (dist >= min_dist || dist < 1e-4f) continue;
            glm::vec2 n = d / dist;
            pos_.x += n.x * (min_dist - dist);
            pos_.z += n.y * (min_dist - dist);
            glm::vec2 v = glm::vec2(f.x, f.z) * speed_;
            float vn = glm::dot(v, n);
            if (vn < 0.0f) {
                impact = std::max(impact, -vn);
                v -= n * vn;
                speed_ = glm::dot(v, glm::vec2(f.x, f.z));
            }
        }
    }
}

void Horse::OnImpact(float impact) {
    HorseAudio* audio = game_->GetHorseAudio();
    glm::vec3 head = pos_ + glm::vec3(0.0f, 20.0f, 0.0f) + Forward() * 14.0f;
    if (impact > 120.0f && rear_t_ < 0.0f) {
        rear_t_ = 0.0f;
        stagger_ = 1.4f;
        speed_ *= 0.25f;
        trauma_ += 0.7f;
        if (audio) audio->Play(HorseAudio::WHINNY, head, 0.75f, 1.05f);
    } else if (impact > 45.0f) {
        head_toss_ = 0.35f;
        trauma_ += 0.3f;
        speed_ *= 0.7f;
        if (audio) audio->Play(HorseAudio::SNORT, head, 0.8f, 1.1f);
    }
}

void Horse::UpdateStamina(float dt) {
    bool galloping = ride_state_ == RideState::MOUNTED && speed_ > CANTER_SPEED + 20.0f;
    if (galloping) {
        stamina_ -= 11.0f * dt;
        stamina_regen_delay_ = 1.0f;
    } else {
        stamina_regen_delay_ -= dt;
        if (stamina_regen_delay_ <= 0.0f) {
            stamina_ += (std::abs(speed_) < TROT_SPEED + 5.0f ? 16.0f : 6.0f) * dt;
        }
    }
    stamina_ = glm::clamp(stamina_, 0.0f, 100.0f);
    if (stamina_ <= 0.0f && !winded_) {
        winded_ = true;
        ShowMessage("Your horse is winded - ease off to let her recover", 3.0f);
        HorseAudio* audio = game_->GetHorseAudio();
        if (audio) audio->Play(HorseAudio::SNORT, pos_ + glm::vec3(0.0f, 18.0f, 0.0f), 0.9f, 0.9f);
    } else if (winded_ && stamina_ > 35.0f) {
        winded_ = false;
    }
    float effort = glm::clamp((std::abs(speed_) - TROT_SPEED) / (GALLOP_SPEED - TROT_SPEED), 0.0f, 1.0f);
    exertion_ = effort > exertion_ ? Damp(exertion_, effort, 0.4f, dt) : Damp(exertion_, effort, 0.12f, dt);
}

// ------------------------------------------------------------------------------------------
// Animation
// ------------------------------------------------------------------------------------------

void Horse::Animate(float dt) {
    // blend the gait table by how fast the legs need to move
    float anim = std::max(std::abs(speed_), std::abs(yaw_rate_) * 14.0f);
    float a = std::min(anim, GAITS[NUM_GAITS - 1].speed);
    int k = 0;
    while (k < NUM_GAITS - 2 && a > GAITS[k + 1].speed) k++;
    const GaitKey& g0 = GAITS[k];
    const GaitKey& g1 = GAITS[k + 1];
    float t = glm::clamp((a - g0.speed) / (g1.speed - g0.speed), 0.0f, 1.0f);
    t = t * t * (3.0f - 2.0f * t);

    duty_ = glm::mix(g0.duty, g1.duty, t);
    float stride = glm::mix(g0.stride, g1.stride, t);
    float comp = glm::mix(g0.comp, g1.comp, t);
    float rock = glm::mix(g0.rock, g1.rock, t);
    float offsets[4];
    for (int i = 0; i < 4; i++) {
        float d = g1.offset[i] - g0.offset[i];
        d -= std::round(d);
        offsets[i] = g0.offset[i] + d * t;
    }
    gait_trot_ = Tent(anim, WALK_SPEED, TROT_SPEED, CANTER_SPEED);
    gait_canter_ = Tent(anim, TROT_SPEED, CANTER_SPEED, GALLOP_SPEED);
    gait_gallop_ = glm::smoothstep(CANTER_SPEED, GALLOP_SPEED, anim);

    float move_w = glm::smoothstep(2.0f, 14.0f, anim);
    float dir = speed_ < -1.0f ? -1.0f : 1.0f;
    float old_phase = phase_;
    phase_ = Fract(phase_ + dir * anim / stride * dt);
    if (gait_gallop_ > 0.5f && phase_ < old_phase && dir > 0.0f && ride_state_ == RideState::MOUNTED) {
        // horses breathe in step with the gallop stride
        HorseAudio* audio = game_->GetHorseAudio();
        if (audio) audio->Play(HorseAudio::BREATH, pos_ + glm::vec3(0.0f, 18.0f, 0.0f) + Forward() * 16.0f, 0.2f, 1.1f);
    }

    // leg loading drives the bob and the rocking of the body
    float front_load = 0.0f, hind_load = 0.0f;
    for (int i = 0; i < 4; i++) {
        float p = Fract(phase_ + offsets[i]);
        if (p < duty_) {
            float load = std::sin(glm::pi<float>() * p / duty_);
            (legs_[i].front ? front_load : hind_load) += load;
        }
    }
    float mean = 4.0f * duty_ * (2.0f / glm::pi<float>());
    float old_bob = bob_;
    bob_ = Damp(bob_, -comp * (front_load + hind_load - mean) * move_w, 25.0f, dt);
    bob_velocity_ = (bob_ - old_bob) / dt;
    pitch_osc_ = Damp(pitch_osc_, rock * (hind_load - front_load) * move_w, 20.0f, dt);

    // terrain under the hooves
    glm::vec3 f = Forward(), r = Right();
    float hf = Ground(pos_.x + f.x * 8.5f, pos_.z + f.z * 8.5f);
    float hb = Ground(pos_.x - f.x * 8.5f, pos_.z - f.z * 8.5f);
    float hl = Ground(pos_.x - r.x * 2.5f, pos_.z - r.z * 2.5f);
    float hr = Ground(pos_.x + r.x * 2.5f, pos_.z + r.z * 2.5f);
    float terrain_pitch = std::atan2(hf - hb, 17.0f);
    float terrain_roll = std::atan2(hr - hl, 5.0f) * 0.5f;
    float ground_offset = airborne_ ? 0.0f : (hf + hb) * 0.5f - pos_.y;

    float accel_pitch = glm::clamp(accel_ * 0.0008f, -0.05f, 0.08f) + braking_ * std::min(speed_ / 150.0f, 1.0f) * 0.2f;
    float jump_pitch = airborne_ ? glm::clamp(vertical_velocity_ / JUMP_SPEED, -1.0f, 1.0f) * 0.22f : 0.0f;
    float lean = glm::clamp(yaw_rate_ * std::abs(speed_) * 0.0011f, -0.22f, 0.22f);

    body_height_ = Damp(body_height_, ground_offset, 10.0f, dt);
    body_pitch_ = Damp(body_pitch_, terrain_pitch + accel_pitch + jump_pitch, 6.0f, dt);
    body_roll_ = Damp(body_roll_, terrain_roll + lean, 5.0f, dt);

    land_squash_vel_ += (-70.0f * land_squash_ - 10.0f * land_squash_vel_) * dt;
    land_squash_ += land_squash_vel_ * dt;
    jump_pose_ = Damp(jump_pose_, airborne_ ? 1.0f : 0.0f, airborne_ ? 12.0f : 9.0f, dt);

    // rearing up on the hind legs
    float rear = 0.0f;
    if (rear_t_ >= 0.0f) {
        rear_t_ += dt;
        if (rear_t_ < 0.35f) rear = glm::smoothstep(0.0f, 0.35f, rear_t_);
        else if (rear_t_ < 1.0f) rear = 1.0f;
        else if (rear_t_ < 1.6f) rear = 1.0f - glm::smoothstep(1.0f, 1.6f, rear_t_);
        else rear_t_ = -1.0f;
    }

    breath_phase_ += dt * (1.5f + 4.0f * exertion_);
    float breath = std::sin(breath_phase_);
    body_->transform.SetScale(glm::vec3(1.0f + (0.01f + 0.025f * exertion_) * breath, 1.0f + (0.006f + 0.015f * exertion_) * breath, 1.0f));

    glm::vec3 body_pos(0.0f,
        BODY_HEIGHT + body_height_ - 0.6f * gait_gallop_ - 0.3f * gait_canter_ + bob_ + land_squash_ + 4.2f * rear,
        2.0f * rear);
    glm::quat body_rot = glm::angleAxis(body_pitch_ + pitch_osc_ + 0.72f * rear, glm::vec3(1.0f, 0.0f, 0.0f))
                       * glm::angleAxis(body_roll_, glm::vec3(0.0f, 0.0f, 1.0f));
    body_pivot_->transform.SetPosition(body_pos);
    body_pivot_->transform.SetOrientation(body_rot);

    AnimateLegs(dt, body_rot, body_pos);
    AnimateNeckAndHead(dt);
    AnimateHair(dt);
    AnimateRider(dt, body_rot);

}

void Horse::AnimateLegs(float dt, const glm::quat& body_rot, const glm::vec3& body_pos) {
    // recompute the blended gait (cheap) so the leg pass has the same values Animate used
    float anim = std::max(std::abs(speed_), std::abs(yaw_rate_) * 14.0f);
    float a = std::min(anim, GAITS[NUM_GAITS - 1].speed);
    int k = 0;
    while (k < NUM_GAITS - 2 && a > GAITS[k + 1].speed) k++;
    const GaitKey& g0 = GAITS[k];
    const GaitKey& g1 = GAITS[k + 1];
    float t = glm::clamp((a - g0.speed) / (g1.speed - g0.speed), 0.0f, 1.0f);
    t = t * t * (3.0f - 2.0f * t);
    float stride = glm::mix(g0.stride, g1.stride, t);
    float lift = glm::mix(g0.lift, g1.lift, t);
    float move_w = glm::smoothstep(2.0f, 14.0f, anim);

    glm::quat inv_body = glm::inverse(body_rot);
    glm::vec3 fwd_body = inv_body * kFwd;
    glm::quat ground_rot = DirRot(inv_body * glm::vec3(0.0f, -1.0f, 0.0f), fwd_body);
    float rear = rear_t_ >= 0.0f ? glm::clamp(1.0f - std::abs(rear_t_ - 0.7f) / 0.9f, 0.0f, 1.0f) : 0.0f;

    for (int i = 0; i < 4; i++) {
        Leg& L = legs_[i];
        float d = g1.offset[i] - g0.offset[i];
        d -= std::round(d);
        float p = Fract(phase_ + g0.offset[i] + d * t);
        bool stance = p < duty_;
        float sweep = stride * duty_ * move_w;
        float x, up = 0.0f, psi, hoof_follow;
        if (stance) {
            float s = p / duty_;
            x = sweep * (0.5f - s);
            psi = 0.55f + 0.45f * s * s;          // heel peels off at the end of stance
            hoof_follow = glm::smoothstep(0.75f, 1.0f, s) * 0.5f * move_w;
        } else {
            float s = (p - duty_) / (1.0f - duty_);
            float e = s * s * (3.0f - 2.0f * s);
            x = sweep * (-0.5f + e);
            up = lift * move_w * std::sin(glm::pi<float>() * std::pow(s, L.front ? 0.75f : 0.9f));
            float fold = std::sin(glm::pi<float>() * std::min(1.0f, s * 1.15f)) * move_w;
            psi = glm::mix(glm::mix(1.0f, 0.55f, e), L.front ? -1.1f : -0.6f, fold);
            hoof_follow = 0.5f + 0.5f * fold;
        }

        glm::vec3 sole(L.side * (L.front ? 2.2f : 2.4f), 0.0f, L.nominal_z - x);
        glm::vec3 sole_world = ToWorld(sole);
        sole.y = Ground(sole_world.x, sole_world.z) - pos_.y + up;
        L.sole_world = ToWorld(sole);

        glm::vec3 coronet = sole + glm::vec3(0.0f, HOOF_HEIGHT, 0.0f);
        glm::vec3 fetlock = coronet + PASTERN * glm::vec3(0.0f, std::cos(psi), std::sin(psi));
        glm::vec3 fet_b = inv_body * (fetlock - body_pos);
        glm::vec3 cor_b = inv_body * (coronet - body_pos);

        // jump: forelegs tuck up, hind legs trail behind
        if (jump_pose_ > 0.01f) {
            glm::vec3 tuck = L.front ? L.hip + glm::vec3(0.0f, -4.4f, 1.8f) : L.hip + glm::vec3(0.0f, -7.6f, 4.6f);
            glm::vec3 tuck_cor = tuck + (L.front ? glm::vec3(0.0f, -0.4f, -1.2f) : glm::vec3(0.0f, -1.2f, 0.4f));
            fet_b = glm::mix(fet_b, tuck, jump_pose_);
            cor_b = glm::mix(cor_b, tuck_cor, jump_pose_);
        }
        // rearing: forelegs paw the air
        if (rear > 0.0f && L.front) {
            float paw = std::sin(time_ * 7.0f + (L.side > 0.0f ? 1.6f : 0.0f));
            glm::vec3 pose = L.hip + glm::vec3(0.0f, -4.2f + 1.3f * paw, -2.2f + 1.2f * paw);
            fet_b = glm::mix(fet_b, pose, rear);
            cor_b = glm::mix(cor_b, pose + glm::vec3(0.0f, -0.5f, 1.2f), rear);
        }

        glm::vec3 pole = L.front ? glm::vec3(0.0f, 0.0f, -1.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 fet_solved;
        glm::vec3 knee = SolveTwoBone(L.hip, fet_b, L.upper_len, L.lower_len, pole, fet_solved);

        glm::quat ru = BoneRot(L.hip, knee, fwd_body);
        glm::quat rl = BoneRot(knee, fet_solved, fwd_body);
        glm::quat rp = BoneRot(fet_solved, cor_b, fwd_body);
        glm::quat rh = glm::slerp(ground_rot, rp, glm::clamp(hoof_follow + jump_pose_ + rear * (L.front ? 1.0f : 0.0f), 0.0f, 1.0f));

        L.upper->transform.SetPosition(L.hip);
        L.upper->transform.SetOrientation(ru);
        L.lower->transform.SetOrientation(glm::inverse(ru) * rl);
        L.pastern->transform.SetOrientation(glm::inverse(rl) * rp);
        L.hoof->transform.SetOrientation(glm::inverse(rp) * rh);

        bool grounded = stance && !airborne_ && !(rear > 0.3f && L.front);
        if (grounded && !L.in_stance && move_w > 0.05f) {
            OnHoofStrike(L);
        }
        L.in_stance = grounded;
    }
}

void Horse::AnimateNeckAndHead(float dt) {
    float target = 0.72f;
    target = glm::mix(target, 0.62f, gait_trot_);
    target = glm::mix(target, 0.55f, gait_canter_);
    target = glm::mix(target, 0.42f, gait_gallop_);
    target -= graze_ * 1.27f;
    if (rear_t_ >= 0.0f) target += 0.35f;
    target += winded_ ? -0.15f : 0.0f;
    neck_pitch_ = Damp(neck_pitch_, target, 3.5f, dt);

    // head toss spring
    head_toss_ = Damp(head_toss_, 0.0f, 3.0f, dt);
    float toss = head_toss_ * std::sin(time_ * 14.0f);

    graze_ = Damp(graze_, behaviour_ == Behaviour::GRAZE && ride_state_ == RideState::FREE ? 1.0f : 0.0f, 1.3f, dt);

    float walk_w = Tent(std::abs(speed_), 0.0f, WALK_SPEED, TROT_SPEED);
    float nod = -pitch_osc_ * 1.8f + walk_w * 0.06f * std::sin(2.0f * TAU * phase_ + 1.0f) + toss;
    float neck_yaw = look_yaw_ * 0.45f + steer_visual_ * 0.22f;
    neck_->transform.SetOrientation(glm::angleAxis(neck_yaw, kUp) * glm::angleAxis(neck_pitch_ + nod, glm::vec3(1.0f, 0.0f, 0.0f)));

    float head_pitch = -1.62f + (0.72f - neck_pitch_) * 0.55f + gait_gallop_ * 0.25f - nod * 0.6f;
    head_pitch += 0.05f * std::sin(time_ * 9.0f) * graze_;   // chewing
    head_->transform.SetOrientation(glm::angleAxis(look_yaw_ * 0.55f + steer_visual_ * 0.1f, kUp) * glm::angleAxis(head_pitch, glm::vec3(1.0f, 0.0f, 0.0f)));

    // ears: forward when moving or watching something, random flicks, back when winded
    bool alert = std::abs(speed_) > 5.0f || std::abs(look_yaw_) > 0.1f;
    ear_alert_ = Damp(ear_alert_, winded_ ? 0.0f : (alert ? 1.0f : 0.35f), 2.0f, dt);
    for (int s = 0; s < 2; s++) {
        float side = s == 0 ? -1.0f : 1.0f;
        if (Rand01() < dt * 0.25f) ear_flick_[s] = 1.0f;
        ear_flick_[s] = Damp(ear_flick_[s], 0.0f, 5.0f, dt);
        float flick = ear_flick_[s] * std::sin(time_ * 25.0f);
        glm::vec3 d = glm::normalize(glm::vec3(0.32f * side + 0.3f * flick, 0.62f + 0.25f * ear_alert_, 0.72f - 0.55f * ear_alert_ + 0.5f * flick));
        ears_[s]->transform.SetOrientation(DirRot(-d, glm::vec3(0.0f, -1.0f, 0.0f)));
    }
}

void Horse::AnimateHair(float dt) {
    // hair hangs with world gravity, not the (possibly pitched) body
    glm::vec3 down = glm::inverse(body_pivot_->transform.GetOrientation()) * glm::vec3(0.0f, -1.0f, 0.0f);
    float wind = glm::clamp(std::abs(speed_) / GALLOP_SPEED, 0.0f, 1.0f) + (airborne_ ? 0.3f : 0.0f);
    glm::quat neck_rot = neck_->transform.GetOrientation();
    glm::quat inv_neck = glm::inverse(neck_rot);
    glm::vec3 neck_side = neck_rot * glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 neck_back = neck_rot * glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 neck_fwd = -neck_back;

    for (int i = 0; i < MANE_LOCKS; i++) {
        float target = wind * 0.85f;
        float acc = 70.0f * (target - mane_blow_[i]) - 8.0f * mane_vel_[i] - bob_velocity_ * 0.35f * (1.0f + i * 0.08f);
        mane_vel_[i] += acc * dt;
        mane_blow_[i] += mane_vel_[i] * dt;
        float b = glm::clamp(mane_blow_[i], -0.3f, 1.2f);
        float flutter = wind * 0.12f * std::sin(time_ * 13.0f + i * 0.9f);
        glm::vec3 d = neck_side * (0.55f * (1.0f - 0.4f * b) + flutter)
                    + down * (0.9f - b * 0.7f)
                    + neck_back * (b * 1.2f)
                    - down * (b * 0.25f);
        mane_[i]->transform.SetOrientation(inv_neck * DirRot(d, neck_fwd));
    }

    // forelock hangs down the face
    forelock_->transform.SetOrientation(DirRot(glm::vec3(0.06f * std::sin(time_ * 3.0f), 0.3f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    // tail: dock lifts with the gait, hair streams back and swishes side to side
    if (ride_state_ == RideState::FREE) {
        tail_swish_timer_ -= dt;
        if (tail_swish_timer_ < 0.0f) {
            tail_swish_ = 1.0f;
            tail_swish_timer_ = 3.0f + 6.0f * Rand01();
        }
    }
    tail_swish_ = Damp(tail_swish_, 0.0f, 0.8f, dt);
    float raise = gait_gallop_ * 0.6f + gait_canter_ * 0.45f + gait_trot_ * 0.25f + (airborne_ ? 0.3f : 0.0f);
    glm::vec3 dock = glm::normalize(glm::vec3(0.0f, -0.75f + raise, 0.65f + 0.2f * raise));
    glm::quat parent = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < TAIL_SEGMENTS; i++) {
        float target = i == 0 ? 0.0f : wind * 0.9f * (float)i / (TAIL_SEGMENTS - 1);
        float acc = 60.0f * (target - tail_lift_[i]) - 9.0f * tail_vel_[i] + bob_velocity_ * 0.06f * i;
        tail_vel_[i] += acc * dt;
        tail_lift_[i] += tail_vel_[i] * dt;
        float lateral = tail_swish_ * 0.8f * std::sin(time_ * 6.5f - i * 0.7f) + steer_visual_ * -0.15f * i;
        glm::vec3 d = glm::mix(dock, down, std::min(1.0f, i * 0.24f))
                    + glm::vec3(0.0f, 0.15f, 1.0f) * tail_lift_[i] * 1.2f
                    + glm::vec3(1.0f, 0.0f, 0.0f) * lateral;
        glm::quat rot = DirRot(d, glm::vec3(1.0f, 0.0f, 0.0f));
        tail_[i]->transform.SetOrientation(glm::inverse(parent) * rot);
        parent = rot;
    }
}

void Horse::AnimateRider(float dt, const glm::quat& body_rot) {
    if (ride_state_ == RideState::FREE) return;

    float k = glm::smoothstep(0.0f, 1.0f, transition_);
    float seated = ride_state_ == RideState::MOUNTING ? k : ride_state_ == RideState::DISMOUNTING ? 1.0f - k : 1.0f;

    // posting trot: the rider rises every other beat
    float post = gait_trot_ * std::max(0.0f, std::sin(2.0f * TAU * phase_ + 0.6f)) * 0.8f;
    posting_ = Damp(posting_, post, 18.0f, dt);
    float lean_target = 0.08f + 0.12f * gait_trot_ + 0.2f * gait_canter_ + 0.34f * gait_gallop_
                      - 0.25f * braking_ + (rear_t_ >= 0.0f ? 0.5f : 0.0f) + (airborne_ ? 0.25f : 0.0f);
    rider_lean_ = Damp(rider_lean_, lean_target, 5.0f, dt);

    glm::vec3 hop = glm::vec3(-7.0f, -10.0f, 0.0f) * (1.0f - seated);
    glm::vec3 root = SEAT + glm::vec3(0.0f, posting_ - bob_ * 0.3f, 0.0f) + hop;
    rider_root_->transform.SetPosition(root);

    glm::quat torso = glm::angleAxis(steer_visual_ * 0.12f, kUp)
                    * glm::angleAxis(-rider_lean_ - pitch_osc_ * 0.8f, glm::vec3(1.0f, 0.0f, 0.0f))
                    * glm::angleAxis(-body_roll_ * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
    rider_torso_->transform.SetOrientation(torso);

    float look_yaw = camera_mode_ == CameraMode::FIRST_PERSON ? glm::clamp(fp_yaw_, -1.2f, 1.2f) : steer_visual_ * 0.3f;
    float look_pitch = camera_mode_ == CameraMode::FIRST_PERSON ? fp_pitch_ * 0.6f : 0.0f;
    rider_head_->transform.SetOrientation(glm::angleAxis(look_yaw, kUp) * glm::angleAxis(rider_lean_ * 0.7f + look_pitch, glm::vec3(1.0f, 0.0f, 0.0f)));
    rider_head_->visible = !(camera_mode_ == CameraMode::FIRST_PERSON && ride_state_ != RideState::DISMOUNTING);

    // hands hold the reins just above the withers and give with the head nod
    glm::quat neck_rot = neck_->transform.GetOrientation();
    glm::quat head_rot = neck_rot * head_->transform.GetOrientation();
    glm::vec3 neck_base(0.0f, 2.3f, -8.7f);
    for (int s = 0; s < 2; s++) {
        float side = s == 0 ? -1.0f : 1.0f;
        glm::vec3 bit = neck_base + neck_rot * glm::vec3(0.0f, 0.0f, -10.3f) + head_rot * glm::vec3(0.95f * side, -0.85f, -5.6f);
        glm::vec3 rest(0.85f * side, 7.4f, -6.0f);
        glm::vec3 hand = glm::mix(rest, bit, 0.12f);
        float pull = steer_visual_ * side < 0.0f ? std::abs(steer_visual_) : 0.0f; // inside rein pulls back
        hand += glm::vec3(0.4f * side * pull, 0.0f, 1.4f * pull + 1.0f * braking_);

        RiderLimb& A = rider_arms_[s];
        glm::vec3 shoulder = root + torso * A.root;
        glm::vec3 hand_solved;
        glm::vec3 elbow = SolveTwoBone(shoulder, hand, A.upper_len, A.lower_len, glm::vec3(side, -0.5f, 1.0f), hand_solved);
        glm::quat ru = BoneRot(shoulder, elbow, kFwd);
        glm::quat rl = BoneRot(elbow, hand_solved, kFwd);
        glm::quat rh = BoneRot(hand_solved, bit, glm::vec3(0.0f, 1.0f, 0.0f));
        A.upper->transform.SetOrientation(glm::inverse(torso) * ru);
        A.lower->transform.SetOrientation(glm::inverse(ru) * rl);
        A.end->transform.SetOrientation(glm::inverse(rl) * glm::slerp(rl, rh, 0.5f));

        // legs wrap the barrel with feet in the stirrups
        RiderLimb& L = rider_legs_[s];
        glm::vec3 hip = root + L.root;
        glm::vec3 ankle = glm::vec3(5.0f * side, -2.4f, -2.1f);
        ankle = glm::mix(ankle, root + glm::vec3(1.5f * side, -9.0f, 0.0f), 1.0f - seated);
        glm::vec3 ankle_solved;
        glm::vec3 knee = SolveTwoBone(hip, ankle, L.upper_len, L.lower_len, glm::vec3(side, 0.0f, -0.4f), ankle_solved);
        glm::quat tu = BoneRot(hip, knee, kFwd);
        glm::quat tl = BoneRot(knee, ankle_solved, kFwd);
        L.upper->transform.SetOrientation(tu);
        L.lower->transform.SetOrientation(glm::inverse(tu) * tl);
    }
}

void Horse::UpdateReins() {
    glm::mat4 world = transform.GetWorldMatrix();
    glm::mat4 inv = glm::inverse(world);
    glm::mat4 head = head_->transform.GetWorldMatrix();
    for (int s = 0; s < 2; s++) {
        float side = s == 0 ? -1.0f : 1.0f;
        glm::vec3 bit = Point(head, glm::vec3(0.95f * side, -0.85f, -5.6f));
        glm::vec3 hand;
        if (ride_state_ != RideState::FREE) {
            hand = Point(rider_arms_[s].end->transform.GetWorldMatrix(), glm::vec3(0.0f, -0.55f, 0.0f));
        } else {
            hand = Point(body_pivot_->transform.GetWorldMatrix(), glm::vec3(0.25f * side, 6.2f, -5.0f)); // looped over the horn
        }
        glm::vec3 a = Point(inv, hand);
        glm::vec3 b = Point(inv, bit);
        float len = glm::length(b - a);
        reins_[s]->transform.SetPosition(a);
        reins_[s]->transform.SetOrientation(BoneRot(a, b, kUp));
        reins_[s]->transform.SetScale(glm::vec3(1.0f, std::max(len, 0.01f), 1.0f));
        reins_[s]->transform.Update(world);
    }
}

void Horse::OnHoofStrike(Leg& leg) {
    float sp = std::abs(speed_);
    HorseAudio* audio = game_->GetHorseAudio();
    if (audio) {
        float vol = 0.15f + 0.5f * glm::clamp(sp / GALLOP_SPEED, 0.0f, 1.0f);
        audio->Play(HorseAudio::HOOF_SAND, leg.sole_world, vol, 0.9f + 0.18f * Rand01() + (leg.front ? 0.05f : 0.0f));
    }
    if (sp > 55.0f) {
        SpawnDust(leg.sole_world, glm::clamp((sp - 40.0f) / 200.0f, 0.1f, 1.0f), 0.6f + 0.6f * sp / GALLOP_SPEED);
    }
    if (ride_state_ == RideState::MOUNTED && sp > 150.0f) {
        trauma_ += camera_mode_ == CameraMode::FIRST_PERSON ? 0.05f : 0.025f;
    }
}

void Horse::SpawnDust(const glm::vec3& at, float intensity, float scale) {
    dust_.erase(std::remove_if(dust_.begin(), dust_.end(), [](const std::weak_ptr<SceneNode>& w) {
        auto p = w.lock();
        return !p || p->deleted;
    }), dust_.end());
    if (dust_.size() > 70) return;
    auto d = std::make_shared<Dust>(1.4f + 0.6f * intensity, intensity);
    d->transform.SetPosition(at + glm::vec3(0.0f, 0.3f, 0.0f));
    d->transform.SetOrientation(glm::angleAxis(yaw_ + (Rand01() - 0.5f) * 0.6f, kUp));
    d->transform.SetScale(glm::vec3(scale));
    game_->ActiveScene().AddNode(d);
    dust_.push_back(d);
}

// ------------------------------------------------------------------------------------------
// Camera
// ------------------------------------------------------------------------------------------

void Horse::ThirdPersonPose(float dt, glm::vec3& eye, glm::quat& rot, float& fov) {
    glm::vec3 f = Forward();
    glm::vec3 focus_target = pos_ + glm::vec3(0.0f, 20.5f + body_height_, 0.0f);
    if (!cam_focus_valid_) {
        cam_focus_ = focus_target;
        cam_focus_valid_ = true;
    }
    // a little lag sells the speed, too much loses the horse
    cam_focus_.x = Damp(cam_focus_.x, focus_target.x, 30.0f, dt);
    cam_focus_.z = Damp(cam_focus_.z, focus_target.z, 30.0f, dt);
    cam_focus_.y = Damp(cam_focus_.y, focus_target.y, 6.0f, dt);

    // drift back behind the horse when the player isn't steering the camera
    if (cam_idle_ > 1.4f && std::abs(speed_) > 15.0f) {
        float rate = 1.0f - std::exp(-1.6f * dt);
        cam_yaw_ += WrapAngle(yaw_ - cam_yaw_) * rate;
        cam_pitch_ = Damp(cam_pitch_, 0.2f, 1.2f, dt);
    }

    float sp = glm::clamp(std::abs(speed_) / GALLOP_SPEED, 0.0f, 1.0f);
    float dist = 44.0f + 8.0f * sp + (rear_t_ >= 0.0f ? 8.0f : 0.0f);
    glm::vec3 dir(std::sin(cam_yaw_) * std::cos(cam_pitch_), std::sin(cam_pitch_), std::cos(cam_yaw_) * std::cos(cam_pitch_));
    eye = cam_focus_ + dir * dist;
    KeepCameraClear(eye);
    float clearance = Ground(eye.x, eye.z) + 5.0f;
    if (eye.y < clearance) eye.y = clearance;
    glm::vec3 look = cam_focus_ + f * (speed_ * 0.06f);
    rot = glm::normalize(glm::quat_cast(glm::inverse(glm::lookAt(eye, look, kUp))));
    fov = 74.0f + 16.0f * sp * sp;
}

void Horse::FirstPersonPose(float dt, glm::vec3& eye, glm::quat& rot, float& fov) {
    eye = Point(rider_head_->transform.GetWorldMatrix(), glm::vec3(0.0f, 1.45f, -0.75f));
    if (cam_idle_ > 1.5f && std::abs(speed_) > 15.0f) {
        fp_yaw_ = Damp(fp_yaw_, 0.0f, 1.5f, dt);
        fp_pitch_ = Damp(fp_pitch_, -0.12f, 1.5f, dt);
    }
    float pitch = fp_pitch_ + (body_pitch_ + pitch_osc_) * 0.3f;
    rot = glm::angleAxis(yaw_ + fp_yaw_, kUp) * glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(body_roll_ * 0.35f, glm::vec3(0.0f, 0.0f, 1.0f));
    float sp = glm::clamp(std::abs(speed_) / GALLOP_SPEED, 0.0f, 1.0f);
    fov = 88.0f + 10.0f * sp * sp;
}

void Horse::KeepCameraClear(glm::vec3& eye) const {
    // slide the camera around cacti and towers instead of clipping through them
    for (const Obstacle& o : obstacles_) {
        float min_dist = o.radius + 6.0f;
        float dx = eye.x - o.x;
        float dz = eye.z - o.z;
        if (std::abs(dx) > min_dist || std::abs(dz) > min_dist || eye.y > o.top + 4.0f) continue;
        float dist = std::sqrt(dx * dx + dz * dz);
        if (dist < min_dist && dist > 1e-4f) {
            eye.x = o.x + dx / dist * min_dist;
            eye.z = o.z + dz / dist * min_dist;
        }
    }
}

void Horse::SetCinematic(int mode, float a, float b, float c) {
    cine_mode_ = mode;
    cine_a_ = a;
    cine_b_ = b;
    cine_c_ = c;
    cine_eye_valid_ = false;
    cine_time_ = 0.0f;
    if (mode == 1) {
        cine_angle_ = yaw_ + glm::half_pi<float>();
    } else if (mode == 3) {
        cine_eye_ = pos_ + Forward() * a + Right() * c;
        cine_eye_.x = glm::clamp(cine_eye_.x, -4700.0f, 4700.0f);
        cine_eye_.z = glm::clamp(cine_eye_.z, -4700.0f, 4700.0f);
        cine_eye_.y = Ground(cine_eye_.x, cine_eye_.z) + b;
    } else if (mode == 0) {
        if (ride_state_ == RideState::FREE && rider_) {
            camera_->Attach(&rider_->transform);
        }
        camera_->SetPerspective(fov_, 0.01f, 10000.0f, (float)camera_->GetWinWidth(), (float)camera_->GetWinHeight());
    }
}

void Horse::CinematicCamera(float dt) {
    if (camera_->IsAttached()) camera_->Detach();
    glm::vec3 f = Forward(), r = Right();
    glm::vec3 target = pos_ + glm::vec3(0.0f, 13.0f + body_height_, 0.0f);
    glm::vec3 eye;
    if (cine_mode_ == 1) {
        cine_angle_ += cine_c_ * dt;
        eye = pos_ + glm::vec3(std::sin(cine_angle_) * cine_a_, cine_b_, std::cos(cine_angle_) * cine_a_);
    } else if (cine_mode_ == 2 || cine_mode_ == 4) {
        // 4 is the same tracking slide on a crane: it starts high over the rider and lowers to the given height
        cine_time_ += dt;
        float k = cine_mode_ == 4 ? glm::smoothstep(0.0f, 1.0f, std::min(cine_time_ / 3.4f, 1.0f)) : 1.0f;
        glm::vec3 desired = pos_ + r * (cine_a_ * glm::mix(0.75f, 1.0f, k)) + kUp * glm::mix(cine_b_ + 42.0f, cine_b_, k) + f * cine_c_;
        if (!cine_eye_valid_) {
            cine_eye_ = desired;
            cine_eye_valid_ = true;
        }
        for (int i = 0; i < 3; i++) cine_eye_[i] = Damp(cine_eye_[i], desired[i], 5.0f, dt);
        eye = cine_eye_;
        // look down onto the rider while high, settle on the whole horse as the crane lowers
        target = glm::mix(pos_ + glm::vec3(0.0f, 20.0f, 0.0f), target + f * 6.0f, k);
    } else {
        eye = cine_eye_;
    }
    KeepCameraClear(eye);
    eye.y = std::max(eye.y, Ground(eye.x, eye.z) + 2.0f);
    glm::quat rot = glm::normalize(glm::quat_cast(glm::inverse(glm::lookAt(eye, target, kUp))));
    float shake = trauma_ * trauma_ * 0.5f;
    if (shake > 0.0f) {
        float t = time_ * 23.0f;
        rot = rot * glm::quat(glm::vec3(std::sin(t * 1.3f), std::sin(t * 1.7f + 2.0f), 0.0f) * 0.02f * shake);
    }
    last_eye_ = eye;
    last_rot_ = rot;
    camera_->transform.SetPosition(eye);
    camera_->transform.SetOrientation(rot);
    camera_->SetPerspective(58.0f, 0.01f, 10000.0f, (float)camera_->GetWinWidth(), (float)camera_->GetWinHeight());
}

void Horse::UpdateCamera(float dt) {
    cam_idle_ += dt;
    if (cine_mode_ != 0) {
        CinematicCamera(dt);
        return;
    }
    if (ride_state_ == RideState::FREE || !rider_) return;
    if (camera_->IsAttached()) camera_->Detach();

    glm::vec3 eye;
    glm::quat rot;
    float fov;
    if (camera_mode_ == CameraMode::THIRD_PERSON) {
        ThirdPersonPose(dt, eye, rot, fov);
    } else {
        FirstPersonPose(dt, eye, rot, fov);
    }

    if (ride_state_ == RideState::MOUNTING) {
        float k = glm::smoothstep(0.0f, 1.0f, transition_);
        eye = glm::mix(from_eye_, eye, k);
        rot = glm::slerp(from_rot_, rot, k);
        fov = glm::mix(from_fov_, fov, k);
    } else if (ride_state_ == RideState::DISMOUNTING) {
        float k = glm::smoothstep(0.0f, 1.0f, transition_);
        glm::vec3 s = rider_->transform.GetScale();
        glm::quat foot_rot = glm::angleAxis(dismount_yaw_, kUp);
        glm::vec3 foot_eye = dismount_spot_ + glm::vec3(0.0f, rider_->GetHeight(), 0.0f) + foot_rot * (glm::vec3(0.0f, 3.0f, -0.4f) * s);
        eye = glm::mix(from_eye_, foot_eye, k);
        rot = glm::slerp(from_rot_, foot_rot, k);
        fov = glm::mix(from_fov_, 90.0f, k);
    } else {
        fov_ = Damp(fov_, fov, 3.0f, dt);
        fov = fov_;
    }
    last_eye_ = eye;
    last_rot_ = rot;
    if (ride_state_ != RideState::DISMOUNTING) fov_ = fov;

    // screen shake from impacts and hard galloping
    float shake = trauma_ * trauma_;
    if (shake > 0.0f) {
        float t = time_ * 23.0f;
        glm::vec3 jitter(std::sin(t * 1.3f) + std::sin(t * 2.9f + 1.0f), std::sin(t * 1.7f + 2.0f) + std::sin(t * 3.3f), std::sin(t * 2.1f + 4.0f));
        rot = rot * glm::quat(glm::vec3(jitter.x * 0.02f, jitter.y * 0.02f, jitter.z * 0.015f) * shake);
        eye += glm::vec3(jitter.y, jitter.x, jitter.z) * 0.25f * shake;
    }
    trauma_ = std::min(trauma_, 1.0f);

    camera_->transform.SetPosition(eye);
    camera_->transform.SetOrientation(rot);
    camera_->SetPerspective(fov, 0.01f, 10000.0f, (float)camera_->GetWinWidth(), (float)camera_->GetWinHeight());
}

// ------------------------------------------------------------------------------------------
// Audio
// ------------------------------------------------------------------------------------------

void Horse::UpdateAudio(float dt) {
    HorseAudio* audio = game_->GetHorseAudio();
    if (!audio) return;

    glm::mat4 inv = glm::inverse(camera_->GetViewMatrix());
    if (ride_state_ != RideState::FREE) {
        inv = glm::translate(glm::mat4(1.0f), last_eye_) * glm::mat4_cast(last_rot_);
    }
    audio->SetListener(glm::vec3(inv[3]), -glm::vec3(inv[2]), glm::vec3(inv[1]));

    // wind rushing past at speed
    float wind = ride_state_ == RideState::MOUNTED ? glm::smoothstep(90.0f, GALLOP_SPEED, std::abs(speed_)) * 0.35f : 0.0f;
    if (wind_loop_ && wind_loop_->isFinished()) HorseAudio::StopLoop(wind_loop_);
    if (wind > 0.01f && !wind_loop_) wind_loop_ = audio->StartLoop(HorseAudio::WIND, 0.0f);
    if (wind_loop_) {
        float v = Damp(wind_loop_->getVolume(), wind, 2.0f, dt);
        wind_loop_->setVolume(v);
        if (wind < 0.01f && v < 0.01f) HorseAudio::StopLoop(wind_loop_);
    }

    glm::vec3 nose = pos_ + glm::vec3(0.0f, 18.0f, 0.0f) + Forward() * 16.0f;
    // heavy blowing after a hard ride
    breath_timer_ -= dt;
    if (breath_timer_ < 0.0f) {
        breath_timer_ = 1.7f - exertion_ * 0.9f;
        if (exertion_ > 0.25f && std::abs(speed_) < CANTER_SPEED) {
            audio->Play(HorseAudio::BREATH, nose, exertion_ * 0.8f, 0.95f + 0.1f * Rand01());
        }
    }
    // the odd idle snort
    if (ride_state_ == RideState::FREE && std::abs(speed_) < 5.0f) {
        idle_snort_timer_ -= dt;
        if (idle_snort_timer_ < 0.0f) {
            idle_snort_timer_ = 8.0f + 10.0f * Rand01();
            audio->Play(HorseAudio::SNORT, nose, 0.5f, 0.9f + 0.2f * Rand01());
            head_toss_ = std::max(head_toss_, 0.15f);
        }
    }
    if (call_whinny_timer_ >= 0.0f) {
        call_whinny_timer_ -= dt;
        if (call_whinny_timer_ < 0.0f) {
            audio->Play(HorseAudio::WHINNY, nose, 0.75f, 0.95f + 0.1f * Rand01());
            head_toss_ = 0.25f;
        }
    }
}
