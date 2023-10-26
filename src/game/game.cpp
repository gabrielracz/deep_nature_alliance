#include "scene_graph.h"
#include "application.h"
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

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "[] Asteroid Field";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 60.0; // Field-of-view of camera (degrees)
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 player_position_g(0.0, 0.0, 800.0);
glm::vec3 camera_position_g(0.0, 0.0, 10.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

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

// Game::Game(void){
//     // Don't do work in the constructor, leave it for the Init() function
// }


void Game::Init(void){
    
    std::cout << "RNG seed: " << rng_seed << std::endl;
    app.SetMouseHandler(std::bind(&Game::MouseControls, this, std::placeholders::_1));
    // Run all initialization steps
    // InitWindow();
    // InitView();
    // InitEventHandlers();
    // InitControls();

    // Set variables
    // animating_ = true;
}

       
void Game::SetupResources(void){

    // Create a simple object to represent the asteroids
    resman.CreateSphere("SimpleObject", 0.8, 5, 5);
    resman.CreateTorus("Beacon", beacon_radius_g, beacon_radius_g - beacon_hitbox_g, 20, 20);
    resman.CreateTorus("Player", player_hitbox_g, 0.1, 15, 15);
    resman.CreateSphere("Enemy", enemy_hitbox_g, 5, 5);
    resman.CreateCylinder("Powerup", powerup_hitbox_g, powerup_hitbox_g, 10);
    resman.CreateCone("Branch", 1.0, 1.0, 2, 10);
    resman.CreateSphere("Leaf", 1.0, 4, 10);
    // resman.CreateCylinder("Branch", 1.0, 1.0, 2, 10);
    // resman.CreateSphere("Branch", 1, 10, 10);



    // Load material to be applied to asteroids
    std::string filename = std::string(SHADER_DIRECTORY) + std::string("/material");
    resman.LoadResource(Material, "ObjectMaterial", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene = SceneGraph();
    scene.SetBackgroundColor(viewport_background_color_g);

    // Create asteroid field
    CreatePlayer();
    CreateTree();
    CreateAsteroidField(500);

    // CreateRaceTrack();
    // CreateEnemies();
    // CreatePowerups();
}

void Game::Update(double dt, KeyMap &keys, Mouse &mouse) {
    CheckControls(keys);

    scene.Update(dt);

    // CheckCollisions();

    // camera_.Update();
}

void Game::CheckCollisions() {
    // check beacons
    float beacon_distance = glm::length(player->transform.position - beacons[active_beacon_index]->transform.position);
    if(beacon_distance < player_hitbox_g + beacon_hitbox_g) {
        //collision
        beacons[active_beacon_index]->inverted = 0;
        active_beacon_index++;
        if(active_beacon_index == beacons.size()) {
            // game_state = WIN;
            scene.SetBackgroundColor(glm::vec3(0.0f, 1.0f, 0.0f));
            std::cout << "WINNER!" << std::endl;
        }
        beacons[active_beacon_index]->inverted = 1;
    }

    // check enemies
    for(auto e : enemies) {
        float dist = glm::length(player->transform.position - e->transform.position);
        if(dist < player_hitbox_g + enemy_hitbox_g && e->active) {
            player->lives -= 1;
            player->move_speed -= speed_upgrade_g;
            e->active = false;
            if(player->lives < 1) {
                scene.SetBackgroundColor(glm::vec3(1.0f, 0.0f, 0.0f));
                std::cout << "LOSER!" << std::endl;
            }
        }
    }

    for(auto p : powerups) {
        float dist = glm::length(player->transform.position - p->transform.position);
        if(dist < player_hitbox_g + powerup_hitbox_g && p->active) {
            player->lives += 1;
            player->move_speed += speed_upgrade_g;
            p->active = false;
            p->inverted = false;
        }
    }   

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



    toggle(player->angular_velocity.x, look_sens, GLFW_KEY_DOWN, GLFW_KEY_UP);
    toggle(player->angular_velocity.y, look_sens, GLFW_KEY_LEFT, GLFW_KEY_RIGHT);
    toggle(player->angular_velocity.z, look_sens, GLFW_KEY_Q, GLFW_KEY_E);

    toggle(player->velocity.x, player->move_speed, GLFW_KEY_D, GLFW_KEY_A);
    toggle(player->velocity.y, player->move_speed, GLFW_KEY_Z, GLFW_KEY_X);
    toggle(player->velocity.z, player->move_speed, GLFW_KEY_S, GLFW_KEY_W);
    // if(keys[GLFW_KEY_W]) {
    //     player->Thrust(1);
    // } else if(keys[GLFW_KEY_S]) {
    //     player->Thrust(-1);
    // }
}

void Game::MouseControls(Mouse& mouse) {
    float mouse_sens = -0.001f;
	glm::vec2 look = mouse.move * mouse_sens;

    player->transform.Yaw(look.x);
    player->transform.Pitch(look.y);
}


Game::~Game(){
    
    glfwTerminate();
}


Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name){

    // Get resources
    Resource *geom = resman.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    scene.AddNode(ast);
    return ast;
}


void Game::CreatePlayer() {
    Resource* geom = resman.GetResource("Player");
    Resource* mat = resman.GetResource("ObjectMaterial");
    player = new Player("PlayerObj", geom, mat);
    player->transform.position = player_position_g;
    // player->visible = false;
    app.GetCamera().Attach(&player->transform); // Attach the camera to the player
    scene.AddNode(player);
}

int tcount = 0;
void Game::GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width) {
    Resource* geom = resman.GetResource("Leaf");
    Resource* mat = resman.GetResource("ObjectMaterial");
    for(int j = 0; j < leaves; j++) {
        // position
        float woff = rng.randfloat(0.0f, 2*PI);
        float wspd = 2.5f;
        float wstr = rng.randfloat(0.006, 0.015);
        Tree* leaf = new Tree("Leaf", geom, mat, woff, wspd, wstr, this);

        float p = rng.randfloat(0.0f, parent_length/1.25f);
        float x = rng.randfloat(0.0f, 1.0f);
        float z = rng.randfloat(0.0f, 1.0f);
        float l = rng.randfloat(0.5,1.0);
        float w = rng.randfloat(0.05f, 0.1);

        float r = rng.randfloat(PI/6, PI/3);


        leaf->transform.scale = {w, l, w};
        leaf->transform.position = {0.0f, p, 0.0f};
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
 
    Resource* geom = resman.GetResource("Branch");
    Resource* mat = resman.GetResource("ObjectMaterial");
    level++;
    for(int j = 0; j < branches; j++) {
        // position
        float woff = 0; //rng.randfloat(0.0f, 2*PI);
        // float ws = rng.randfloat(1.0f, 2.0f);
        float wstr = rng.randfloat(0.0004, 0.001);
        float wspd = rng.randfloat(1.0, 2.0);

        Tree* branch = new Tree("Branch", geom, mat, woff, wspd, wstr, this);

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
        tcount++;
        GrowTree(branch, branches, l, w, level, max_iterations);
    }
}

void Game::CreateTree() {
    Resource* bgeom = resman.GetResource("Branch");
    Resource* bmat = resman.GetResource("ObjectMaterial");
    Resource* lgeom = resman.GetResource("Leaf");
    Resource* lmat = resman.GetResource("ObjectMaterial");
    Tree* tree = new Tree("Tree", bgeom, bmat, 0, 0, 0, this);


    int branches = 3;
    int iterations = 4;
    float height = rng.randfloat(10.0, 20.0);
    float width = 0.25f;
    SceneNode* root = tree;

    GrowTree(tree, branches, height, width, 0, iterations);
    std::cout << tcount << std::endl;
    // Tree* tree = new Tree("Tree", bgeom, bmat, lgeom, lmat, branches, height, width, 0, iterations, this);
    tree->transform.position = player_position_g - glm::vec3(0, 0, 20);
    tree->transform.scale = {width, height, width};
    scene.AddNode(tree);
}

void Game::CreateRaceTrack() {
    glm::quat beacon_orientations[] = {
        {1      , 0     , 0     , 0},
        {0.921649       , 0.229045      , 0.3132        , -0.00270161},
        {0.964975       , 0.227993      , 0.10694       , -0.0735319},
        {0.990901       , 0.0835413     , -0.0349064    , -0.0995837},
        {0.994095       , -0.033332     , -0.0190866    , -0.101492},
        {0.831337       , 0.294662      , -0.470931     , -0.016675},
        {0.281068       , 0.405307      , -0.863279     , -0.10713 },
        {0.0346075      , 0.398591      , -0.910368     , -0.10563 },
        {-0.194923      , 0.366005      , -0.892426     , -0.177824},
        {-0.320758      , 0.455473      , -0.590345     , -0.584082},
        {-0.442946      , 0.264096      , -0.297878     , -0.803319},
        {-0.513011      , 0.030545      , -0.23939      , -0.823759},
        {-0.567791      , -0.320068     , -0.0383407    , -0.75743 },
        {-0.517779      , -0.576523     , 0.100737      , -0.624001},
    };




    Resource* geom = resman.GetResource("Beacon");
    Resource* mat = resman.GetResource("ObjectMaterial");
    for(int i = 0; i < num_beacons_g; i++) {
        glm::vec3& pos = beacon_positions_g[i];
        glm::quat& ori = beacon_orientations[i];
        SceneNode* b = new SceneNode("Beacon" + std::to_string(i), geom, mat);
        b->transform.position = pos;
        b->transform.orientation = ori;
        // b->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        scene.AddNode(b);
        beacons.push_back(b);
    }

    beacons.front()->inverted = 1;
}

void Game::CreateEnemies() {
    glm::vec3 enemy_positions[] = {
        {108            , 0     , 689                    },
        {34.1823        , 64.8259       , 525.166},
        {13.7887        , 117.501       , 307.928},
        {151.521        , -16.012       , 259.632},
        {19.4067        , -17.1548      , 638.167},
        {73.5702        , 85.7602       , 379.409},
    };

    Resource* geom = resman.GetResource("Enemy");
    Resource* mat = resman.GetResource("ObjectMaterial");
    int cnt = 0;
    for(auto p : enemy_positions) {
        Enemy* e = new Enemy("Enemy" + std::to_string(cnt++), geom, mat);
        e->transform.position = p;
        e->target = &player->transform;
        scene.AddNode(e);
        enemies.push_back(e);
    }

}

void Game::CreatePowerups() {
    Resource* geom = resman.GetResource("Powerup");
    Resource* mat = resman.GetResource("ObjectMaterial");
    std::vector<glm::vec3> powerup_positions(beacon_positions_g, std::end(beacon_positions_g));
    powerup_positions.push_back({-39.1208       , 77.1831       , 524.026});
    powerup_positions.push_back({6.96003        , 85.8356       , 427.861});
    powerup_positions.push_back({101.604        , 8.05086       , 365.088});

    for(auto bp : powerup_positions) {
        SceneNode* p = new SceneNode("Powerup", geom, mat);
        p->transform.position = bp;
        scene.AddNode(p);
        powerups.push_back(p);
        p->inverted = true;
    }
}

void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleObject", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}

