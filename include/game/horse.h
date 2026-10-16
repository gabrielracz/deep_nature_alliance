#ifndef HORSE_H_
#define HORSE_H_

#include <memory>
#include <string>
#include <vector>
#include <irrKlang.h>

#include "scene_node.h"
#include "player.h"
#include "terrain.h"
#include "defines.h"

class Game;
class FP_Player;
class Camera;
class ResourceManager;

// A rideable horse. Procedurally animated (gait cycles, foot IK on the dunes, secondary motion),
// carries the first person player as a visible rider and owns the camera while ridden.
class Horse : public SceneNode {
public:
    Horse(Game* game, std::shared_ptr<Terrain> terrain, Camera* camera);
    ~Horse();

    static void LoadAssets(ResourceManager& resman);

    virtual void Update(double dt) override;

    void SetRider(FP_Player* rider) { rider_ = rider; }
    void PlaceAt(const glm::vec3& position, float yaw);
    // circular obstacles the horse can't pass through (unless it jumps over them)
    void AddObstacles(SceneNode& instanced, float radius, float height_per_scale);
    void AddObstacle(const glm::vec3& position, float radius, float height);

    // player actions
    void Interact();        // mount or dismount
    void Call();            // whistle, the horse comes to you
    void ToggleCamera();    // third / first person while riding
    bool IsMounted() const { return ride_state_ != RideState::FREE; }
    void OnSceneExit();     // silence looping audio when leaving the level

    // scripted showcase cameras: 0 off, 1 orbit (radius, height, rad/s),
    // 2 tracking (side, up, ahead offsets), 3 fixed lens the horse rides past (ahead, height, side),
    // 4 crane tracking: like 2, but starts high looking down on the rider and lowers over a few seconds
    void SetCinematic(int mode, float a, float b, float c);

    // forwarded from the rider while mounted
    void RiderControl(Player::Controls c, float dt);
    void RiderMouse(Mouse& mouse);

    std::string PromptText() const;
    std::string StatusText() const;

    // tuning
    static constexpr float WALK_SPEED   = 45.0f;
    static constexpr float TROT_SPEED   = 110.0f;
    static constexpr float CANTER_SPEED = 170.0f;
    static constexpr float GALLOP_SPEED = 240.0f;

private:
    enum class RideState { FREE, MOUNTING, MOUNTED, DISMOUNTING };
    enum class Behaviour { IDLE, GRAZE, WANDER, COMING };
    enum class CameraMode { THIRD_PERSON, FIRST_PERSON };

    struct Leg {
        SceneNode* upper = nullptr;
        SceneNode* lower = nullptr;
        SceneNode* pastern = nullptr;
        SceneNode* hoof = nullptr;
        glm::vec3 hip;          // joint position in body space
        float upper_len;
        float lower_len;
        bool front;
        float side;             // -1 left, +1 right
        float nominal_z;        // where the hoof rests under the body (horse space)
        bool in_stance = true;
        glm::vec3 sole_world = glm::vec3(0.0f);
    };

    struct RiderLimb {
        SceneNode* upper = nullptr;
        SceneNode* lower = nullptr;
        SceneNode* end = nullptr;
        glm::vec3 root;         // joint position in the parent's space
        float upper_len;
        float lower_len;
    };

    struct Obstacle {
        float x, z, radius, top;
    };

    // --- setup ---
    void BuildModel();
    SceneNode* Part(SceneNode* parent, const std::string& mesh, const std::string& texture, const glm::vec3& pos, int material);

    // --- simulation ---
    void UpdateRideState(float dt);
    void DriveFromInput(float dt);
    void UpdateBehaviour(float dt);
    void SteerTowards(const glm::vec3& target, float speed, float dt);
    float AvoidObstacles(float lookahead = 40.0f);
    void Integrate(float dt);
    void ResolveCollisions(float& impact);
    void OnImpact(float impact);
    void UpdateStamina(float dt);

    // --- animation ---
    void Animate(float dt);
    void AnimateLegs(float dt, const glm::quat& body_rot, const glm::vec3& body_pos);
    void AnimateNeckAndHead(float dt);
    void AnimateHair(float dt);
    void AnimateRider(float dt, const glm::quat& body_rot);
    void UpdateReins();
    void OnHoofStrike(Leg& leg);

    // --- camera, audio, fx ---
    void UpdateCamera(float dt);
    void ThirdPersonPose(float dt, glm::vec3& eye, glm::quat& rot, float& fov);
    void FirstPersonPose(float dt, glm::vec3& eye, glm::quat& rot, float& fov);
    void CinematicCamera(float dt);
    void KeepCameraClear(glm::vec3& eye) const;
    void UpdateAudio(float dt);
    void SpawnDust(const glm::vec3& at, float intensity, float scale);
    void ShowMessage(const std::string& msg, float seconds);

    void BeginMount();
    void BeginDismount();
    void FinishDismount();

    glm::vec3 Forward() const;
    glm::vec3 Right() const;
    glm::vec3 ToWorld(const glm::vec3& local) const;
    float Ground(float x, float z) const;
    float DistanceToRider() const;

    Game* game_;
    std::shared_ptr<Terrain> terrain_;
    Camera* camera_;
    FP_Player* rider_ = nullptr;
    std::vector<Obstacle> obstacles_;

