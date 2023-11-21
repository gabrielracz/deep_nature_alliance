#include "defines.h"
#include "fp_player.h"
#include "scene_graph.h"
#include "application.h"
#include "shader.h"
#include "tp_player.h"
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

void Game::Init(void){
    SetupResources();
    SetupScenes();
    app.SetResizeHandler(std::bind(&Game::ResizeCameras, this, std::placeholders::_1, std::placeholders::_2));
}

       
void Game::SetupResources(void){
    LoadMeshes();
    LoadShaders();
    LoadTextures();
}

void Game::LoadMeshes() {
    // load .obj meshes
    resman.LoadMesh        ("M_Ship", RESOURCES_DIRECTORY"/h2.obj");
    resman.LoadMesh        ("M_Tree4", RESOURCES_DIRECTORY"/tree4.obj");
    // resman.LoadMesh        ("M_Tree", RESOURCES_DIRECTORY"/oak.obj");
    resman.LoadMesh        ("M_BirchTree", RESOURCES_DIRECTORY"/birch_tree.obj");
    resman.LoadMesh        ("M_Tree", RESOURCES_DIRECTORY"/lowpolytree.obj");

    // generate geometry
    resman.CreateQuad      ("M_Quad");
    resman.CreateCone      ("M_Branch", 1.0, 1.0, 2, 10);
    resman.CreateSphere    ("M_Leaf", 1.0, 4, 10);
    resman.CreateSphere    ("M_Sphere", 1.0, 110, 10);
    resman.CreatePointCloud("M_StarCloud", 70000, 2000, {0.8, 0.8, 0.8, 0.8});
    resman.CreateSphere    ("M_Planet", 1, 200, 200);

    std::cout << "meshes loaded" << std::endl;
}

void Game::LoadShaders() {
    // load shader programs
    resman.LoadShader("S_Default", SHADER_DIRECTORY"/material_vp.glsl", SHADER_DIRECTORY"/material_fp.glsl");
    resman.LoadShader("S_Lit", SHADER_DIRECTORY"/lit_vp.glsl", SHADER_DIRECTORY"/lit_fp.glsl");
    resman.LoadShader("S_Text", SHADER_DIRECTORY"/text_vp.glsl", SHADER_DIRECTORY"/text_fp.glsl");
    resman.LoadShader("S_Planet", SHADER_DIRECTORY"/ship_vp.glsl", SHADER_DIRECTORY"/textured_fp.glsl");
    resman.LoadShader("S_NormalMap", SHADER_DIRECTORY"/normal_map_vp.glsl", SHADER_DIRECTORY"/normal_map_fp.glsl");
    resman.LoadShader("S_Instanced", SHADER_DIRECTORY"/instanced_normal_map_vp.glsl", SHADER_DIRECTORY"/normal_map_fp.glsl", true);

    std::cout << "shaders loaded" << std::endl;
}

