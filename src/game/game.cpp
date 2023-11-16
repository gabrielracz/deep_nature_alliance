#include "defines.h"
#include "scene_graph.h"
#include "application.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <string>
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
#include "terrain.h"

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "[] Asteroid Field";

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
    app.ToggleMouseCapture();
    SetupScene();

    //callback for responsive mouse controls
    // app.SetMouseHandler(std::bind(&Game::MouseControls, this, std::placeholders::_1));
}

       
void Game::SetupResources(void){
    LoadMeshes();
    LoadShaders();
    LoadTextures();
}

void Game::LoadMeshes() {
    // load .obj meshes
    resman.LoadMesh        ("M_Ship", RESOURCES_DIRECTORY"/h2.obj");
    // resman.CreateTorus     ("M_Ship", 3.0, 1.0, 100, 100); 
    // generate geometry
    resman.CreateQuad      ("M_Quad");
    resman.CreateCone      ("M_Branch", 1.0, 1.0, 2, 10);
    resman.CreateSphere    ("M_Leaf", 1.0, 4, 10);
    resman.CreatePointCloud("M_StarCloud", 50000, 600, {0.8, 0.8, 0.8, 0.8});
    resman.CreateSphere    ("M_Planet", 1, 500, 500);
}

void Game::LoadShaders() {
    std::cout << "shaders loaded" << std::endl;
    // load shader programs
    resman.LoadShader("S_Default", SHADER_DIRECTORY"/material_vp.glsl", SHADER_DIRECTORY"/material_fp.glsl");
    // resman.LoadShader("S_Ship", SHADER_DIRECTORY"/ship_vp.glsl", SHADER_DIRECTORY"/lit_fp.glsl");
    resman.LoadShader("S_Lit", SHADER_DIRECTORY"/lit_vp.glsl", SHADER_DIRECTORY"/lit_fp.glsl");
    resman.LoadShader("S_Text", SHADER_DIRECTORY"/text_vp.glsl", SHADER_DIRECTORY"/text_fp.glsl");
    resman.LoadShader("S_Planet", SHADER_DIRECTORY"/ship_vp.glsl", SHADER_DIRECTORY"/textured_fp.glsl");
}

void Game::LoadTextures() {
    // load textures
    resman.LoadTexture("T_Charmap", RESOURCES_DIRECTORY"/fixedsys_alpha.png", GL_CLAMP_TO_EDGE);
    resman.LoadTexture("T_LavaPlanet", RESOURCES_DIRECTORY"/lava_planet.png", GL_REPEAT, 4.0f);
    resman.LoadTexture("T_Ship", RESOURCES_DIRECTORY"/h2tex.png", GL_REPEAT);
    // resman.LoadTexture("T_LavaPlanet", RESOURCES_DIRECTORY"/lava_planet.png", GL_REPEAT, 4.0f);
    // resman.LoadTexture("T_SnowPlanet", RESOURCES_DIRECTORY"/snow_planet.png", GL_REPEAT);
    // resman.LoadTexture("T_MarsPlanet", RESOURCES_DIRECTORY"/8k_mars.jpg", GL_REPEAT);
    // resman.LoadTexture("T_RockPlanet", RESOURCES_DIRECTORY"/mine_rocks.png", GL_REPEAT);
    // resman.LoadTexture("T_RedPlanet", RESOURCES_DIRECTORY"/red_rubble.png", GL_REPEAT);
    resman.LoadTexture("T_MoonPlanet", RESOURCES_DIRECTORY"/4k_ceres.jpg", GL_REPEAT);
    // resman.LoadTexture("T_KaliaPlanet", RESOURCES_DIRECTORY"/kalia.png", GL_REPEAT);
}


void Game::SetupScene(void){

    // Set background color for the scene
    scenes.push_back( new SceneGraph(app)); // FPS TEST SCENE
    scene = scenes[FPTEST];
    scene->SetBackgroundColor(viewport_background_color_g);


    //player created temporarily just so when controls query for player not null
    //once player refactor shoudl fix 
    //also for assignment can just remove control code from game temporarily
    CreatePlayer();

    SetupFPScene(); // FPS TEST SCENE


    // // CreateTerrain();
    // CreatePlanets();
    // CreateTriggers();
    // // CreateTree();
    // CreateAsteroidField(500);
    CreateLights();
    CreateHUD();
}

