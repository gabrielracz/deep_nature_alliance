#include "scene_graph.h"
#include "application.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#define GLM_FORCE_RADIANS
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <time.h>
#include <sstream>
#include <functional>

#include "asteroid.h"
#include "game.h"
#include "enemy.h"
#include "path_config.h"
#include "resource.h"
#include "scene_node.h"
#include "tree.h"
#include "text.h"

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "[] Asteroid Field";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 player_position_g(0.0, 0.0, 4.0);

const float beacon_radius_g = 20.0f;
const float beacon_hitbox_g = 15.0f;
const float player_hitbox_g = 0.5f;
const float enemy_hitbox_g = 0.5f;
const float powerup_hitbox_g = 2.0f;
const float speed_upgrade_g = 0.5f;

// Materials 
const std::string material_directory_g = SHADER_DIRECTORY;

glm::vec3 beacon_positions_g[] = {
    {0      , 0     , 731},                   
    {-36.2169       , 26.1707       , 665.71},
    {-71.7758       , 77.3913       , 586.664},     
    {-81.4512       , 127.351       , 466.188},     
    {-74.1859       , 121.125       , 271.9}  ,     
    {-6.60791       , 143.437       , 186.731},     
    {91.6369        , 182.934       , 226.512},     
    {127.061        , 171.769       , 338.286},     
    {129.616        , 149.748       , 421.257},     
    {82.3724        , 17.3056       , 480.708},     
    {96.9571        , -86.6217      , 441.634},     
    {92.5081        , -146.151      , 349.63} ,     
    {30.3872        , -143.304      , 222.383},     
    {-33.0518       , -71.9166      , 171.495},
};

int num_beacons_g = sizeof(beacon_positions_g) / sizeof(beacon_positions_g[0]);

void Game::Init(void){
    SetupResources();
    SetupScene();

    //callback for responsive mouse controls
    app.SetMouseHandler(std::bind(&Game::MouseControls, this, std::placeholders::_1));
}

       
void Game::SetupResources(void){

    // load .obj meshes
    resman.LoadMesh("Player", RESOURCES_DIRECTORY"/h2.obj");

    // generate geometry
    resman.CreateQuad("TextQuad");
    resman.CreateSphere("SimpleObject", 0.8, 5, 5);
    resman.CreateTorus("Beacon", beacon_radius_g, beacon_radius_g - beacon_hitbox_g, 20, 20);
    resman.CreateSphere("Enemy", enemy_hitbox_g, 5, 5);
    resman.CreateCylinder("Powerup", powerup_hitbox_g, powerup_hitbox_g, 10);
    resman.CreateCone("Branch", 1.0, 1.0, 2, 10);
    resman.CreateSphere("Leaf", 1.0, 4, 10);
    resman.CreatePointCloud("PointCloud", 200000, 600);

    // load shader programs
    resman.LoadShader("ObjectMaterial", SHADER_DIRECTORY"/material_vp.glsl", SHADER_DIRECTORY"/material_fp.glsl");
    resman.LoadShader("ShipShader", SHADER_DIRECTORY"/ship_vp.glsl", SHADER_DIRECTORY"/lit_fp.glsl");
    resman.LoadShader("TextShader", SHADER_DIRECTORY"/text_vp.glsl", SHADER_DIRECTORY"/text_fp.glsl");

    // load textures
    resman.LoadTexture("Charmap", RESOURCES_DIRECTORY"/fixedsys_alpha.png", GL_CLAMP_TO_EDGE);
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene = SceneGraph();
    scene.SetBackgroundColor(viewport_background_color_g);

    // Create asteroid field
    CreatePlayer();
    CreateTree();
    CreateAsteroidField(1500);
    CreateLights();
    CreateHUD();
}

void Game::Update(double dt, KeyMap &keys) {
    CheckControls(keys);
    scene.Update(dt);
    // CheckCollisions();
}

void Game::CheckCollisions() {
}