void Game::LoadTextures() {
    // load textures
    resman.LoadTexture("T_Charmap", RESOURCES_DIRECTORY"/fixedsys_alpha.png", GL_CLAMP_TO_EDGE);
    // resman.LoadTexture("T_LavaPlanet", RESOURCES_DIRECTORY"/lava_planet.png", GL_REPEAT, GL_NEAREST, 4.0f);
    resman.LoadTexture("T_Ship", RESOURCES_DIRECTORY"/shiptex.png", GL_REPEAT);
    // resman.LoadTexture("T_LavaPlanet", RESOURCES_DIRECTORY"/lava_planet.png", GL_REPEAT, GL_NEAREST, 4.0f);
    // resman.LoadTexture("T_SnowPlanet", RESOURCES_DIRECTORY"/snow_planet.png", GL_LINEAR);
    resman.LoadTexture("T_MarsPlanet", RESOURCES_DIRECTORY"/8k_mars.jpg", GL_REPEAT);
    // resman.LoadTexture("T_RockPlanet", RESOURCES_DIRECTORY"/mine_rocks.png", GL_REPEAT);
    // resman.LoadTexture("T_RedPlanet", RESOURCES_DIRECTORY"/red_rubble.png", GL_REPEAT);
    resman.LoadTexture("T_MoonPlanet", RESOURCES_DIRECTORY"/4k_ceres.jpg", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_KaliaPlanet", RESOURCES_DIRECTORY"/kalia.png", GL_REPEAT);
    // resman.LoadTexture("T_KaliaPlanet", RESOURCES_DIRECTORY"/kalia.png", GL_REPEAT);
    resman.LoadTexture("T_WallNormalMap", RESOURCES_DIRECTORY"/normal_map2.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_RockNormalMap", RESOURCES_DIRECTORY"/lavarock_normalmap.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Grass", RESOURCES_DIRECTORY"/whispy_grass_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_GrassNormalMap", RESOURCES_DIRECTORY"/whispy_grass_normal_map.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MetalNormalMap", RESOURCES_DIRECTORY"/metal_normal_map.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Tree4", RESOURCES_DIRECTORY"/tree4_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_TreeNormalMap", RESOURCES_DIRECTORY"/tree4_normal_map.png", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_Tree", RESOURCES_DIRECTORY"/oak_texture.png", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_Tree", RESOURCES_DIRECTORY"/birch_tree_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Tree", RESOURCES_DIRECTORY"/lowpolytree_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_RuggedTerrain", RESOURCES_DIRECTORY"/rugged_terrain_texture.png", GL_REPEAT, GL_LINEAR);

    std::cout << "textures loaded" << std::endl;
}


void Game::SetupScenes(void){
    // allocate all scenes
    for(int i = 0; i < SceneEnum::NUM_SCENES; i++) {
        scenes.push_back(new SceneGraph(app));
    }

    active_scene = scenes[FPTEST];
    active_scene->SetBackgroundColor(viewport_background_color_g);


    //player created temporarily just so when controls query for player not null
    //once player refactor shoudl fix 
    //also for assignment can just remove control code from game temporarily
    CreatePlayer();

    SetupSpaceScene();
    SetupFPScene(); // FPS TEST SCENE
    SetupForestScene();



    // // CreateTerrain();
    // CreatePlanets();
    // CreateTriggers();
    // // CreateTree();
    CreateLights();
    CreateHUD();
}

void Game::SetupSpaceScene() {
    SceneGraph* scn = scenes[SPACE];
    Camera& camera = scn->GetCamera();
    camera.SetView(config::camera_position, config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    Player* player = new Tp_Player("Obj_Player", "M_Ship", "S_NormalMap", "T_Ship");
    player->transform.SetPosition(player_position_g);
    player->SetNormalMap("T_MetalNormalMap", 1.0);
    camera.Attach(&player->transform, true);
    scn->SetPlayer(player);
    scn->AddNode(player);

    SceneNode* stars = new SceneNode("Obj_Starcloud", "M_StarCloud", "S_Default", "");
    scn->AddNode(stars);

    SceneNode* planet = new SceneNode("Obj_Planet", "M_Planet", "S_NormalMap", "T_MarsPlanet");
    planet->transform.SetPosition({0.0, 0.0, -2000.0});
    planet->transform.SetScale({800, 800, 800});
    planet->transform.SetOrientation(glm::angleAxis(PI/1.5f, glm::vec3(1.0, 0.0, 0.0)));
    planet->SetNormalMap("T_RockNormalMap", 4.0f);
    scn->AddNode(planet);

    Light* light = new Light(Colors::WarmWhite);
    light->transform.SetPosition({300.0, 300.0, 0.0});
    scn->AddLight(light);

    Light* l2 = new Light(Colors::Yellow);
    l2->transform.SetPosition({-300.0, -300.0, -300.0});
    scn->AddLight(l2);
    // Light* flashlight = new Light(Colors::Red);
    // l2->transform.SetPosition({-300.0, -300.0, 0.0});
    // l2->Attach(&player->transform);
    // scn->AddLight(l2);
}

void Game::SetupFPScene(void) {
    Camera& camera = scenes[FPTEST]->GetCamera();
    camera.SetView(config::fp_camera_position, config::fp_camera_position + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    FP_Player* p = new FP_Player("Obj_FP_Player", "M_Ship", "S_Lit", "T_Ship", &camera);
    p->transform.SetPosition(player_position_g);
    p->visible = false;
    AddPlayerToScene(FPTEST, p);

    SceneNode* n = new SceneNode("Shippy", "M_Ship", "S_Lit");
    n->transform.SetPosition(glm::vec3(0,0,-10));
    AddToScene(FPTEST, n);

    int terrain_size = 1000;
    Terrain* t = new Terrain("Obj_MoonTerrain", "M_MoonTerrain", "S_NormalMap", "T_MoonPlanet", TerrainType::MOON, terrain_size, terrain_size, 0.25, this);
    t->transform.Translate({-terrain_size / 2.0, -30.0, -terrain_size / 2.0});
    t->material.texture_repetition = 6.0f;
    t->SetNormalMap("T_WallNormalMap", 40.0f);
    AddToScene(FPTEST, t);
    p->SetTerrain(t);
}

void Game::SetupForestScene() {
    Camera& camera = scenes[FOREST]->GetCamera();
    camera.SetView(config::fp_camera_position, config::fp_camera_position + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());

    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());
    FP_Player* p = new FP_Player("Obj_FP_Player", "M_Ship", "S_Lit", "T_Ship", &camera);
    p->transform.SetPosition(player_position_g);
    p->visible = false;
    AddPlayerToScene(FOREST, p);
    // scn->SetPlayer(p);
    // scn->AddNode(p);

    int terrain_size = 1000;
    Terrain* terr = new Terrain("Obj_ForestTerrain", "M_ForestTerain", "S_NormalMap", "T_Grass", TerrainType::FOREST, terrain_size, terrain_size, 0.2, this);
    terr->transform.Translate({-terrain_size / 2.0, -30.0, -terrain_size / 2.0});
    terr->material.specular_power = 0.0f;
    terr->material.texture_repetition = 10.0f;
    terr->SetNormalMap("T_GrassNormalMap", 5.0f);
    p->SetTerrain(terr);
    scenes[FOREST]->AddNode(terr);

    Light* light = new Light(Colors::WarmWhite);
    light->transform.SetPosition({300.0, 300.0, 0.0});
    scenes[FOREST]->AddLight(light);

    SceneNode* forest = new SceneNode("Obj_Forest", "M_Tree", "S_Instanced", "T_Tree");
    // forest->transform.SetScale({5, 5, 5});
    forest->SetNormalMap("T_WallNormalMap", 1.0f);
    forest->material.specular_power = 0.0f;
    forest->SetAlphaEnabled(true);
    for(int i = 0; i < 100; i++) {
        bool instanced = true;
        float x = rng.randfloat(-400, 400);
        float z = rng.randfloat(-400, 400);
        float y = terr->SampleHeight(x, z);
        float s = rng.randfloat(3, 10);
        float r = rng.randfloat(0, 2*PI);
        // float s = 1;
        if(instanced) {
            Transform t;
            t.SetPosition({x, y, z});
            t.SetScale({s,s,s});
            t.Yaw(r);
            forest->AddInstance(t);
        } else {
            SceneNode* tree = new SceneNode("Obj_Forest", "M_Tree", "S_Lit", "T_Tree");
            // tree->SetNormalMap("T_TreeNormalMap");
            tree->transform.SetPosition({x, y, z});
            tree->transform.SetScale({s,s,s});
            tree->transform.Yaw(r);
            tree->SetAlphaEnabled(true);
            scenes[FOREST]->AddNode(tree);
        }
    }
    scenes[FOREST]->AddNode(forest);
}

void Game::Update(double dt, KeyMap &keys) {
    CheckControls(keys, dt);
    active_scene->Update(dt);
}


void Game::CheckControls(KeyMap& keys, float dt) {

    // if (keys[GLFW_KEY_ESCAPE]){
    //     app.Quit();
    //     return;
    // }

    // Stop animation if space bar is pressed
    if (keys[GLFW_KEY_ESCAPE]){
        app.ToggleMouseCapture();
        keys[GLFW_KEY_ESCAPE] = false;
    }

    if(keys[GLFW_KEY_T]) {
        // for(auto s : scenes) {
        //     delete s;
        // }
        SetupScenes();
        keys[GLFW_KEY_T] = false;
    }

    if(keys[GLFW_KEY_1]) {
        ChangeScene(FPTEST);
        keys[GLFW_KEY_1] = false;
    }
    if(keys[GLFW_KEY_2]) {
        ChangeScene(SPACE);
        keys[GLFW_KEY_2] = false;
    }
    if(keys[GLFW_KEY_3]) {
        ChangeScene(FOREST);
        keys[GLFW_KEY_3] = false;
    }


    Player* player = active_scene->GetPlayer();
    // Debug print the player's location
    if(player != nullptr) {
    if(keys[GLFW_KEY_P]) {
        glm::vec3 p = player->transform.GetPosition();
        glm::quat o = player->transform.GetOrientation();
        std::cout << glm::to_string(p) << " " << glm::to_string(o) << std::endl;
        keys[GLFW_KEY_P] = false;
    }

    if(keys[GLFW_KEY_RIGHT_BRACKET]) {
        app.ToggleRenderMode();
        keys[GLFW_KEY_RIGHT_BRACKET] = false;
    }

    if(keys[GLFW_KEY_LEFT_SHIFT]) {
        player->Control(Player::Controls::SHIFT, dt);
    };
    if(keys[GLFW_KEY_LEFT_CONTROL]) {
        player->Control(Player::Controls::CTRL, dt);
    };
    if(keys[GLFW_KEY_S]) {
        player->Control(Player::Controls::S, dt);
    };
    if(keys[GLFW_KEY_W]) {
        player->Control(Player::Controls::W, dt);
    };
    if(keys[GLFW_KEY_Q]) {
        player->Control(Player::Controls::Q, dt);
    };
    if(keys[GLFW_KEY_E]) {
        player->Control(Player::Controls::E, dt);
    };
    if(keys[GLFW_KEY_A]) {
        player->Control(Player::Controls::A, dt);
    };
    if(keys[GLFW_KEY_D]) {
        player->Control(Player::Controls::D, dt);
    };
    if(keys[GLFW_KEY_SPACE]) {
        player->Control(Player::Controls::SPACE, dt);
        keys[GLFW_KEY_SPACE] = false;
    };

    if(keys[GLFW_KEY_UP]) {
        active_scene->GetCamera().transform.Pitch(0.05f);
    }
    if(keys[GLFW_KEY_DOWN]) {
        active_scene->GetCamera().transform.Pitch(-0.05f);
    }

    if(keys[GLFW_KEY_X]) {
        if(active_scene->GetCamera().IsAttached()) {
            active_scene->GetCamera().Drop();
        } else {
            active_scene->GetCamera().Attach(&player->transform);
        }
        keys[GLFW_KEY_X] = false;
    }
    }

    if(keys[GLFW_KEY_0]) {
        SetupResources();
        keys[GLFW_KEY_0] = false;
    }


    if(keys[GLFW_KEY_I]) {
        active_scene->GetCamera().transform.Translate({0.0, 0.0, -0.1});
    }
    if(keys[GLFW_KEY_J]) {
        active_scene->GetCamera().transform.Translate({0.0, 0.0,0.1});
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
        if(active_scene->GetCamera().IsAttached()) {
            if(camera_mode++ % 2 == 0) {
                // app.GetCamera().MoveTo({0.0, 3.0f, -20.0f});
            } else {
                active_scene->GetCamera().Reset();
            }
        }
        keys[GLFW_KEY_Z] = false;
    }
}

// void Game::MouseControls(Mouse& mouse) {
//     // float mouse_sens = -0.001f;
//     float mouse_sens = -0.1f;
// 	glm::vec2 look = mouse.move * mouse_sens;

//     // player->transform.Yaw(look.x);
//     Player* player = active_scene->GetPlayer();
//     if(player == nullptr) {return;}
//     player->ShipControl(Player::Controls::YAWL, look.x);
//     player->ShipControl(Player::Controls::PITCHU, look.y);
// }

void Game::CreatePlayer() {
    Player* player = new Tp_Player("Obj_Player", "M_Ship", "S_Lit", "T_Charmap");
    player->transform.SetPosition(player_position_g);
    // player->visible = false;

    active_scene->GetCamera().Attach(&player->transform); // Attach the camera to the player
    AddPlayerToScene(SceneEnum::SPACE, player);
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
        return std::to_string((glm::length(active_scene->GetPlayer()->velocity)));
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


void Game::ChangeScene(int sceneIndex) {
    std::cout << "changing scenes" << std::endl;
    active_scene = scenes[sceneIndex];
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

void Game::ResizeCameras(int width, int height) {
    for(auto s : scenes) {
        s->GetCamera().SetScreenSize(width, height);
    }
}