void Game::SetupFPScene(void) {

    FP_Player* p = new FP_Player("Obj_FP_Player", "M_Ship", "S_Lit", "T_Ship");
    p->transform.SetPosition(player_position_g);
    p->visible = false;
    scenes[FPTEST]->SetFPPlayer(p);
    AddToScene(FPTEST, p);

    SceneNode* n = new SceneNode("Shippy", "M_Ship", "S_Lit");
    n->transform.SetPosition(glm::vec3(0,0,-10));
    AddToScene(FPTEST, n);

    int terrain_size = 1000;
    Terrain* t = new Terrain("Obj_MoonTerrain", "M_MoonTerrain", "S_Lit", "T_MoonPlanet", terrain_size, terrain_size, 0.25, this);
    t->transform.Translate({-terrain_size / 2.0, -30.0, -terrain_size / 2.0});
    AddToScene(FPTEST, t);
    p->SetTerrain(t);

    p->Init(app.GetWindow().ptr, &app.GetCamera());
    app.SetMouseHandler(std::bind(&FP_Player::MouseControls, scene->GetFPPlayer(), std::placeholders::_1));
    app.SetFirstPersonView();
}

void Game::Update(double dt, KeyMap &keys) {
    CheckControls(keys);
    scene->Update(dt);
}


void Game::CheckControls(KeyMap& keys) {
    Player* player = scene->GetPlayer();
    // if (keys[GLFW_KEY_ESCAPE]){
    //     app.Quit();
    //     return;
    // }

    // Stop animation if space bar is pressed
    if (keys[GLFW_KEY_ESCAPE]){
        app.ToggleMouseCapture();
        keys[GLFW_KEY_ESCAPE] = false;
    }

    // Debug print the player's location
    if(keys[GLFW_KEY_P]) {
        glm::vec3 p = player->transform.GetPosition();
        glm::quat o = player->transform.GetOrientation();
        std::cout << glm::to_string(p) << " " << glm::to_string(o) << std::endl;
        keys[GLFW_KEY_P] = false;
    }

    if(keys[GLFW_KEY_T]) {
        SetupScene();
        keys[GLFW_KEY_T] = false;
    }

    if(keys[GLFW_KEY_RIGHT_BRACKET]) {
        app.ToggleRenderMode();
        keys[GLFW_KEY_RIGHT_BRACKET] = false;
    }

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

    if(keys[GLFW_KEY_UP]) {
        app.GetCamera().transform.Pitch(0.05f);
    }
    if(keys[GLFW_KEY_DOWN]) {
        app.GetCamera().transform.Pitch(-0.05f);
    }

    if(keys[GLFW_KEY_0]) {
        LoadShaders();
        keys[GLFW_KEY_0] = false;
    }


    if(keys[GLFW_KEY_I]) {
        app.GetCamera().transform.Translate({0.0, 0.0, -0.1});
    }
    if(keys[GLFW_KEY_J]) {
        app.GetCamera().transform.Translate({0.0, 0.0,0.1});
    }

    if(keys[GLFW_KEY_X]) {
        if(app.GetCamera().IsAttached()) {
            app.GetCamera().Drop();
        } else {
            app.GetCamera().Attach(&player->transform);
        }
        keys[GLFW_KEY_X] = false;
    }

    // if(keys[GLFW_KEY_2]) {
    //     SetActiveScene(FPTEST);
    //     //This some jank obv
    //     scene->GetFPPlayer()->Init(app.GetWindow().ptr, &app.GetCamera());
    //     app.SetMouseHandler(std::bind(&FP_Player::MouseControls, scene->GetFPPlayer(), std::placeholders::_1));
    //     app.SetFirstPersonView();
    //     keys[GLFW_KEY_2] = false;
    // }

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
}