    // body state
    glm::vec3 pos_ = glm::vec3(0.0f);
    float yaw_ = 0.0f;
    float speed_ = 0.0f;            // signed, along the heading
    float target_speed_ = 0.0f;
    float yaw_rate_ = 0.0f;
    float target_yaw_rate_ = 0.0f;
    float vertical_velocity_ = 0.0f;
    bool airborne_ = false;
    float air_time_ = 0.0f;
    float accel_ = 0.0f;
    float braking_ = 0.0f;
    float slope_factor_ = 1.0f;
    float stamina_ = 100.0f;
    float stamina_regen_delay_ = 0.0f;
    bool winded_ = false;
    float exertion_ = 0.0f;         // drives heavy breathing after hard riding
    float rear_t_ = -1.0f;          // >= 0 while rearing up
    float stagger_ = 0.0f;
    float time_ = 0.0f;

    // input for this frame
    bool in_forward_ = false, in_back_ = false, in_left_ = false, in_right_ = false;
    bool in_sprint_ = false, in_walk_ = false, in_jump_ = false;

    // gait / animation
    float phase_ = 0.0f;
    float duty_ = 0.6f;
    float gait_trot_ = 0.0f, gait_canter_ = 0.0f, gait_gallop_ = 0.0f;
    float body_height_ = 0.0f;      // smoothed terrain driven offsets
    float body_pitch_ = 0.0f;
    float body_roll_ = 0.0f;
    float bob_ = 0.0f;
    float bob_velocity_ = 0.0f;
    float pitch_osc_ = 0.0f;
    float land_squash_ = 0.0f;
    float land_squash_vel_ = 0.0f;
    float jump_pose_ = 0.0f;
    float breath_phase_ = 0.0f;
    float graze_ = 0.0f;
    float look_yaw_ = 0.0f;
    float neck_pitch_ = 0.7f;
    float ear_alert_ = 0.5f;
    float ear_flick_[2] = {0.0f, 0.0f};
    float tail_swish_ = 0.0f;
    float tail_swish_timer_ = 3.0f;
    float head_toss_ = 0.0f;
    float posting_ = 0.0f;
    float skid_dust_timer_ = 0.0f;
    float mane_blow_[10] = {};
    float mane_vel_[10] = {};
    float tail_lift_[6] = {};
    float tail_vel_[6] = {};
    float rider_lean_ = 0.1f;
    float steer_visual_ = 0.0f;

    // behaviour when not ridden
    Behaviour behaviour_ = Behaviour::IDLE;
    float behaviour_timer_ = 4.0f;
    glm::vec3 wander_target_ = glm::vec3(0.0f);
    bool met_rider_ = false;
    float call_whinny_timer_ = -1.0f;

    // riding
    RideState ride_state_ = RideState::FREE;
    float transition_ = 0.0f;
    bool rider_was_visible_ = true;
    glm::vec3 last_rider_sync_ = glm::vec3(0.0f);
    glm::vec3 dismount_spot_ = glm::vec3(0.0f);
    float dismount_yaw_ = 0.0f;
    bool ever_mounted_ = false;
    float hint_timer_ = 0.0f;
    std::string message_;
    float message_timer_ = 0.0f;

    // camera
    CameraMode camera_mode_ = CameraMode::THIRD_PERSON;
    float cam_yaw_ = 0.0f;
    float cam_pitch_ = 0.22f;
    float cam_idle_ = 10.0f;        // seconds since the mouse last moved
    glm::vec3 cam_focus_ = glm::vec3(0.0f);
    bool cam_focus_valid_ = false;
    float fov_ = 90.0f;
    float fp_yaw_ = 0.0f;
    float fp_pitch_ = -0.12f;
    float trauma_ = 0.0f;
    glm::vec3 from_eye_ = glm::vec3(0.0f);
    glm::quat from_rot_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float from_fov_ = 90.0f;
    int cine_mode_ = 0;
    float cine_a_ = 0.0f, cine_b_ = 0.0f, cine_c_ = 0.0f;
    float cine_angle_ = 0.0f;
    float cine_time_ = 0.0f;
    glm::vec3 cine_eye_ = glm::vec3(0.0f);
    bool cine_eye_valid_ = false;
    glm::vec3 last_eye_ = glm::vec3(0.0f);
    glm::quat last_rot_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // audio
    irrklang::ISound* wind_loop_ = nullptr;
    float breath_timer_ = 2.0f;
    float idle_snort_timer_ = 6.0f;

    // fx
    std::vector<std::weak_ptr<SceneNode>> dust_;

    // model
    SceneNode* body_pivot_ = nullptr;
    SceneNode* body_ = nullptr;
    SceneNode* neck_ = nullptr;
    SceneNode* head_ = nullptr;
    SceneNode* ears_[2] = {};
    SceneNode* forelock_ = nullptr;
    SceneNode* mane_[10] = {};
    glm::vec3 mane_root_[10];
    SceneNode* tail_[6] = {};
    float tail_len_[6] = {};
    Leg legs_[4];                   // left fore, right fore, left hind, right hind

    SceneNode* rider_root_ = nullptr;
    SceneNode* rider_torso_ = nullptr;
    SceneNode* rider_head_ = nullptr;
    RiderLimb rider_arms_[2];
    RiderLimb rider_legs_[2];
    SceneNode* reins_[2] = {};
};

#endif // HORSE_H_