void Game::CheckControls(KeyMap& keys) {
    // Get user data with a pointer to the game class
    // Quit game if 'q' is pressed
    if (keys[GLFW_KEY_ESCAPE]){
        app.Quit();
        return;
    }

    // Stop animation if space bar is pressed
    if (keys[GLFW_KEY_SPACE]){
        app.Pause();
        keys[GLFW_KEY_SPACE] = false;
    }

    // Debug print the player's location
    if(keys[GLFW_KEY_P]) {
        glm::vec3& p = player->transform.position;
        glm::quat& o = player->transform.orientation;
        std::cout << "Player Trace:\t{" << p.x << "\t, " << p.y << "\t, " << p.z << "}" 
        << "\t{" << o.w << "\t, " << o.x << "\t, " << o.y << "\t, " << o.z << "}" << std::endl;
        keys[GLFW_KEY_P] = false;
    }

    if(keys[GLFW_KEY_T]) {
        SetupScene();
        keys[GLFW_KEY_T] = false;
    }

    // View control
    float look_sens = 0.035; // amount the ship turns per keypress

    auto toggle = [&keys](float& target, float val, int ctrl1, int ctrl2) {
        if(keys[ctrl1]) {
            target = val;
        } else if(keys[ctrl2]) {
            target = -val;
        } else {
            target = 0.0f;
        }
    };

    auto toggle_func = [&keys](float& target, std::function<void(int)> f, int ctrl1, int ctrl2) {
        if(keys[ctrl1]) {
            f(1);
        } else if(keys[ctrl2]) {
            f(-1);
        }
    };



    // toggle(player->angular_velocity.x, look_sens, GLFW_KEY_DOWN, GLFW_KEY_UP);
    // toggle(player->angular_velocity.y, look_sens, GLFW_KEY_LEFT, GLFW_KEY_RIGHT);
    // toggle(player->angular_velocity.z, look_sens, GLFW_KEY_Q, GLFW_KEY_E);

    // toggle(player->velocity.x, player->move_speed, GLFW_KEY_D, GLFW_KEY_A);
    // toggle(player->velocity.y, player->move_speed, GLFW_KEY_Z, GLFW_KEY_X);
    // toggle(player->velocity.z, player->move_speed, GLFW_KEY_S, GLFW_KEY_W);

    if(keys[GLFW_KEY_LEFT_SHIFT]) {
        player->ShipControl(Player::Controls::THRUST);
    };
    if(keys[GLFW_KEY_LEFT_CONTROL]) {
        player->ShipControl(Player::Controls::BRAKE);
    };
    if(keys[GLFW_KEY_S]) {
        player->ShipControl(Player::Controls::PITCHU);
    };
    if(keys[GLFW_KEY_W]) {
        player->ShipControl(Player::Controls::PITCHD);
    };
    if(keys[GLFW_KEY_Q]) {
        player->ShipControl(Player::Controls::YAWL);
    };
    if(keys[GLFW_KEY_E]) {
        player->ShipControl(Player::Controls::YAWR);
    };
    if(keys[GLFW_KEY_A]) {
        player->ShipControl(Player::Controls::ROLLL);
    };
    if(keys[GLFW_KEY_D]) {
        player->ShipControl(Player::Controls::ROLLR);
    };


    if(keys[GLFW_KEY_I]) {
        app.GetCamera().transform.position.z -= 0.1;
    }
    if(keys[GLFW_KEY_J]) {
        app.GetCamera().transform.position.z += 0.1;
    }

    if(keys[GLFW_KEY_X]) {
        if(app.GetCamera().IsAttached()) {
            app.GetCamera().Drop();
        } else {
            app.GetCamera().Attach(&player->transform);
        }
        keys[GLFW_KEY_X] = false;
    }
    if(keys[GLFW_KEY_X]) {
        if(app.GetCamera().IsAttached()) {
            app.GetCamera().Drop();
        } else {
            app.GetCamera().Attach(&player->transform);
        }
        keys[GLFW_KEY_X] = false;
    }
    if(keys[GLFW_KEY_Z]) {
        if(app.GetCamera().IsAttached()) {
            if(camera_mode++ % 2 == 0) {
                app.GetCamera().MoveTo({0.0, 3.0f, -20.0f});
            } else {
                app.GetCamera().Reset();
            }
        }
        keys[GLFW_KEY_Z] = false;
    }

    // if(keys[GLFW_KEY_W]) {
    //     player->Thrust(1);
    // } else if(keys[GLFW_KEY_S]) {
    //     player->Thrust(-1);
    // }
}

void Game::MouseControls(Mouse& mouse) {
    // float mouse_sens = -0.001f;
    float mouse_sens = -0.1f;
	glm::vec2 look = mouse.move * mouse_sens;

    // player->transform.Yaw(look.x);
    player->ShipControl(Player::Controls::YAWL, look.x);
    player->ShipControl(Player::Controls::PITCHU, look.y);
    
}

void Game::CreatePlayer() {
    Mesh* mesh = resman.GetMesh("Player");
    Shader* shd = resman.GetShader("ShipShader");
    player = new Player("PlayerObj", mesh, shd);
    player->transform.position = player_position_g;
    // player->visible = false;
    app.GetCamera().Attach(&player->transform); // Attach the camera to the player
    scene.AddNode(player);
}

void Game::CreateHUD() {
    Mesh* mtxt = resman.GetMesh("TextQuad");
    Shader* stxt = resman.GetShader("TextShader");
    Texture* ttxt = resman.GetTexture("Charmap");
    Text* txt = new Text("hello", mtxt, stxt, this, "This is a game about flying through space");
    txt->transform.SetPosition({0.0f, 1.0f, 0.0f});
    txt->SetTexture(ttxt);
    txt->SetAnchor(Text::Anchor::TOPCENTER);
    txt->SetSize(15);
    scene.AddNode(txt);

    Text* fps = new Text("fps", mtxt, stxt, this, "FPS");
    fps->transform.SetPosition({-1.0, 1.0, 0.0f});
    fps->SetColor({0.0f, 1.0f, 0.0f, 1.0f});
    fps->SetAnchor(Text::Anchor::TOPLEFT);
    fps->SetCallback([this]() -> std::string {
        return "fps: " + std::to_string(app.GetFPS());
    });
    scene.AddNode(fps);
}