void Game::MouseControls(Mouse& mouse) {
    // float mouse_sens = -0.001f;
    float mouse_sens = -0.1f;
	glm::vec2 look = mouse.move * mouse_sens;

    // player->transform.Yaw(look.x);
    Player* player = scene->GetPlayer();
    player->ShipControl(Player::Controls::YAWL, look.x);
    player->ShipControl(Player::Controls::PITCHU, look.y);
}

void Game::CreatePlayer() {
    Player* player = new Player("Obj_Player", "M_Ship", "S_Lit");
    player->transform.SetPosition(player_position_g);
    // player->visible = false;

    app.GetCamera().Attach(&player->transform); // Attach the camera to the player
    AddPlayerToScene(SceneEnum::ALL, player);
    // scenes[BEFORETRIGGER]->AddNode(player);
    // scenes[BEFORETRIGGER]->SetPlayer(player);

    // scenes[AFTERTRIGGER]->AddNode(player);
    // scenes[AFTERTRIGGER]->SetPlayer(player);
}

void Game::AddLightToScene(SceneEnum sceneNum, Light* l){
    if (sceneNum == SceneEnum::ALL){
        for (auto s : scenes){
            s->AddLight(l);
        }
    } else{
        scenes[sceneNum]->AddLight(l);
    }
}

void Game::AddPlayerToScene(SceneEnum sceneNum, Player* node) {
    if (sceneNum == SceneEnum::ALL){
        for (auto s : scenes){
            s->SetPlayer(node);
        }
    } else{
        scenes[sceneNum]->SetPlayer(node);
    }
    AddToScene(sceneNum, node);
}

void Game::AddToScene(SceneEnum sceneNum, SceneNode* node) {
    if (sceneNum == SceneEnum::ALL){
        for (auto s : scenes){
            s->AddNode(node);
            s->GetColman().AddNode(node);
        }
    } else{
        scenes[sceneNum]->AddNode(node);
        scenes[sceneNum]->GetColman().AddNode(node);
    }
}

// void Game::CreatePlanets() {
//     SceneNode* planet = new SceneNode("Obj_Sun", "M_Planet", "S_Planet", "T_LavaPlanet");
//     planet->transform.SetScale({800, 800, 800});
//     planet->transform.SetPosition({200, 0, -2000});
//     planet->transform.SetOrientation(glm::angleAxis(PI/1.5f, glm::vec3(1.0, 0.0, 0.0)));
//     AddToScene(SceneEnum::AFTERTRIGGER, planet);
// }

// void Game::CreateTerrain() {
//     // this generates its own terrain.
//     Terrain* t = new Terrain("Obj_MoonTerrain", "M_MoonTerrain", "S_Lit", "T_MoonPlanet", 1000, 1000, 0.25, this);
//     AddToScene(SceneEnum::AFTERTRIGGER, t);

//     // std::vector<float> verts = Terrain::GenerateHeightmap(100.0, 200.0, 1.0);


// }

void Game::CreateHUD() {

    Text* txt = new Text("Obj_Banner", "M_Quad", "S_Text", "T_Charmap", this, "DEEP NATURE ALLIANCE\nA millenia ago, thousands of soldiers\nwere sent deep into the far reaches of space\nin search of life.\nOne of these soldiers is you...");
    txt->transform.SetPosition({0.0f, 1.0f, 0.0f});
    txt->SetAnchor(Text::Anchor::TOPCENTER);
    txt->SetColor(Colors::SlimeGreen);
    txt->SetSize(15);
    // AddToScene(SceneEnum::AFTERTRIGGER, txt);

    Text* fps = new Text("Obj_Fps", "M_Quad", "S_Text", "T_Charmap", this, "FPS");
    fps->transform.SetPosition({-1.0, 1.0, 0.0f});
    fps->SetColor(Colors::Magenta);
    fps->SetAnchor(Text::Anchor::TOPLEFT);
    fps->SetCallback([this]() -> std::string {
        return "fps: " + std::to_string(app.GetFPS());
    });
    AddToScene(SceneEnum::ALL, fps);

    Text* speedo = new Text("Obj_Speedo", "M_Quad", "S_Text", "T_Charmap", this, "");
    speedo->transform.SetPosition({0.0, -0.1, 0.0f});
    speedo->SetColor(HEXCOLORALPH(0xFF00FF, 0.75));
    speedo->SetAnchor(Text::Anchor::CENTER);
    speedo->SetCallback([this]() -> std::string {
        return std::to_string((glm::length(scene->GetPlayer()->velocity)));
    });
    // scene->AddNode(speedo);
    AddToScene(SceneEnum::ALL, speedo);

    // Text* crosshair = new Text("Obj_Crosshair", "M_Quad", "S_Text", "T_Charmap", this, "[ ]");
    // crosshair->transform.SetPosition({0.0, 0.1, 0.0});
    // crosshair->SetSize(10.0f);
    // crosshair->SetColor(HEXCOLORALPH(0xFF00FF, 0.75));
    // crosshair->SetBackgroundColor(Colors::Transparent);
    // crosshair->SetAnchor(Text::Anchor::CENTER);
    // AddToScene(SceneEnum::AFTERTRIGGER, crosshair);
}

void Game::CreateLights() {
    Light* light = new Light({1.0f, 1.0f, 1.0f, 1.0f});
    light->transform.SetPosition({50.5, 100.5, 50.5});
    AddLightToScene(SceneEnum::ALL, light);
    // scenes[BEFORETRIGGER]->GetLights().push_back(light);
    // scenes[AFTERTRIGGER]->GetLights().push_back(light);
}

// int tcount = 0;
// void Game::GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width) {
//     Mesh* mesh = resman.GetMesh("Leaf");
//     Shader* shd = resman.GetShader("ObjectMaterial");
//     for(int j = 0; j < leaves; j++) {
//         // position
//         float woff = rng.randfloat(0.0f, 2*PI);
//         float wspd = 2.5f;
//         float wstr = rng.randfloat(0.006, 0.015);
//         Tree* leaf = new Tree("Leaf", "M_Branch", "S_Default", "", woff, wspd, wstr, this);

//         float p = rng.randfloat(0.0f, parent_length/1.25f);
//         float x = rng.randfloat(0.0f, 1.0f);
//         float z = rng.randfloat(0.0f, 1.0f);
//         float l = rng.randfloat(0.5,1.0);
//         float w = rng.randfloat(0.05f, 0.1);

//         float r = rng.randfloat(PI/6, PI/3);


//         leaf->transform.SetSca = {w, l, w};
//         leaf->transform.SetPosition({0.0f, p, 0.0f});
//         // leaf->transform.position = {x, p, z};
        
//         leaf->transform.orbit = glm::angleAxis(rng.randfloat(0, 2*PI), glm::vec3(0, 1, 0)); // yaw
//         leaf->transform.orbit *= glm::angleAxis(rng.randsign() * r, glm::vec3(0, 0, 1)); // roll
//         leaf->transform.joint = glm::vec3(0, -l/2, 0); // base of the leaf
 
//         root->AddChild(leaf);
//         tcount++;
//     }
// }

// void Game::GrowTree(SceneNode* root, int branches, float parent_height, float parent_width, int level, int max_iterations) {
//     if(level >= max_iterations) {
//         GrowLeaves(root, branches*branches, parent_height, parent_width);
//         return;
//     }
 
//     Mesh* mesh = resman.GetMesh("Branch");
//     Shader* shd = resman.GetShader("ObjectMaterial");
//     level++;
//     for(int j = 0; j < branches; j++) {
//         // position
//         float woff = 0; //rng.randfloat(0.0f, 2*PI);
//         // float ws = rng.randfloat(1.0f, 2.0f);
//         float wstr = rng.randfloat(0.0004, 0.001);
//         float wspd = rng.randfloat(1.0, 2.0);

//         Tree* branch = new Tree("Branch", "M_Branch", "S_Default", "", woff, wspd, wstr, this);