void Game::CreateLights() {
    Light* light = new Light({1.0f, 1.0f, 1.0f, 1.0f});
    light->transform.SetPosition({0.0, 100, 0});
    lights.push_back(light);
}

int tcount = 0;
void Game::GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width) {
    Mesh* mesh = resman.GetMesh("Leaf");
    Shader* shd = resman.GetShader("ObjectMaterial");
    for(int j = 0; j < leaves; j++) {
        // position
        float woff = rng.randfloat(0.0f, 2*PI);
        float wspd = 2.5f;
        float wstr = rng.randfloat(0.006, 0.015);
        Tree* leaf = new Tree("Leaf", mesh, shd, woff, wspd, wstr, this);

        float p = rng.randfloat(0.0f, parent_length/1.25f);
        float x = rng.randfloat(0.0f, 1.0f);
        float z = rng.randfloat(0.0f, 1.0f);
        float l = rng.randfloat(0.5,1.0);
        float w = rng.randfloat(0.05f, 0.1);

        float r = rng.randfloat(PI/6, PI/3);


        leaf->transform.scale = {w, l, w};
        leaf->transform.SetPosition({0.0f, p, 0.0f});
        // leaf->transform.position = {x, p, z};
        
        leaf->transform.orbit = glm::angleAxis(rng.randfloat(0, 2*PI), glm::vec3(0, 1, 0)); // yaw
        leaf->transform.orbit *= glm::angleAxis(rng.randsign() * r, glm::vec3(0, 0, 1)); // roll
        leaf->transform.joint = glm::vec3(0, -l/2, 0); // base of the leaf
 
        root->children.push_back(leaf);
        tcount++;
    }
}

void Game::GrowTree(SceneNode* root, int branches, float parent_height, float parent_width, int level, int max_iterations) {
    if(level >= max_iterations) {
        GrowLeaves(root, branches*branches, parent_height, parent_width);
        return;
    }
 
    Mesh* mesh = resman.GetMesh("Branch");
    Shader* shd = resman.GetShader("ObjectMaterial");
    level++;
    for(int j = 0; j < branches; j++) {
        // position
        float woff = 0; //rng.randfloat(0.0f, 2*PI);
        // float ws = rng.randfloat(1.0f, 2.0f);
        float wstr = rng.randfloat(0.0004, 0.001);
        float wspd = rng.randfloat(1.0, 2.0);

        Tree* branch = new Tree("Branch", mesh, shd, woff, wspd, wstr, this);

        float p = rng.randfloat(0.0f, parent_height/2.0f);
        float l = rng.randfloat(5.0f, parent_height - 1);
        float w = rng.randfloat(0.1f, parent_width/2);

        float r = rng.randfloat(PI/6, PI/3);


        branch->transform.scale = {w, l, w};
        branch->transform.position = {0.0f, p, 0.0f};
        
        branch->transform.orbit = glm::angleAxis(rng.randfloat(0, 2*PI), glm::vec3(0, 1, 0)); // yaw
        branch->transform.orbit *= glm::angleAxis(rng.randsign() * r, glm::vec3(0, 0, 1)); // roll
        branch->transform.joint = glm::vec3(0, -l/2, 0); // base of the cone
 
        root->children.push_back(branch);
        GrowTree(branch, branches, l, w, level, max_iterations);
        tcount++;
    }
}

void Game::CreateTree() {
    Mesh* mesh = resman.GetMesh("Branch");
    Shader* shd = resman.GetShader("ObjectMaterial");


    int branches = 3;
    int iterations = 4;
    float height = rng.randfloat(10.0, 20.0);
    float width = 0.25f;

    float cnt = 0;
    float spread = 20;

    for(int i = 0; i < 2; i++) {

        Tree* tree = new Tree("Tree", mesh, shd, 0, 0, 0, this);
        SceneNode* root = tree;
        GrowTree(tree, branches, height, width, 0, iterations);
        tree->transform.position = player_position_g - glm::vec3(i*spread, 0, 40);
        tree->transform.scale = {width, height, width};
        scene.AddNode(tree);
        tcount++;
    }
    std::cout << "SceneNodes: " << tcount << std::endl;
}

void Game::CreateAsteroidField(int num_asteroids){
    Shader* shd = resman.GetShader("ObjectMaterial");
    Mesh* mesh = resman.GetMesh("PointCloud");
    scene.AddNode(new SceneNode("Stars", mesh, shd));
}