//         float p = rng.randfloat(0.0f, parent_height/2.0f);
//         float l = rng.randfloat(5.0f, parent_height - 1);
//         float w = rng.randfloat(0.1f, parent_width/2);

//         float r = rng.randfloat(PI/6, PI/3);


//         branch->transform.scale = {w, l, w};
//         branch->transform.position = {0.0f, p, 0.0f};
        
//         branch->transform.orbit = glm::angleAxis(rng.randfloat(0, 2*PI), glm::vec3(0, 1, 0)); // yaw
//         branch->transform.orbit *= glm::angleAxis(rng.randsign() * r, glm::vec3(0, 0, 1)); // roll
//         branch->transform.joint = glm::vec3(0, -l/2, 0); // base of the cone
 
//         root->AddChild(branch);
//         GrowTree(branch, branches, l, w, level, max_iterations);
//         tcount++;
//     }
// }

// void Game::CreateTree() {
//     Mesh* mesh = resman.GetMesh("Branch");
//     Shader* shd = resman.GetShader("ObjectMaterial");


//     int branches = 3;
//     int iterations = 4;
//     float height = rng.randfloat(10.0, 20.0);
//     float width = 0.25f;

//     float cnt = 0;
//     float spread = 20;

//     for(int i = 0; i < 2; i++) {

//         Tree* tree = new Tree("Obj_TreeTrunk", "M_Branch", "S_Default", "", 0, 0, 0, this);
//         SceneNode* root = tree;
//         GrowTree(tree, branches, height, width, 0, iterations);
//         tree->transform.position = player_position_g - glm::vec3(i*spread, 0, 40);
//         tree->transform.scale = {width, height, width};
//         scene->AddNode(tree);
//         tcount++;
//     }
//     std::cout << "SceneNodes: " << tcount << std::endl;
// }

// void Game::CreateAsteroidField(int num_asteroids){
//     scenes[AFTERTRIGGER]->AddNode(new SceneNode("Obj_Stars", "M_StarCloud", "S_Default"));
// }

void Game::ChangeScene(int sceneIndex) {
    std::cout << "changing scenes" << std::endl;
    scene = scenes[sceneIndex];
    return;
}

/*
Using std::bind to defer the call to Game::ChangeScene as far as I can tell
requires an actual defined member function which kinda defeats the main idea
I had originally when I thought of this (easy and clutter free change of game state) 
(see commented out func below)

Still has the advantage we can initiate change of gamestate from internal state of one of the scene nodes
however a similar thing could be done by just having a pointer to game in each scene node

The use of something like this could also be used to implement all kinds of things within scene node or other classes
idk lmk what u guys think
*/
// void Game::CreateTriggers(){
//     glm::vec3 trigger_positions[] = {
//         player_position_g - glm::vec3(0, 0, 40)
//     };

//     for (int i = 0; i < 1; i++){
//         std::function<void()> triggerAction = std::bind(&Game::ChangeScene, this, 1);

//         Trigger* t = new Trigger("Trigger" + std::to_string(i), "M_Leaf", "S_Default", "", triggerAction);
//         t->transform.SetPosition(trigger_positions[i]);
//         AddToScene(BEFORETRIGGER, t);
//         // scenes[BEFORETRIGGER]->AddNode(t);
//         // scenes[BEFORETRIGGER]->GetColman().AddTrigger(t);
//     }
// }


//other option (storing a pointer to main (kinda hacky))
// void Game::CreateTriggers(){
//     glm::vec3 trigger_positions[] = {
//         player_position_g - glm::vec3(0, 0, 40)
//     };

//     for (int i = 0; i < 1; i++){
//         std::function<void(Game&)> triggerAction = [](Game& game) {
//             game.scene = &game.scenes[1];
//         };
//         Trigger* t = new Trigger("Trigger" + std::to_string(i), "M_Leaf", "S_Default", "", game, triggerAction);
//         t->transform.position = trigger_positions[i];
//         scene->AddNode(t);
//         triggers.push_back(t);
//     }
// }

