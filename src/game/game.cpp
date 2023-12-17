#include "defines.h"
#include "fp_player.h"
#include "glm/gtc/random.hpp"
#include "scene_graph.h"
#include "application.h"
#include "shader.h"
#include "story_data.h"
#include "tp_player.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <memory>
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
#include "thrust.h"
#include "text.h"
#include "terrain.h"
#include "menu_controller.h"
#include "colliders/colliders.h"
#include "rocket.h"
#include "explosion.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    app.ToggleMouseCapture(); // disable mouse by default
}
   
void Game::SetupResources(void){
    LoadMeshes();
    LoadShaders();
    LoadTextures();
}

void Game::LoadMeshes() {
    std::cout << "loading meshes..." << std::endl;
    // load .obj meshes
    resman.LoadMesh        ("M_Ship", MESH_DIRECTORY"/dnafighter-combo.obj");
    resman.LoadMesh        ("M_H2", MESH_DIRECTORY"/h2.obj");
    resman.LoadMesh        ("M_Tree4", MESH_DIRECTORY"/tree4.obj");
    // resman.LoadMesh        ("M_Tree", MESH_DIRECTORY"/oak.obj");
    resman.LoadMesh        ("M_BirchTree", MESH_DIRECTORY"/birch_tree.obj");
    resman.LoadMesh        ("M_Tree", MESH_DIRECTORY"/lowpolytree.obj");
    resman.LoadMesh        ("M_Soldier", MESH_DIRECTORY"/soldier.obj");
    resman.LoadMesh        ("M_Cactus2", MESH_DIRECTORY"/cactus2.obj");
    resman.LoadMesh        ("M_Cactus9", MESH_DIRECTORY"/cactus9.obj");
    resman.LoadMesh        ("M_Cactus8", MESH_DIRECTORY"/cactus8.obj");
    resman.LoadMesh        ("M_MoonTree", MESH_DIRECTORY"/moontree.obj");
    resman.LoadMesh        ("M_SELTower", MESH_DIRECTORY"/moontower.obj");
    resman.LoadMesh        ("M_MoonObject", MESH_DIRECTORY"/moonobj.obj");
    resman.LoadMesh        ("M_MoonCloud", MESH_DIRECTORY"/mooncloud.obj");
    resman.LoadMesh        ("M_Leaf", MESH_DIRECTORY"/yellowleaf.obj");
    resman.LoadMesh        ("M_Branch", MESH_DIRECTORY"/cone.obj");
    resman.LoadMesh        ("M_Monestary", MESH_DIRECTORY"/monestary.obj");
    resman.LoadMesh        ("M_Tower_m", MESH_DIRECTORY"/medium_tower.obj");
    resman.LoadMesh        ("M_Tower_s", MESH_DIRECTORY"/small_tower.obj");
    resman.LoadMesh        ("M_Tower_t", MESH_DIRECTORY"/tall_tower.obj");

    // generate geometry
    resman.CreateSimpleQuad("M_Quad");
    // resman.CreateCone2      ("M_Branch", 1.0, 1.0, 5, 23);
    // resman.CreateSphere    ("M_Leaf", 1.0, 4, 10);
    resman.CreateSphere    ("M_Sphere", 1.0, 110, 10);
    resman.CreatePointCloud("M_StarCloud", 70000, 2000, {0.8, 0.8, 0.8, 0.8});
    resman.CreateSphere    ("M_Planet", 1, 100, 100);
    resman.CreateSphere    ("M_Asteroid", 1, 7, 5);
    resman.CreatePointCloud("M_Thrust", 1200, 0.25f);
    resman.CreatePointCloud("M_Explosion", 5000, 0.25f);
    resman.CreateSnowParticles("M_MoonSnow", 10000, 200, 20, 10.0);
    resman.CreateSnowParticles("M_MoonSpirals", 10000, 2000, 20, 0);
    resman.CreateCone("M_Rocket", 1.0, 0.5, 4, 4);

    resman.CreateSaplingQuad("M_Sapling");
    //resman.CreateCone2       ("M_MoonObject", 10, 3, 4, 4);
    resman.CreateSphere   ("M_Beacon", 1.0, 20, 10);

    resman.CreateSimpleCube("M_Skybox");

}

void Game::LoadShaders() {
    std::cout << "loading shaders..." << std::endl;
    // load shader programs
    resman.LoadShader("S_Default", SHADER_DIRECTORY"/material_vp.glsl", SHADER_DIRECTORY"/material_fp.glsl");
    resman.LoadShader("S_Lit", SHADER_DIRECTORY"/lit_vp.glsl", SHADER_DIRECTORY"/lit_fp.glsl");
    resman.LoadShader("S_Text", SHADER_DIRECTORY"/text_vp.glsl", SHADER_DIRECTORY"/text_fp.glsl");
    resman.LoadShader("S_Planet", SHADER_DIRECTORY"/ship_vp.glsl", SHADER_DIRECTORY"/textured_fp.glsl");
    resman.LoadShader("S_NormalMap", SHADER_DIRECTORY"/normal_map_vp.glsl", SHADER_DIRECTORY"/normal_map_fp.glsl");
    resman.LoadShader("S_NormalMapNoShadow", SHADER_DIRECTORY"/normal_map_vp.glsl", SHADER_DIRECTORY"/normal_map_noshadows_fp.glsl");
    resman.LoadShader("S_Instanced", SHADER_DIRECTORY"/instanced_normal_map_vp.glsl", SHADER_DIRECTORY"/normal_map_noshadows_fp.glsl", "", true);
    resman.LoadShader("S_Lava", SHADER_DIRECTORY"/lit_vp.glsl", SHADER_DIRECTORY"/lit_lava_fp.glsl");
    resman.LoadShader("S_Sun", SHADER_DIRECTORY"/lit_vp.glsl", SHADER_DIRECTORY"/sun_fp.glsl");
    resman.LoadShader("S_Skybox", SHADER_DIRECTORY"/skybox_vp.glsl", SHADER_DIRECTORY"/skybox_fp.glsl");
    resman.LoadShader("S_Texture", SHADER_DIRECTORY"/passthrough_vp.glsl", SHADER_DIRECTORY"/passthrough_fp.glsl");
    resman.LoadShader("S_Heatstroke", SHADER_DIRECTORY"/passthrough_vp.glsl", SHADER_DIRECTORY"/desert_heat_vision_fp.glsl");
    resman.LoadShader("S_TextureWithTransform", SHADER_DIRECTORY"/passthrough_with_transform_vp.glsl", SHADER_DIRECTORY"/passthrough_fp.glsl");
    resman.LoadShader("S_ShowDepth", SHADER_DIRECTORY"/passthrough_vp.glsl", SHADER_DIRECTORY"/show_depth_fp.glsl");
    resman.LoadShader("S_Depth", SHADER_DIRECTORY"/depth_vp.glsl", SHADER_DIRECTORY"/depth_fp.glsl");
    resman.LoadShader("S_InstancedDepth", SHADER_DIRECTORY"/depth_instanced_vp.glsl", SHADER_DIRECTORY"/depth_fp.glsl", "", true);
    resman.LoadShader("S_InstancedShadow", SHADER_DIRECTORY"/instanced_normal_map_vp.glsl", SHADER_DIRECTORY"/normal_map_fp.glsl", "", true);
    resman.LoadShader("S_Thrust", SHADER_DIRECTORY"/thrust_vp.glsl", SHADER_DIRECTORY"/thrust_fp.glsl", SHADER_DIRECTORY"/thrust_gp.glsl");
    resman.LoadShader("S_MoonSnow", SHADER_DIRECTORY"/snow_vp.glsl", SHADER_DIRECTORY"/snow_fp.glsl", SHADER_DIRECTORY"/snow_gp.glsl");
    resman.LoadShader("S_MoonSpiral", SHADER_DIRECTORY"/spiral_vp.glsl", SHADER_DIRECTORY"/spiral_fp.glsl", SHADER_DIRECTORY"/spiral_gp.glsl");
    resman.LoadShader("S_SpaceBugs", SHADER_DIRECTORY"/spacebug_vp.glsl", SHADER_DIRECTORY"/spacebug_fp.glsl", SHADER_DIRECTORY"/spacebug_gp.glsl");
    resman.LoadShader("S_Explosion", SHADER_DIRECTORY"/explosion_vp.glsl", SHADER_DIRECTORY"/explosion_fp.glsl", SHADER_DIRECTORY"/explosion_gp.glsl");
    resman.LoadShader("S_Violence", SHADER_DIRECTORY"/red_vision_vp.glsl", SHADER_DIRECTORY"/red_vision_fp.glsl");
    resman.LoadShader("S_SSDither", SHADER_DIRECTORY"/passthrough_vp.glsl", SHADER_DIRECTORY"/dither_fp.glsl");

    resman.SetScreenSpaceShader("S_Texture");
}

void Game::LoadTextures() {
    std::cout << "loading textures..." << std::endl;
    // load textures
    resman.LoadTexture("T_Charmap", TEXTURE_DIRECTORY"/fixedsys_alpha.png", GL_CLAMP_TO_EDGE);
    resman.LoadTexture("T_LavaPlanet", TEXTURE_DIRECTORY"/lava_planet.png", GL_REPEAT, GL_NEAREST);
    resman.LoadTexture("T_Ship", TEXTURE_DIRECTORY"/dnafighter-combo.png", GL_REPEAT);
    resman.LoadTexture("T_H2", TEXTURE_DIRECTORY"/shiptex.png", GL_REPEAT);
    // resman.LoadTexture("T_LavaPlanet", TEXTURE_DIRECTORY"/lava_planet.png", GL_REPEAT, GL_NEAREST, 4.0f);
    // resman.LoadTexture("T_SnowPlanet", TEXTURE_DIRECTORY"/snow_planet.png", GL_LINEAR);
    resman.LoadTexture("T_MarsPlanet", TEXTURE_DIRECTORY"/8k_mars.jpg", GL_REPEAT);
    resman.LoadTexture("T_ForestPlanet", TEXTURE_DIRECTORY"/alpine_planet.png", GL_REPEAT);
    resman.LoadTexture("T_DesertPlanet", TEXTURE_DIRECTORY"/8k_venus_surface.jpg", GL_REPEAT);
    resman.LoadTexture("T_MoonPlanet", TEXTURE_DIRECTORY"/kalia.png", GL_REPEAT);
    resman.LoadTexture("T_Sun", TEXTURE_DIRECTORY"/8k_sun.jpg", GL_REPEAT);
    // resman.LoadTexture("T_RockPlanet", TEXTURE_DIRECTORY"/mine_rocks.png", GL_REPEAT);
    resman.LoadTexture("T_RedPlanet", TEXTURE_DIRECTORY"/redstone.png", GL_REPEAT);
    //resman.LoadTexture("T_MoonPlanet", TEXTURE_DIRECTORY"/4k_ceres.jpg", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_KaliaPlanet", TEXTURE_DIRECTORY"/kalia.png", GL_REPEAT);
    // resman.LoadTexture("T_KaliaPlanet", TEXTURE_DIRECTORY"/kalia.png", GL_REPEAT);
    resman.LoadTexture("T_WallNormalMap", TEXTURE_DIRECTORY"/normal_map2.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_RockNormalMap", TEXTURE_DIRECTORY"/lavarock_normalmap.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Grass", TEXTURE_DIRECTORY"/whispy_grass_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_GrassNormalMap", TEXTURE_DIRECTORY"/whispy_grass_normal_map.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MetalNormalMap", TEXTURE_DIRECTORY"/metal_normal_map.png", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_Tree4", TEXTURE_DIRECTORY"/tree4_texture.png", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_TreeNormalMap", TEXTURE_DIRECTORY"/tree4_normal_map.png", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_Tree", TEXTURE_DIRECTORY"/oak_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_BirchTree", TEXTURE_DIRECTORY"/birch_texture.png", GL_REPEAT, GL_LINEAR);
    // resman.LoadTexture("T_BirchNormalMap", TEXTURE_DIRECTORY"/birch_normal_map.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Tree", TEXTURE_DIRECTORY"/lowpolytree_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MoonObj1", TEXTURE_DIRECTORY"/whitedevil.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MoonObj2", TEXTURE_DIRECTORY"/reddevil.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MoonEyes", TEXTURE_DIRECTORY"/mooneyes.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Soldier", TEXTURE_DIRECTORY"/soldier_texture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Cactus2", TEXTURE_DIRECTORY"/cactus2.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Cactus2_n", TEXTURE_DIRECTORY"/cactus2_n.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Cactus9", TEXTURE_DIRECTORY"/cactus9.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Cactus9_n", TEXTURE_DIRECTORY"/cactus9_n.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Cactus8", TEXTURE_DIRECTORY"/cactus8.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Cactus8_n", TEXTURE_DIRECTORY"/cactus8_n.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Sand", TEXTURE_DIRECTORY"/sand.jpg", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Sand_n", TEXTURE_DIRECTORY"/sand_n.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Cursor", TEXTURE_DIRECTORY"/cursor.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    resman.LoadTexture("T_Splash", TEXTURE_DIRECTORY"/splash.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    resman.LoadTexture("T_Pill", TEXTURE_DIRECTORY"/scaledpill.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MoonTree", TEXTURE_DIRECTORY"/moontreetex.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_SpaceMetal", TEXTURE_DIRECTORY"/spacemetal.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Beacon", TEXTURE_DIRECTORY"/beacon.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Fire", RESOURCES_DIRECTORY"/flame4x4orig.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MoonFlesh", TEXTURE_DIRECTORY"/moonflesh.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Unforgivable", TEXTURE_DIRECTORY"/unforgivable.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_MoonEye", TEXTURE_DIRECTORY"/mooneye.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Stone", TEXTURE_DIRECTORY"/stone.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_SpiralParticle", TEXTURE_DIRECTORY"/flake.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_EyeParticle", TEXTURE_DIRECTORY"/eyepart.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Rocket", TEXTURE_DIRECTORY"/stone_old.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Bark", TEXTURE_DIRECTORY"/hairybark.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_YellowLeaf", TEXTURE_DIRECTORY"/yellow_leaf.png", GL_REPEAT, GL_LINEAR);

    resman.LoadTexture("T_Picture", TEXTURE_DIRECTORY"/picture.png", GL_REPEAT, GL_LINEAR);
    resman.LoadTexture("T_Tower_t", TEXTURE_DIRECTORY"/tall_tower.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    resman.LoadTexture("T_Tower_t_n", TEXTURE_DIRECTORY"/tall_tower_n.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    resman.LoadTexture("T_Tower_s", TEXTURE_DIRECTORY"/small_tower.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    resman.LoadTexture("T_Tower_s_n", TEXTURE_DIRECTORY"/small_tower_n.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    resman.LoadTexture("T_Tower_m", TEXTURE_DIRECTORY"/medium_tower.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    resman.LoadTexture("T_Tower_m_n", TEXTURE_DIRECTORY"/medium_tower_n.png", GL_CLAMP_TO_EDGE, GL_LINEAR);

    resman.LoadCubemap("T_SpaceSkybox", TEXTURE_DIRECTORY"/skyboxes/space");
    resman.LoadCubemap("T_MessedUpSkybox", TEXTURE_DIRECTORY"/skyboxes/messedup");
    resman.LoadCubemap("T_BlueSkybox", TEXTURE_DIRECTORY"/skyboxes/bluesky", false);
}


void Game::SetupScenes(void){
    std::cout << "loading scenes..." << std::endl;
    // allocate all scenes
    for(int i = 0; i < SceneEnum::NUM_SCENES; i++) {
        scenes.push_back(new SceneGraph(this));
    }


    //player created temporarily just so when controls query for player not null
    //once player refactor shoudl fix 
    //also for assignment can just remove control code from game temporarily/
    // CreatePlayer();

    SetupSpaceScene();
    SetupFPScene(); // FPS TEST SCENE
    SetupForestScene();
    SetupDesertScene();
    SetupMainMenuScene();
    SetupStartScene();
    SetupCreditsScene();

    ChangeScene(START);


    // // CreateTerrain();
    // CreatePlanets();
    // CreateTriggers();
    // // CreateTree();
    CreateLights();
    // CreateHUD();
    CreateStory();
}

void Game::SetupSpaceScene() {
    SceneGraph* scn = scenes[SPACE];
    CreateFPSCounter(SPACE);
    CreateShipHUD(SPACE);

    scn->SetResetCallback([this]() { this->SetupSpaceScene(); });

    Camera& camera = scn->GetCamera();
    camera.SetView({0.000000, 8.087938, 20.895229}, config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, 16000.0f, app.GetWinWidth(), app.GetWinHeight());
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    auto stars = std::make_shared<SceneNode>("Obj_Starcloud", "M_StarCloud", "S_Default", "");
    // scn->AddNode(stars);

    auto planet = std::make_shared<SceneNode>("Obj_Planet", "M_Planet", "S_NormalMap", "T_ForestPlanet");
    planet->transform.SetPosition({0.0, 0.0, -2000.0});
    planet->transform.SetScale({800, 800, 800});
    planet->transform.SetOrientation(glm::angleAxis(PI/1.5f, glm::vec3(1.0, 0.0, 0.0)));
    planet->SetNormalMap("T_RockNormalMap", 4.0f);
    planet->material.specular_coefficient = 0.0f;
    SphereCollider* col = new SphereCollider(*planet, 800);
    col->SetCallback([this]() { this->ShipHitPlanet({0.0f,0.0f,0.0f}); });
    planet->SetCollider(col);
    AddColliderToScene(SPACE, planet);

    auto planet2 = std::make_shared<SceneNode>("Obj_Planet", "M_Planet", "S_NormalMap", "T_DesertPlanet");
    planet2->transform.SetPosition({-3500, 3000, -6000.0});
    planet2->transform.SetScale({1100, 1100, 1100});
    planet2->transform.SetOrientation(glm::angleAxis(PI/-1.5f, glm::vec3(1.0, 0.0, 0.0)));
    planet2->SetNormalMap("T_RockNormalMap", 4.0f);
    planet2->material.specular_coefficient = 0.0f;
    SphereCollider* p2col = new SphereCollider(*planet2, 1100);
    p2col->SetCallback([this]() { this->ShipHitPlanet({-3000, 3000, -3500.0}); });
    planet2->SetCollider(p2col);
    AddColliderToScene(SPACE, planet2);

    auto planet3 = std::make_shared<SceneNode>("Obj_Planet", "M_Planet", "S_NormalMap", "T_MoonPlanet");
    planet3->transform.SetPosition({-5500, 5000, -15000.0});
    planet3->transform.SetScale({500, 500, 500});
    planet3->transform.SetOrientation(glm::angleAxis(PI/1.8f, glm::normalize(glm::vec3(0.9, 0.2, 0.0))));
    planet3->SetNormalMap("T_RockNormalMap", 4.0f);
    planet3->material.specular_coefficient = 0.0f;
    SphereCollider* p3col = new SphereCollider(*planet3, 500);
    p3col->SetCallback([this]() { this->ShipHitPlanet({-5500, 4000, -14000.0}); });
    planet3->SetCollider(p3col);
    AddColliderToScene(SPACE, planet3);

    auto light = std::make_shared<Light>(Colors::WarmWhite);
    light->transform.SetPosition({300.0, 300.0, 15000.0});
    scn->AddLight(light);

    auto l2 = std::make_shared<Light>(Colors::Yellow);
    l2->transform.SetPosition({-300.0, -300.0, 15000.0});
    scn->AddLight(l2);

    auto sun = std::make_shared<SceneNode>("Obj_Planet", "M_Planet", "S_Sun", "T_Sun");
    sun->transform.SetPosition({300, 300, 15000.0});
    sun->transform.SetScale({2000, 2000, 2000});
    sun->transform.SetOrientation(glm::angleAxis(PI/1.5f, glm::vec3(1.0, 0.0, 0.0)));
    sun->material.specular_coefficient = 2.0f;
    sun->material.diffuse_strength = 10.0f;
    SphereCollider* suncol = new SphereCollider(*sun, 2000);
    suncol->SetCallback([this]() { this->PlayerHitRespawnMessage({0.0, 0.0, 0.0}, "Closer than Icarus ever got you think.\n The star swallows every atom... "); });
    sun->SetCollider(suncol);
    AddColliderToScene(SPACE, sun);

    auto skybox = std::make_shared<SceneNode>("Obj_Skybox", "M_Skybox", "S_Skybox", "T_SpaceSkybox");
    skybox->transform.SetScale({2000, 2000, 2000});
    scenes[SPACE]->SetSkybox(skybox);

    auto astr = std::make_shared<SceneNode>("Obj_Forest", "M_Asteroid", "S_Instanced", "T_LavaPlanet");
    astr->SetNodeType(NodeType::TASTEROID);
    astr->material.specular_power = 3000.0f;
    astr->material.specular_coefficient = 0.0f;
    astr->SetNormalMap("T_RockNormalMap", 4.0f);

    for(int i = 0; i < 3; i++) {
        float radius = 8000.0f;
        glm::vec3 base_pos = {0.0, 0.0, -radius*i};

        for(int i = 0; i < 3000; i++) {
            bool instanced = true;
            glm::vec3 pos = base_pos + glm::ballRand(radius);
            float s = rng.randfloat(10, 30);
            float y = rng.randfloat(0, 2*PI);
            float r = rng.randfloat(0, 2*PI);
            float p = rng.randfloat(0, 2*PI);
            Transform t;
            t.SetPosition(pos);
            t.SetScale({s,s,s});
            t.Yaw(y);
            t.Roll(r);
            t.Pitch(p);
            astr->AddInstance(t);
        }
    }
    scenes[SPACE]->AddNode(astr);
    scenes[SPACE]->AddCollider(astr);

    auto player = std::make_shared<Tp_Player>("Obj_Player", "M_Ship", "S_NormalMap", "T_Ship", this);
    player->transform.SetPosition(glm::vec3(1679.251343, 727.375793, -537.549316));

    player->SetNormalMap("T_MetalNormalMap", 1.0);
    camera.Attach(&player->transform, false);
    scn->SetPlayer(player);
    scn->AddNode(player);

    float thrust_scale = 0.05f;
    Thrust* thrust1 = new Thrust("Obj_Ship", "M_Thrust", "S_Thrust", "T_Fire");
    thrust1->transform.SetPosition(glm::vec3(-0.85, -0.25, 5.0));
    thrust1->SetAlphaEnabled(true);
    thrust1->SetAlphaFunc(GL_ONE);
    // thrust1->transform.SetScale({thrust_scale, thrust_scale, thrust_scale});
    player->AddChild(thrust1);
    player->thrust1 = thrust1;

    Thrust* thrust2 = new Thrust("Obj_Ship", "M_Thrust", "S_Thrust", "T_Fire");
    thrust2->transform.SetPosition(glm::vec3(0.85, -0.25, 5.0));
    thrust2->SetAlphaEnabled(true);
    thrust2->SetAlphaFunc(GL_ONE);
    // thrust2->transform.SetScale({thrust_scale, thrust_scale, thrust_scale});
    player->AddChild(thrust2);
    player->thrust2 = thrust2;

    auto beacon1 = std::make_shared<Beacon>("Obj_Beacon", "M_Beacon", "S_Lit", "T_Beacon");
    beacon1->material.specular_coefficient = 0.0f;
    beacon1->material.diffuse_strength = 2.0f;
    beacon1->transform.SetPosition(planet->transform.GetPosition());
    beacon1->SetAlphaEnabled(true);
    beacon1->SetCollectCallback([this]() { this->BeaconOneHit(); });
    beacon1->transform.Translate({0,planet->transform.GetScale().y * 0.5f,planet->transform.GetScale().z * 0.85f});
    AddColliderToScene(SPACE, beacon1);

    auto beacon2 = std::make_shared<Beacon>("Obj_Beacon", "M_Beacon", "S_Lit", "T_Beacon");
    beacon2->material.specular_coefficient = 0.0f;
    beacon2->material.diffuse_strength = 2.0f;
    beacon2->transform.SetPosition(planet2->transform.GetPosition());
    beacon2->SetAlphaEnabled(true);
    beacon2->SetCollectCallback([this]() { this->ChangeSceneAndSpawn(DESERT, {0,100,0}); });
    beacon2->transform.Translate({0,planet2->transform.GetScale().y,0});
    AddColliderToScene(SPACE, beacon2);

    auto beacon3 = std::make_shared<Beacon>("Obj_Beacon", "M_Beacon", "S_Lit", "T_Beacon");
    beacon3->material.specular_coefficient = 0.0f;
    beacon3->material.diffuse_strength = 2.0f;
    beacon3->transform.SetPosition(planet3->transform.GetPosition());
    beacon3->SetAlphaEnabled(true);
    beacon3->SetCollectCallback([this]() { this->ChangeSceneAndSpawn(FPTEST, {0,0,0}); });
    beacon3->transform.Translate({0,0,planet3->transform.GetScale().z});
    AddColliderToScene(SPACE, beacon3);
}


const std::vector<std::vector<float>> Game::readTerrain(const std::string& filePath) {
    int width, height, channels;
    unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_grey);

    if (!image) {
        std::cout << "Error: Unable to read terrain" << filePath << std::endl;
        return {};
    }

    std::vector<std::vector<float>> floatImage(height, std::vector<float>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            floatImage[i][j] = (image[i * width + j]) / 255.0f;
        }
    }
    stbi_image_free(image);
    return floatImage;
}

void Game::SetupFPScene(void) {
    CreateFPSCounter(FPTEST);

    scenes[FPTEST]->SetResetCallback([this]() { this->SetupFPScene(); });
    Camera& camera = scenes[FPTEST]->GetCamera();
    camera.SetView(glm::vec3(0.0, 3.0, -0.4), glm::vec3(0.0, 3.0, -0.4) + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    auto p = std::make_shared<FP_Player>("Obj_FP_Player", "M_Soldier", "S_NormalMap", "T_Soldier", &camera);
    p->SetNormalMap("T_MetalNormalMap");
    p->transform.SetPosition(player_position_g);
    p->transform.SetScale({3.5, 5.0, 3.5});
    AddPlayerToScene(FPTEST, p);

    auto skybox = std::make_shared<SceneNode>("Obj_MoonSkybox", "M_Skybox", "S_Skybox", "T_MessedUpSkybox");
    skybox->transform.SetScale({2000, 2000, 2000});
    scenes[FPTEST]->SetSkybox(skybox);

    const std::vector<std::vector <float>>& gangAintNunOfThatSquad = readTerrain(RESOURCES_DIRECTORY"/terrain/moon.png");
    int terrain_size = 1500;
    auto t = std::make_shared<Terrain>("Obj_MoonTerrain", "M_MoonTerrain", "S_NormalMap", "T_MoonPlanet", TerrainType::MOON, gangAintNunOfThatSquad, terrain_size, terrain_size, 0.2, this);
    t->transform.Translate({-terrain_size / 2.0, -30.0, -terrain_size / 2.0});
    t->SetNormalMap("T_RockNormalMap", 40.0f);
    AddToScene(FPTEST, t);
    p->SetTerrain(t);

    auto lt = std::make_shared<Terrain>("Obj_MoonLava", "M_MoonLava", "S_Lava", "T_MoonPlanet", TerrainType::LAVA, std::vector<std::vector<float>>{}, 400, 400, 0.1, this);
    lt->transform.Translate({-200.0f, -65.0f, -200.0f});
    lt->material.texture_repetition = 6.0f;
    lt->material.diffuse_strength = 1.5f;
    lt->SetNodeType(TLAVA);
    AddColliderToScene(FPTEST, lt);

    auto moonobj = std::make_shared<SceneNode>("Obj_MoonObject", "M_MoonObject", "S_InstancedShadow", "T_MoonObj1");
    moonobj->SetNormalMap("T_WallNormalMap", 0.005f);
    moonobj->material.specular_power = 0.0f;
    for(int i = 0; i < 500; i++) {
        bool instanced = true;
        float x = rng.randfloat(-1200, 1200);
        float z = rng.randfloat(-1200, 1200);
        float y = rng.randfloat(-30, 300);
        float s = rng.randfloat(0.3, 1.3);
        float r1 = rng.randfloat(-2*PI, 2*PI);
        float r2 = rng.randfloat(-2*PI, 2*PI);
        float r3 = rng.randfloat(-2*PI, 2*PI);

        Transform t;
        t.SetPosition({x, y, z});
        t.SetScale({s,s,s});
        t.Yaw(r1);
        t.Pitch(r2);
        t.Roll(r3);
        moonobj->AddInstance(t);
    }
    scenes[FPTEST]->AddNode(moonobj);

    auto moonobj2 = std::make_shared<SceneNode>("Obj_MoonObject", "M_MoonObject", "S_InstancedShadow", "T_MoonObj2");
    moonobj2->SetNormalMap("T_WallNormalMap", 0.005f);
    moonobj2->material.specular_power = 0.0f;
    for(int i = 0; i < 500; i++) {
        bool instanced = true;
        float x = rng.randfloat(-1200, 1200);
        float z = rng.randfloat(-1200, 1200);
        float y = rng.randfloat(-30, 300);
        float s = rng.randfloat(0.3, 1.3);
        float r1 = rng.randfloat(-2*PI, 2*PI);
        float r2 = rng.randfloat(-2*PI, 2*PI);
        float r3 = rng.randfloat(-2*PI, 2*PI);

        Transform t;
        t.SetPosition({x, y, z});
        t.SetScale({s,s,s});
        t.Yaw(r1);
        t.Pitch(r2);
        t.Roll(r3);
        moonobj2->AddInstance(t);
    }
    scenes[FPTEST]->AddNode(moonobj2);

    auto tree = std::make_shared<SceneNode>("Obj_MoonTree", "M_MoonTree", "S_InstancedShadow", "T_MoonTree");
    tree->SetNormalMap("T_WallNormalMap", 1.0f);
    tree->material.specular_power = 150.0;
    std::vector<glm::vec3> tree_points = rng.generateUniqueRandomPoints(100, 10.0f, 750.0f);
    for(int i = 0; i < tree_points.size(); i++) {
        bool instanced = true;
        float x = tree_points[i].x;
        float z = tree_points[i].z;
        float y = t->SampleHeight(x, z);
        float s = rng.randfloat(2, 5);
        float r = rng.randfloat(0, 2*PI);
        if(instanced) {
            Transform tra;
            tra.SetPosition({x, y, z});
            tra.SetScale({s,s,s});
            tra.Yaw(r);
            tree->AddInstance(tra);
        }
    }
    scenes[FPTEST]->AddNode(tree);

    auto mooneyes = std::make_shared<SceneNode>("Obj_MoonEyes", "M_MoonObject", "S_InstancedShadow", "T_MoonEyes");
    mooneyes->material.specular_power = 0.0;
    mooneyes->material.texture_repetition = 3.0f;
    mooneyes->material.ambient_additive = 0.2f;
    mooneyes->SetNormalMap("T_WallNormalMap", 0.005f);
    //mooneyes->material.diffuse_strength = 10.0f;
    for(int i = 0; i < 350; i++) {
        bool instanced = true;
        float x = rng.randfloat(-750, 750);
        float z = rng.randfloat(-750, 750);
        float y = t->SampleHeight(x, z);
        float s = rng.randfloat(2, 6);
        float r1 = rng.randfloat(0, 2*PI);
        float r2 = rng.randfloat(-0.3f, 0.3f);
        if(instanced) {
            Transform tra;
            tra.SetPosition({x, y + s, z});
            tra.SetScale({s/2,s,s/2});
            tra.Yaw(r1);
            tra.Pitch(r2);
            mooneyes->AddInstance(tra);
        }
    }
    scenes[FPTEST]->AddNode(mooneyes);

    auto tower = std::make_shared<SceneNode>("Obj_SpaceTower", "M_SELTower", "S_InstancedShadow", "T_SpaceMetal");
    tower->SetNormalMap("T_MetalNormalMap", 1.0f);
    tower->material.specular_power = 15000.0;
    std::vector<glm::vec3> points = rng.generateUniqueRandomPoints(12, 200.0f, 700.0f);
    for (int i = 0; i < points.size(); ++i) {
        bool instanced = true;
        float x = points[i].x;
        float z = points[i].z;
        float y = t->SampleHeight(x, z);
        float s = rng.randfloat(15, 16);
        float r = rng.randfloat(0, 2*PI);
        if(instanced) {
            Transform tra;
            tra.SetPosition({x, y, z});
            tra.SetScale({s,s,s});
            tra.Yaw(r);
            tower->AddInstance(tra);
        }
    }
    scenes[FPTEST]->AddNode(tower);
    
    std::vector<glm::vec3> cloud_points = rng.generateUniqueRandomPoints(40, 40.0f, 750.0f);
    for (int i = 0; i < cloud_points.size(); ++i) {
        bool instanced = true;
        float x = cloud_points[i].x;
        float z = cloud_points[i].z;
        float y = rng.randfloat(0, 30);
        float s = rng.randfloat(3, 5);
        float r = rng.randfloat(0, 2*PI);
        int eyes = rng.randint(3, 20);
        auto cloud = std::make_shared<MoonCloud>("Obj_MoonCloud", "M_MoonCloud", "S_NormalMap", "T_Unforgivable");
        cloud->material.diffuse_strength = 2.0f;
        cloud->SetNormalMap("T_MetalNormalMap", 1.0f);
        cloud->transform.SetPosition({x, y, z});
        cloud->transform.SetScale({s,s,s});
        cloud->transform.Yaw(r);
        cloud->material.specular_power = 0.0f;
        cloud->SetAlphaEnabled(true);
        for (int j = 0; j < eyes; ++j) {
            MoonEye* eye = new MoonEye("Obj_MoonEye", "M_Asteroid", "S_Lit", "T_MoonEye");
            cloud->AddChild(eye);
            eye->transform.Yaw(rng.randfloat(-2*PI, 2*PI));
            eye->transform.Pitch(rng.randfloat(-2*PI, 2*PI));
            eye->transform.Roll(rng.randfloat(-2*PI, 2*PI));
            glm::vec3 forward = glm::normalize(eye->transform.GetOrientation() * glm::vec3(rng.randfloat(-1, 1), rng.randfloat(-1, 1), rng.randfloat(-1, 1)));
            eye->transform.Translate(forward * 10.0f);
        }
        scenes[FPTEST]->AddNode(cloud);
    }

    auto ship = std::make_shared<SceneNode>("Obj_LandedShip", "M_Ship", "S_NormalMap", "T_Ship");
    ship->SetNormalMap("T_MetalNormalMap", 10.0f);
    ship->transform.SetPosition({-30.0f,-30.0f,30.0f});
    ship->transform.SetOrientation({0.4, 0.0, 0.0, 0.0});
    ship->transform.SetScale({11.0, 11.0, 9.5});
    ship->material.specular_power = 169.0f;
    SphereCollider* col = new SphereCollider(*ship, 9.0f);
    col->SetCallback([this]() { PlayerHitShip({-5500, 5550, -15000.0}); });
    ship->SetCollider(col);
    AddColliderToScene(FPTEST, ship);

    auto pill = std::make_shared<Item>("Obj_Pill", "M_Sapling", "S_Lit", "T_Pill");
    pill->transform.SetPosition({-600.0f,-18.0f,-600.0f});
    pill->transform.SetScale({5,5,5});
    pill->SetAlphaEnabled(true);
    pill->SetCollectCallback([this]() { this->CollectEndingItem(PILL); });
    pill->DeleteOnCollect(true);
    AddColliderToScene(FPTEST, pill);

    auto picture = std::make_shared<Item>("Obj_Picture", "M_Sapling", "S_Lit", "T_Picture");
    float px = 295;
    float pz = 575;
    picture->transform.SetPosition({px, t->SampleHeight(px, pz) + 2, pz});
    picture->transform.SetScale({2,2,2});
    picture->SetAlphaEnabled(true);
    picture->SetCollectCallback([this]() { this->CollectStoryItem(PICTURE); });
    picture->DeleteOnCollect(true);
    AddColliderToScene(FPTEST, picture);

    // Area of effect when in range of pill
    /*Toggle* pill_vision = new Toggle("Obj_Toggle", "M_Sapling", "S_Default", "T_SpiralParticle");
    pill_vision->transform.SetPosition({-600.0f,-18.0f,-600.0f});
    pill_vision->transform.SetScale({1,1,1});
    SphereCollider* col_vision = new SphereCollider(*pill_vision, 150.0f);
    pill_vision->SetCollider(col_vision);
    pill_vision->SetOnCallback([this]() { 
        if (!this->GetBadEnd()) {
            this->resman.SetScreenSpaceShader("S_Violence"); 
        }
    });
    pill_vision->SetOffCallback([this]() { 
        if (!this->GetBadEnd()) {
            this->resman.SetScreenSpaceShader("S_Texture");
        }
    });
    AddColliderToScene(FPTEST, pill_vision);*/

    /*SceneNode* snow = new SceneNode("Obj_MoonSnow", "M_MoonSnow", "S_MoonSnow", "T_SpiralParticle");
    snow->SetAlphaEnabled(true);
    snow->SetAlphaFunc(GL_ONE);
    snow->transform.SetPosition({0.0, -20.0, 0.0});
    scenes[FPTEST]->AddNode(snow);*/
}

void Game::SetupForestScene() {
    // resman.SetScreenSpaceShader("S_SSDither");
    CreateFPSCounter(FOREST);

    scenes[FOREST]->SetResetCallback([this]() { this->SetupForestScene(); });
    Camera& camera = scenes[FOREST]->GetCamera();
    camera.SetView(config::fp_camera_position, config::fp_camera_position + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    const glm::vec3 player_pos = {-191.718155, 20.999252, -395.274536};

    // PLAYER
    auto p = std::make_shared<FP_Player>("Obj_FP_Player", "M_Soldier", "S_NormalMap", "T_Soldier", &camera);
    p->SetNormalMap("T_MetalNormalMap", 1.0f);
    p->material.specular_power = 200.0f;
    // p->transform.SetPosition(player_pos); // real pos
    p->transform.SetPosition({293.913483, 17.500790, 152.102478}); // tmp test
    p->transform.SetOrientation({0.315484, 0.000000, 0.948931, 0.000000});
    p->visible = false;
    p->jump_speed_ = 20;
    p->gravity_ = 9.8* 6;
    AddPlayerToScene(FOREST, p);

    auto ship = std::make_shared<SceneNode>("Obj_LandedShip", "M_Ship", "S_NormalMap", "T_Ship");
    ship->SetNormalMap("T_MetalNormalMap", 10.0f);
    ship->transform.SetPosition({-200.730484, 25.999245, -427.873383});
    ship->transform.SetOrientation({0.334468, 0.000000, 0.942407, 0.000000});
    ship->transform.SetScale({11.0, 11.0, 9.5});
    ship->material.specular_power = 169.0f;
    SphereCollider* col = new SphereCollider(*ship, 9.0f);
    col->SetCallback([this]() { PlayerHitShip({0.0f, 850.0f, -2100.0f}); });
    ship->SetCollider(col);
    AddColliderToScene(FOREST, ship);
    
    const std::vector<std::vector <float>> gangAintNunOfThatSquad = readTerrain(RESOURCES_DIRECTORY"/terrain/mountain_hm_n.png");
    // ENV
    int terrain_size = 1000;
    auto terr = std::make_shared<Terrain>("Obj_ForestTerrain", "M_ForestTerain", "S_NormalMap", "T_Grass", TerrainType::FOREST, gangAintNunOfThatSquad, terrain_size, terrain_size, 0.2, this);
    terr->transform.Translate({-terrain_size / 2.0, -30.0, -terrain_size / 2.0});
    terr->material.specular_power = 0.0f;
    terr->material.texture_repetition = 10.0f;
    // terr->SetNormalMap("T_GrassNormalMap", 5.0f);
    terr->SetNormalMap("T_WallNormalMap", 50.0f);
    p->SetTerrain(terr);
    scenes[FOREST]->AddTerrain(terr);

    auto light = std::make_shared<Light>(Colors::BrightYellow);
    // light->transform.SetPosition({1000.0, 1000.0, -2000.0});
    light->transform.SetPosition({300.0, 600.0, 0.0});
    // ship->transform.SetOrientation({0.334468, 0.000000, 0.942407, 0.000000});
    light->ambient_power = 0.15;
    scenes[FOREST]->AddLight(light);

    auto hilight = std::make_shared<Light>(Colors::Goldish);
    // light->transform.SetPosition({1000.0, 1000.0, -2000.0});
    hilight->transform.SetPosition({800.0, 1300.0, 0.0});
    hilight->ambient_power = 0.025;
    scenes[FOREST]->AddLight(hilight);


    auto skybox = std::make_shared<SceneNode>("Obj_Skybox", "M_Skybox", "S_Skybox", "T_SpaceSkybox");
    skybox->transform.SetScale({2000, 2000, 2000});
    scenes[FOREST]->SetSkybox(skybox);

    // TRANSPARENT
    // SceneNode* forest = new SceneNode("Obj_Forest", "M_Tree", "S_Instanced", "T_Tree");
    auto forest = std::make_shared<SceneNode>("Obj_Forest", "M_BirchTree", "S_InstancedShadow", "T_BirchTree");
    // forest->transform.SetScale({5, 5, 5});
    forest->SetNormalMap("T_WallNormalMap", 0.005f);
    forest->material.specular_power = 150.0;
    forest->SetCullInstances(true);
    for(int i = 0; i < sizeof(forest_trees)/sizeof(forest_trees[0]); i++) {
        bool instanced = true;
        float x = forest_trees[i][0];
        float z = forest_trees[i][2];
        float y = terr->SampleHeight(x, z);
        float s = forest_trees[i][3];
        float r = forest_trees[i][4];
        if(instanced) {
            Transform t;
            t.SetPosition({x, y, z});
            t.SetScale({s,s,s});
            t.Yaw(r);
            forest->AddInstance(t);
        } else {
            auto tree = std::make_shared<SceneNode>("Obj_Forest", "M_BirchTree", "S_NormalMap", "T_BirchTree");
            tree->SetNormalMap("T_WallNormalMap", 1.0f);
            tree->transform.SetPosition({x, y, z});
            tree->transform.SetScale({s,s,s});
            tree->transform.Yaw(r);
            tree->material.specular_power = 0.0f;
            tree->SetAlphaEnabled(true);
            scenes[FOREST]->AddNode(tree);
        }
    }
    scenes[FOREST]->AddNode(forest);

    auto birch = std::make_shared<SceneNode>("Obj_Birch", "M_BirchTree", "S_NormalMap", "T_BirchTree");
    birch->transform.SetPosition({410.245483, 18.229790, -122.216019});
    birch->transform.SetScale({3, 3, 3});
    scenes[FOREST]->AddNode(birch);

    Tree::branch_mesh       = "M_Branch";
    Tree::branch_texture    = "T_Bark";
    Tree::branch_normal_map = "T_RockNormalMap";
    Tree::leaf_mesh         = "M_Leaf";
    Tree::leaf_texture      = "T_YellowLeaf";
    Tree::leaf_normal_map   = "T_MetalNormalMap";

    for(const float* ht : htrees) {
        glm::vec3 pos = {ht[0], ht[1], ht[2]};
        glm::quat ori = {ht[3], ht[4], ht[5], ht[6]};
        auto htree = std::make_shared<Tree>("Tree", "M_Branch", "S_NormalMap", "T_Bark", 0, 0, 0, this);
        htree->transform.SetPosition(pos);
        htree->transform.SetOrientation(ori);
        htree->GrowTree();
        scenes[FOREST]->AddNode(htree);
    }

    auto first_tree = std::make_shared<Item>("Obj_FirstTreeDialogue", "", "S_Lit", "T_Pill", 50.0f);
    first_tree->transform.SetPosition({-388.425018, 21.000000, -272.856903});
    first_tree->transform.SetScale({55,55,55});
    first_tree->SetCollectCallback([this]() { AddStoryToScene(FOREST, StoryBeat::FIRST_TREE); });
    first_tree->DeleteOnCollect(true);
    AddColliderToScene(FOREST, first_tree);

    glm::vec3 crashed_pos = {442.438568, -1.132055, 353.692505};
    auto crashed = std::make_shared<SceneNode>("Obj_CrashedShip", "M_H2", "S_NormalMap", "T_H2");
    crashed->SetNormalMap("T_MetalNormalMap", 10.0f);
    crashed->transform.SetPosition(crashed_pos);
    crashed->transform.SetOrientation({0.975208, {0.076261, -0.193031, -0.076759}});
    crashed->transform.SetScale({11.0, 11.0, 9.5});
    crashed->material.specular_power = 169.0f;
    AddToScene(FOREST, crashed);

    float crash_area_radius = 75.0f;
    auto crash_area = std::make_shared<Item>("Obj_CrashArea", "", "S_Default", "T_SpiralParticle", crash_area_radius);
    crash_area->transform.SetPosition(crashed_pos);
    crash_area->SetCollectCallback([this]() {
        Camera& cam = active_scene->GetCamera();
        cam.Detach();
        cam.Reset();
        cam.transform.SetPosition({442.438568, 80.132055, 353.692505});
        cam.transform.SetOrientation(glm::angleAxis(-PI/2.0f, glm::vec3(1.0, 0.0, 0.0)));
        CollectStoryItem(StoryBeat::CRASH_AREA);
    });
    crash_area->DeleteOnCollect(true);
    AddColliderToScene(FOREST, crash_area);

    auto ship_vision = std::make_shared<Toggle>("Obj_Toggle", "", "S_Default", "T_SpiralParticle");
    ship_vision->transform.SetPosition(crashed->transform.GetPosition());
    SphereCollider* col_vision = new SphereCollider(*ship_vision, crash_area_radius + 20.0f);
    ship_vision->SetCollider(col_vision);
    ship_vision->SetOnCallback([this]() { 
        this->resman.SetScreenSpaceShader("S_SSDither"); 
    });
    ship_vision->SetOffCallback([this]() { 
        this->resman.SetScreenSpaceShader("S_Texture");
    });
    AddColliderToScene(FOREST, ship_vision);

    auto investigate_ship = std::make_shared<Item>("Obj_Investigate", "", "S_Default", "T_SpiralParticle", 15.0f);
    investigate_ship->transform.SetPosition(crashed_pos);
    investigate_ship->SetCollectCallback([this]() {
        Camera& cam = active_scene->GetCamera();
        cam.Detach();
        cam.Reset();
        cam.transform.SetPosition(glm::vec3(448.251495, -4.456905, 341.754303) + glm::vec3(0.000000, 7.083450, 0.000000));
        cam.transform.SetOrientation(glm::quat(0.992476, {0.000000, -0.122440, 0.000000}) * glm::quat(0.993588, {0.111878, 0.016183, -0.002168}));
        CollectStoryItem(StoryBeat::INVESTIGATE_SHIP);
    });
    investigate_ship->DeleteOnCollect(true);
    AddColliderToScene(FOREST, investigate_ship);

}

void Game::SetupDesertScene() {
    CreateFPSCounter(DESERT);
    scenes[DESERT]->SetResetCallback([this]() { this->SetupDesertScene(); });
    Camera& camera = scenes[DESERT]->GetCamera();
    camera.SetView(config::fp_camera_position, config::fp_camera_position + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    auto p = std::make_shared<FP_Player>("Obj_Desert_player", "M_Soldier", "S_NormalMap", "T_Soldier", &camera);
    p->SetNormalMap("T_MetalNormalMap", 1.0f);
    p->transform.SetPosition({-4250,0,-4000});
    p->transform.SetOrientation(glm::quat(0.346089, {0.000000, -0.938202, 0.000000}));
    p->SetTargetStartPos(glm::vec3(-4000,0,-4000));
    AddPlayerToScene(DESERT, p);

    auto ship = std::make_shared<SceneNode>("Obj_LandedShip", "M_Ship", "S_NormalMap", "T_Ship");
    ship->SetNormalMap("T_MetalNormalMap", 10.0f);
    ship->transform.SetPosition({-4040.0f, 120.0f, -4060.0f});
    ship->transform.SetOrientation({0.334468, 0.0, 0.0, 0.0});
    ship->transform.SetScale({11.0, 11.0, 9.5});
    ship->material.specular_power = 169.0f;
    SphereCollider* col = new SphereCollider(*ship, 9.0f);
    col->SetCallback([this]() { PlayerHitShip({-3500.0f, 4200.0f, -6000.0f}); });
    ship->SetCollider(col);
    AddColliderToScene(DESERT, ship);

    std::vector<std::tuple<glm::vec3, std::string>> towerInfo = {
        {{-2950, 225, -2550}, "m"},
        {{-2528.142090, 195, -1170.641357}, "s"},
        {{-2926.778320, 220, 1686.182495}, "t"},
        {{-849.199463, 165, 2641.724121}, "s"}
    };

    for (size_t i = 0; i < towerInfo.size(); ++i) {
        std::string materialType = std::get<1>(towerInfo[i]);
        auto tower = std::make_shared<SceneNode>("Obj_Tower" + std::to_string(i), "M_Tower_" + materialType, "S_NormalMap", "T_Tower_" + materialType);
        tower->SetNormalMap("T_Tower_" + materialType + "_n");
        tower->transform.SetScale(glm::vec3(4));
        tower->transform.SetPosition(std::get<0>(towerInfo[i]));
        tower->material.specular_coefficient = 0;
        tower->SetNodeType(NodeType::TDONTUSECOLLIDER);
        tower->SetCollision(CollisionData(100));
        AddColliderToScene(DESERT, tower);
    }

    const std::vector<std::vector <float>>& gangAintNunOfThatSquad = readTerrain(RESOURCES_DIRECTORY"/terrain/dunes.jpg");

    int terrain_size = 10000;
    auto terr = std::make_shared<Terrain>("Obj_DesertTerrain", "M_DesertTerain", "S_NormalMap", "T_Sand", TerrainType::DUNES, gangAintNunOfThatSquad, terrain_size, terrain_size, 0.2, this);
    terr->transform.Translate({-terrain_size / 2.0, -30.0, -terrain_size / 2.0});
    terr->material.specular_power = 0.0f;
    terr->material.texture_repetition = 50.0f;
    terr->SetNormalMap("T_Sand_n", 50.0f);
    p->SetTerrain(terr);
    scenes[DESERT]->AddTerrain(terr);

    auto light = std::make_shared<Light>(Colors::SunLight);
    light->transform.SetPosition({300.0, 600.0, 0.0});
    light->Attach(&p->transform);
    // light->SetProjMatrix(glm::ortho(-250.0f, 250.0f, -250.0f, 250.0f, 20.0f, 1300.0f));
    scenes[DESERT]->AddLight(light);

    auto skybox = std::make_shared<SceneNode>("Obj_Skybox", "M_Skybox", "S_Skybox", "T_BlueSkybox");
    skybox->transform.SetScale({1000, 1000, 1000});
    skybox->transform.SetOrientation(glm::angleAxis(PI_2, glm::vec3(1.0, 0.0, 0.0)));
    scenes[DESERT]->SetSkybox(skybox);

    auto cactus1 = std::make_shared<SceneNode>("Obj_Cactus9", "M_Cactus9", "S_InstancedShadow", "T_Cactus9");
    auto cactus2 = std::make_shared<SceneNode>("Obj_Cactus2", "M_Cactus2", "S_InstancedShadow", "T_Cactus2");
    auto cactus3 = std::make_shared<SceneNode>("Obj_Cactus8", "M_Cactus8", "S_InstancedShadow", "T_Cactus8");

    cactus1->SetNodeType(NodeType::TDONTUSECOLLIDER);
    cactus1->SetCollision(CollisionData(2.5));
    cactus2->SetNodeType(NodeType::TDONTUSECOLLIDER);
    cactus2->SetCollision(CollisionData(2.5));
    cactus3->SetNodeType(NodeType::TDONTUSECOLLIDER);
    cactus3->SetCollision(CollisionData(2.5));

    cactus1->SetNormalMap("T_Cactus9_n", 1.0f);
    cactus2->SetNormalMap("T_Cactus2_n", 1.0f);
    cactus3->SetNormalMap("T_Cactus8_n", 1.0f);

    cactus1->material.specular_power = 1250.0f;
    cactus1->material.specular_coefficient = 0.1;
    cactus2->material.specular_power = 1250.0f;
    cactus2->material.specular_coefficient = 0.1;
    cactus3->material.specular_power = 1250.0f;
    cactus3->material.specular_coefficient = 0.1;

    float CACTUS_SPAWN_X = 5000.0;
    float CACTUS_SPAWN_Z = 5000.0;

    float x, y, z, s, r;

    Transform cactusTransform;

    for (int i = 0; i < 1000; i++) {
        x = rng.randfloat(-CACTUS_SPAWN_X, CACTUS_SPAWN_X);
        z = rng.randfloat(-CACTUS_SPAWN_Z, CACTUS_SPAWN_Z);
        y = terr->SampleHeight(x, z) - 0.02;
        s = rng.randfloat(8, 14);
        r = rng.randfloat(0, 2 * PI);

        cactusTransform.SetPosition({x, y, z});
        cactusTransform.SetScale({s, s, s});
        cactusTransform.SetOrientation(glm::angleAxis(r, glm::vec3(0, 1, 0)));

        cactus2->AddInstance(cactusTransform);

        x = rng.randfloat(-CACTUS_SPAWN_X, CACTUS_SPAWN_X);
        z = rng.randfloat(-CACTUS_SPAWN_Z, CACTUS_SPAWN_Z);
        y = terr->SampleHeight(x, z) - 0.02;
        s = rng.randfloat(8, 14);
        r = rng.randfloat(0, 2 * PI);

        cactusTransform.SetPosition({x, y, z});
        cactusTransform.SetScale({s, s, s});
        cactusTransform.SetOrientation(glm::angleAxis(r, glm::vec3(0, 1, 0)));

        //should probably use another transform but is copied on pass so this works
        cactus1->AddInstance(cactusTransform);

        x = rng.randfloat(-CACTUS_SPAWN_X, CACTUS_SPAWN_X);
        z = rng.randfloat(-CACTUS_SPAWN_Z, CACTUS_SPAWN_Z);
        y = terr->SampleHeight(x, z) - 0.02;
        s = rng.randfloat(8, 14);
        r = rng.randfloat(0, 2 * PI);

        cactusTransform.SetPosition({x, y, z});
        cactusTransform.SetScale({s, s, s});
        cactusTransform.SetOrientation(glm::angleAxis(r, glm::vec3(0, 1, 0)));

        cactus3->AddInstance(cactusTransform);
    }
    AddColliderToScene(DESERT, cactus1);
    AddColliderToScene(DESERT, cactus2);
    AddColliderToScene(DESERT, cactus3);
}


void Game::SetupMainMenuScene() {
    Camera& camera = scenes[MAIN_MENU]->GetCamera();
    camera.SetView(config::fp_camera_position, config::fp_camera_position + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.transform.SetPosition({0,0,0});
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    RenderBundle mouseCursor = { "M_Quad", "S_TextureWithTransform", "T_Cursor" };
    auto p = std::make_shared<Menu_Player>("Obj_Menu_Player", "M_Quad", "S_Texture", "T_Splash", mouseCursor, app.GetWindow());
    AddPlayerToScene(MAIN_MENU, p);
    Button *button = new Button();
    button->action = std::bind(&Game::ChangeScene, this, START);
    button->topLeftCoordPercentage = {0.74,0.81};
    button->spanPercentage = {0.215, 0.052};
    p->addButton(button);

    button = new Button();
    button->action = std::bind(&Game::ChangeScene, this, FOREST);
    button->topLeftCoordPercentage = {0.785,0.74};
    button->spanPercentage = {0.173, 0.035};
    p->addButton(button);

    button = new Button();
    button->action = std::bind(&Game::ChangeScene, this, DESERT);
    button->topLeftCoordPercentage = {0.785,0.687};
    button->spanPercentage = {0.173, 0.035};
    p->addButton(button);

    button = new Button();
    button->action = std::bind(&Game::ChangeScene, this, FPTEST);
    button->topLeftCoordPercentage = {0.785,0.634};
    button->spanPercentage = {0.173, 0.035};
    p->addButton(button);

    
    camera.Attach(&p->transform, false);

    auto light = std::make_shared<Light>(Colors::WarmWhite);
    light->transform.SetPosition({300.0, 300.0, 0.0});
    AddLightToScene(MAIN_MENU, light);

    auto skybox = std::make_shared<SceneNode>("Obj_MoonSkybox", "M_Skybox", "S_Skybox", "T_SpaceSkybox");
    skybox->transform.SetScale({2000, 2000, 2000});
    scenes[MAIN_MENU]->SetSkybox(skybox);

    float radius = 600.0f;
    // glm::vec3 base_pos = {radius*i, 0.0, 0.0};
    auto astr = std::make_shared<SceneNode>("Obj_Forest", "M_Asteroid", "S_InstancedShadow", "T_LavaPlanet");
    astr->SetNormalMap("T_WallNormalMap", 4.0f);
    astr->material.texture_repetition = 5.0f;
    for (int i = 0; i < 512; i++) {
        glm::vec3 pos = glm::ballRand(radius);
        float s = rng.randfloat(3, 10);
        float y = rng.randfloat(0, 2 * PI);
        float r = rng.randfloat(0, 2 * PI);
        float p = rng.randfloat(0, 2 * PI);
        // float s = 1;
        Transform t;
        t.SetPosition(pos);
        t.SetScale({s, s, s});
        t.Yaw(y);
        t.Roll(r);
        t.Pitch(p);
        astr->AddInstance(t);
    }
    scenes[MAIN_MENU]->AddNode(astr);
}

void Game::SetupStartScene() {
    Camera& camera = scenes[START]->GetCamera();
    camera.SetView(config::fp_camera_position, config::fp_camera_position + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.transform.SetPosition({0,0,0});
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    auto sun = std::make_shared<SceneNode>("Obj_Planet", "M_Planet", "S_Sun", "T_Sun");
    sun->transform.SetPosition({0.0, 0.0, -5000.0});
    sun->transform.SetScale({2000, 2000, 2000});
    sun->transform.SetOrientation(glm::angleAxis(PI/1.5f, glm::vec3(1.0, 0.0, 0.0)));
    //sun->SetNormalMap("T_WallNormalMap", 4.0f);
    sun->material.specular_coefficient = 2.0f;
    sun->material.diffuse_strength = 1.0f;
    scenes[START]->AddNode(sun);

    auto light = std::make_shared<Light>(Colors::WarmWhite);
    light->transform.SetPosition({0.0, 0.0, 300.0});
    AddLightToScene(START, light);

    auto light2 = std::make_shared<Light>(Colors::BrightYellow);
    light2->transform.SetPosition({0.0, 0.0, -5000.0});
    AddLightToScene(START, light2);

    auto skybox = std::make_shared<SceneNode>("Obj_Skybox", "M_Skybox", "S_Skybox", "T_SpaceSkybox");
    skybox->transform.SetScale({2000, 2000, 2000});
    scenes[START]->SetSkybox(skybox);

    auto p = std::make_shared<FP_Player>("Obj_FP_Player", "M_Ship", "S_NormalMap", "T_Ship", &camera);
    p->SetNormalMap("T_MetalNormalMap");
    p->transform.SetPosition({0.0, -1000.0, 0.0});
    p->visible = false;
    AddPlayerToScene(START, p);
    p->SetStatic(true);
    camera.Detach();
    camera.transform.SetPosition({0,0,0});
    camera.transform.SetOrientation(glm::quat());
}


void Game::SetupCreditsScene() {
    Camera& camera = scenes[ENDING]->GetCamera();
    camera.SetView(config::fp_camera_position, config::fp_camera_position + config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, app.GetWinWidth(), app.GetWinHeight());
    camera.transform.SetPosition({0,0,0});
    camera.SetOrtho(app.GetWinWidth(), app.GetWinHeight());

    auto light = std::make_shared<Light>(Colors::WarmWhite);
    light->transform.SetPosition({0.0, 0.0, 300.0});
    AddLightToScene(ENDING, light);

    auto p = std::make_shared<FP_Player>("Obj_FP_Player", "M_Ship", "S_NormalMap", "T_Ship", &camera);
    p->SetNormalMap("T_MetalNormalMap");
    p->transform.SetPosition({0.0, 0.0, 0.0});
    p->visible = false;
    AddPlayerToScene(ENDING, p);
    p->SetStatic(true);
    camera.Detach();
    camera.transform.SetPosition({0,0,0});
    camera.transform.SetOrientation(glm::quat());

    auto skybox = std::make_shared<SceneNode>("Obj_MoonSkybox", "M_Skybox", "S_Skybox", "T_MessedUpSkybox");
    skybox->transform.SetScale({2000, 2000, 2000});
    scenes[ENDING]->SetSkybox(skybox);
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
    if(keys[GLFW_KEY_4]) {
        ChangeScene(DESERT);
        keys[GLFW_KEY_4] = false;
    }
    if(keys[GLFW_KEY_5]) {
        ChangeScene(MAIN_MENU);
        keys[GLFW_KEY_5] = false;
    }
    if(keys[GLFW_KEY_6]) {
        ChangeScene(START);
        keys[GLFW_KEY_6] = false;
    }
    if(keys[GLFW_KEY_7]) {
        ChangeScene(CREDITS);
        keys[GLFW_KEY_7] = false;
    }
    if(keys[GLFW_KEY_9]) {
        resman.SetScreenSpaceShader("S_Texture");
        active_scene->Reset();
        active_scene->SetCollision(true);
        scenes[active_scene_num]->GetPlayer()->transform.SetPosition(current_respawn_position);
        keys[GLFW_KEY_9] = false;
    }


    auto player = active_scene->GetPlayer();


    if (glfwGetMouseButton(app.GetWindow()->ptr, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        player->Control(Player::Controls::LEFTCLICK, dt);
    }

    // Debug print the player's location
    if(keys[GLFW_KEY_P]) {
        glm::vec3 p = player->transform.GetPosition();
        glm::quat o = player->transform.GetOrientation();
        glm::vec3 c = active_scene->GetCamera().transform.GetPosition();
        glm::quat co = active_scene->GetCamera().transform.GetOrientation();
        std::cout << "player: " << glm::to_string(p) << " " << glm::to_string(o) << "\n"
                  << "camera: " << glm::to_string(c) << " " << glm::to_string(co) << "\n" << std::endl;
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
    if(keys[GLFW_KEY_X]) {
        if(active_scene->GetCamera().IsAttached()) {
            active_scene->GetCamera().Drop();
        } else {
            active_scene->GetCamera().Attach(&player->transform);
        }
        keys[GLFW_KEY_X] = false;
    }

    float tilt_speed = 1.5f;
    if(keys[GLFW_KEY_UP]) {
        active_scene->GetCamera().transform.Pitch(tilt_speed * dt);
        // player->Control(Player::Controls::UP, dt);
    }
    if(keys[GLFW_KEY_DOWN]) {
        active_scene->GetCamera().transform.Pitch(-tilt_speed * dt);
        // player->Control(Player::Controls::DOWN, dt);
    }
    if(keys[GLFW_KEY_RIGHT]) {
        active_scene->GetCamera().transform.Yaw(-tilt_speed * dt);
    }
    if(keys[GLFW_KEY_LEFT]) {
        active_scene->GetCamera().transform.Yaw(tilt_speed * dt);
    }
    if(keys[GLFW_KEY_RIGHT_CONTROL]) {
        active_scene->GetCamera().transform.SetOrientation({0.0, 0.0, 0.0, 0.0});
    }


    if(keys[GLFW_KEY_0]) {
        LoadShaders(); 
        keys[GLFW_KEY_0] = false;
    }


    float cam_speed = 3.0f * dt;
    if(keys[GLFW_KEY_I]) {
        active_scene->GetCamera().transform.Translate({0.0, 0.0, -cam_speed});
    }
    if(keys[GLFW_KEY_K]) {
        active_scene->GetCamera().transform.Translate({0.0, 0.0,cam_speed});
    }
    if(keys[GLFW_KEY_U]) {
        active_scene->GetCamera().transform.Translate({0.0, -cam_speed, 0.0});
    }
    if(keys[GLFW_KEY_O]) {
        active_scene->GetCamera().transform.Translate({0.0, cam_speed,0.0});
    }

    if(keys[GLFW_KEY_R]) {
        // Pause at last textbox at start of game, end of game and credits
        if ((active_scene_num == START && active_scene->StoryTextAmount() == 1)
            || (ending_sequence_ && active_scene->StoryTextAmount() == 1)
            || (good_end_ && active_scene->StoryTextAmount() == 1)) {
            keys[GLFW_KEY_R] = false;
            return;
        }
        active_scene->DismissStoryText();

        // Dramatic violence vision
        if (ending_sequence_ && active_scene->StoryTextAmount() == 2) {
            resman.SetScreenSpaceShader("S_Violence"); 
        }

        if(reading_item_ && active_scene->StoryTextAmount() == 0) {
            if(!active_scene->GetCamera().IsAttached()) {
                active_scene->GetCamera().Attach(&active_scene->GetPlayer()->transform);
            }
            active_scene->GetPlayer()->SetStatic(false);
            reading_item_ = false;
            active_scene->SetCollision(true);
        }

        keys[GLFW_KEY_R] = false;
    }

    if(keys[GLFW_KEY_C]) {
        active_scene->ToggleHUD();
        keys[GLFW_KEY_C] = false;
    }

    if(keys[GLFW_KEY_J]) {
        if(active_scene_num == START) {
            active_scene->ClearStoryText();
            ChangeScene(SPACE);
        }
        keys[GLFW_KEY_J] = false;
    }

    if(keys[GLFW_KEY_N]) {
        if(ending_sequence_ && active_scene->StoryTextAmount() == 1) {
            resman.SetScreenSpaceShader("S_Texture");
            ending_sequence_ = false;
            good_end_ = true;
            ChangeScene(ENDING);
            keys[GLFW_KEY_N] = false;
            return;
        }
        keys[GLFW_KEY_N] = false;
    }

    if(keys[GLFW_KEY_Y]) {
        if(ending_sequence_ && active_scene->StoryTextAmount() == 1) {
            active_scene->GetPlayer()->SetStatic(false);
            reading_item_ = false;
            active_scene->ClearStoryText();
            ending_sequence_ = false;
            AddStoryToScene(active_scene_num, BAD_END);
            active_scene->SetCollision(true);
            bad_end_ = true;
            resman.SetScreenSpaceShader("S_Texture");
            keys[GLFW_KEY_Y] = false;
            return;
        }
        keys[GLFW_KEY_Y] = false;
    }

    if(keys[GLFW_KEY_V] && bad_end_) {
        if(violence_mode_) {
            resman.SetScreenSpaceShader("S_Texture");
            violence_mode_ = false;
        } else {
            resman.SetScreenSpaceShader("S_Violence");
            violence_mode_ = true;
        }
        keys[GLFW_KEY_V] = false;
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
                active_scene->GetCamera().transform.SetPosition(config::cockpit_cam_pos);
                active_scene->GetCamera().transform.SetOrientation(config::cockpit_cam_ori);
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
std::shared_ptr<Player> player = std::make_shared<Tp_Player>("Obj_Player", "M_Ship", "S_Lit", "T_Charmap", this);
    player->transform.SetPosition(player_position_g);
    // player->visible = false;

    active_scene->GetCamera().Attach(&player->transform); // Attach the camera to the player
    AddPlayerToScene(SceneEnum::SPACE, player);
}

void Game::AddLightToScene(SceneEnum sceneNum, const std::shared_ptr<Light>& l) {
    if (sceneNum == SceneEnum::ALL) {
        for (auto& s : scenes) {
            s->AddLight(l);
        }
    } else {
        scenes[sceneNum]->AddLight(l);
    }
}

void Game::AddPlayerToScene(SceneEnum sceneNum, const std::shared_ptr<Player>& node) {
    if (sceneNum == SceneEnum::ALL) {
        for (auto& s : scenes) {
            s->SetPlayer(node);
        }
    } else {
        scenes[sceneNum]->SetPlayer(node);
    }
    AddColliderToScene(sceneNum, node);
}

void Game::AddColliderToScene(SceneEnum sceneNum, const std::shared_ptr<SceneNode>& node) {
    if (sceneNum == SceneEnum::ALL) {
        for (auto& s : scenes) {
            s->AddNode(node);
            s->AddCollider(node);
        }
    } else {
        scenes[sceneNum]->AddNode(node);
        scenes[sceneNum]->AddCollider(node);
    }
}

void Game::AddToScene(SceneEnum sceneNum, const std::shared_ptr<SceneNode>& node) {
    if (sceneNum == SceneEnum::ALL) {
        for (auto& s : scenes) {
            s->AddNode(node);
        }
    } else {
        scenes[sceneNum]->AddNode(node);
    }
}

void Game::AddTextToScene(SceneEnum sceneNum, const std::shared_ptr<Text>& text) {
    if (sceneNum == SceneEnum::ALL) {
        for (auto& s : scenes) {
            s->AddText(text);
        }
    } else {
        scenes[sceneNum]->AddText(text);
    }
}


void Game::AddStoryToScene(SceneEnum sceneNum, StoryBeat index) {
    if (sceneNum == SceneEnum::ALL) {
        for (auto& scene : scenes) {
            for (const auto& t : STORY.at(index)) {
                std::shared_ptr<Text> textPtr = std::make_shared<Text>(t);  // Creating a shared_ptr from the existing Text object
                scene->PushStoryText(textPtr);
            }
        }
    } else {
        for (const auto& t : STORY.at(index)) {
            std::shared_ptr<Text> textPtr = std::make_shared<Text>(t);  // Creating a shared_ptr from the existing Text object
            scenes[sceneNum]->PushStoryText(textPtr);
        }
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

void Game::CreateFPSCounter(SceneEnum scene) {
    auto fps = std::make_shared<Text>("Obj_Fps", "M_Quad", "S_Text", "T_Charmap", "FPS");
    fps->transform.SetPosition({-1.0, 1.0, 0.0f});
    fps->SetColor(Colors::White);
    fps->SetAnchor(Text::Anchor::TOPLEFT);
    fps->SetCallback([this]() -> std::string {
        return "fps: " + std::to_string(app.GetFPS());
    });
    AddTextToScene(scene, fps);

}

void Game::CreateShipHUD(SceneEnum scene) {
    auto speedo = std::make_shared<Text>("Obj_Speedo", "M_Quad", "S_Text", "T_Charmap", "");
    speedo->transform.SetPosition({-1.0, -1.0, 0.0f});
    speedo->SetColor(Colors::Amber);
    speedo->SetAnchor(Text::Anchor::BOTTOMLEFT); 
    speedo->SetCallback([this]() -> std::string {
        auto draw_bar = [](float in, float max, int total_bars) -> std::string {
        int num_bars = ((in/max)*(float)total_bars);
            std::string begin =        "[";
            std::string bars(num_bars, '#');
            std::string end = std::string(total_bars - num_bars, ' ') + ']';
            return begin + bars + end;
        };

        float velocity = glm::length(active_scene->GetPlayer()->velocity);
        float ang_vel = glm::length(active_scene->GetPlayer()->ang_velocity);
        float max_velocity = 190;
        float max_ang_vel = 1.3;
        int total_bars = 25;

        std::string vel_bar = draw_bar(velocity, max_velocity, total_bars);
        std::string rot_bar = draw_bar(ang_vel, max_ang_vel, total_bars);

        std::string out = std::to_string(velocity) + "\n"
                        + vel_bar + "\n"
                        + std::to_string(ang_vel) + "\n"
                        + rot_bar;
        return out;
    });
    // scene->AddNode(speedo);
    AddTextToScene(scene, speedo);

    auto crosshair = std::make_shared<Text>("Obj_Crosshair", "M_Quad", "S_Text", "T_Charmap", "[ ]");
    crosshair->transform.SetPosition({0.0, 0.0, 0.0});
    crosshair->SetSize(10.0f);
    crosshair->SetColor(Colors::Amber);
    crosshair->SetBackgroundColor(Colors::Transparent);
    crosshair->SetAnchor(Text::Anchor::CENTER);
    AddTextToScene(SPACE, crosshair);
}

void Game::CreateMapHUD(SceneEnum scene){ 
    auto fp_map = std::make_shared<Text>("Obj_FPMap", "M_Quad", "S_Text", "T_Charmap", "waiting");
    fp_map->SetAnchor(Text::Anchor::BOTTOMRIGHT);
    fp_map->transform.SetPosition({1.0, -1.0, 0.0});
    fp_map->SetColor(Colors::White);
    fp_map->SetSize(10.0f);
    fp_map->SetCallback([this]() -> std::string {
        auto terr = active_scene->GetTerrain();
        if(terr == nullptr) {
            return "MAP NULL DATA";
        }
        int map_width = 28;
        int map_height = map_width/2 + 1;
        
        int terr_width = terr->GetWidth();
        int terr_height = terr->GetDepth();
        glm::vec3 pt = active_scene->GetPlayer()->transform.GetPosition();

        // int 0 --> MAP_WIDTH_CHARS
        glm::vec3 prel = pt  - terr->transform.GetPosition(); // top left

        int playerx = std::max(std::min((int)((prel.x / terr_width) * map_width), map_width), 1);
        int playery = std::max(std::min((int)((prel.z / terr_height) * map_height), map_height), 1);

        char bg_char = '.';
        char player_char = '@';
        char wall_char = ' ';
        std::string map;
        map.reserve();
        map += std::string(map_width+2, wall_char) + "\n";
        for(int i = 1; i < map_height; i++) {
            if(i != playery) {
                map += wall_char + std::string(map_width, bg_char) + wall_char + "\n";
            }else {
                map += wall_char + std::string(playerx-1, bg_char) + player_char + std::string(map_width - playerx, bg_char) + wall_char + "\n";
            }
        }
        map += std::string(map_width+2, wall_char) + "\n";
        return map;
    });
    AddTextToScene(FOREST, fp_map);
}

void Game::CreateStory() {
    AddStoryToScene(SceneEnum::START, StoryBeat::BEGINNING);
    AddStoryToScene(SceneEnum::SPACE,  StoryBeat::INTRO);
    AddStoryToScene(SceneEnum::FOREST, StoryBeat::FOREST_THOUGHTS);

    AddStoryToScene(SceneEnum::ENDING, StoryBeat::GOOD_END);
    AddStoryToScene(SceneEnum::ENDING, StoryBeat::CREDITS);
}

void Game::CreateLights() {
    auto light = std::make_shared<Light>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    light->ambient_power = 0.1f;
    light->transform.SetPosition({50.5, 100.5, 50.5});
    AddLightToScene(SceneEnum::SPACE, light);
    AddLightToScene(SceneEnum::FPTEST, light);

    // scenes[BEFORETRIGGER]->GetLights().push_back(light);
    // scenes[AFTERTRIGGER]->GetLights().push_back(light);
}


void Game::ChangeScene(int sceneIndex) {
    current_respawn_position = glm::vec3(0.0f);
    std::cout << "changing scenes" << std::endl;
    active_scene = scenes[sceneIndex];
    active_scene_num = SceneEnum(sceneIndex);
    app.SetMouseHandler(std::bind(&Player::MouseControls, active_scene->GetPlayer(), std::placeholders::_1));
    active_scene->SetBackgroundColor(viewport_background_color_g);
}

void Game::ChangeSceneAndSpawn(int sceneIndex, glm::vec3 position) {
    auto old_player = active_scene->GetPlayer();
    if(old_player != nullptr) {
        old_player->transform.SetPosition({0.0, 0.0, 0.0});
    }
    ChangeScene(sceneIndex);
    current_respawn_position = position;
    auto p = active_scene->GetPlayer();
    if(p) {
        p->transform.SetPosition(position);
    }
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

void Game::ShipHitPlanet(glm::vec3 respawn_pos, std::string message) {
    glm::vec3 pos = active_scene->GetPlayer()->transform.GetPosition();
    SpawnExplosion(pos, active_scene->GetPlayer()->transform.GetScale());
    PlayerHitRespawnMessage(respawn_pos, message);
}

void Game::PlayerHitRespawnMessage(glm::vec3 respawn_pos, std::string message) {
    resman.SetScreenSpaceShader("S_Violence"); 
    active_scene->SetCollision(false);
    active_scene->ClearStoryText();
    active_scene->ClearText();
    active_scene->GetPlayer()->deleted = true;
    auto dead = std::make_shared<Text>(message, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), Text::Anchor::CENTER, glm::vec3(0.0, 0.0, 0.0));
    active_scene->AddText(dead);
    // Add delay????
    auto dead_info = std::make_shared<Text>("Press [9] to restart", glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, -2.0f, 0.0f, 0.8f), Text::Anchor::CENTER, glm::vec3(0.0, -0.5, 0.0), 0.2f);
    active_scene->AddText(dead_info);
    current_respawn_position = respawn_pos;
}

void Game::PlayerHitShip(glm::vec3 spawn_pos) {
    ChangeSceneAndSpawn(SPACE,spawn_pos);
}

void Game::SpawnExplosion(glm::vec3 position, glm::vec3 scale) {
    auto explosion = std::make_shared<Explosion>("Obj_Explosion", "M_Explosion", "S_Explosion", "T_Fire");
    explosion->transform.SetPosition(position);
    explosion->transform.SetScale(scale);
    explosion->SetAlphaEnabled(true);
    explosion->SetAlphaFunc(GL_ONE);
    active_scene->AddNode(explosion);
}

void Game::SpawnRocket(glm::vec3 position, glm::quat orientation, glm::vec3 initial_velocity) {
    auto rocket = std::make_shared<Rocket>("Obj_Rocket", "M_Rocket", "S_NormalMap", "T_Rocket", this);
    rocket->SetNormalMap("T_MetalNormalMap");

    Thrust* thrust = new Thrust("Obj_rocketthrust", "M_Thrust", "S_Thrust", "T_Fire");
    thrust->SetAlphaEnabled(true);
    thrust->SetAlphaFunc(GL_ONE);
    rocket->AddThrust(thrust);

    rocket->transform.SetPosition(position);
    rocket->transform.SetOrientation(orientation);
    rocket->SetNodeType(TROCKET);
    rocket->velocity = initial_velocity;

    AddColliderToScene(SPACE, rocket);
}

void Game::CollectStoryItem(StoryBeat l) {
    active_scene->SetCollision(false);
    active_scene->GetPlayer()->SetStatic(true);
    AddStoryToScene(active_scene_num, l);
    reading_item_ = true;
}

void Game::CollectEndingItem(StoryBeat l) {
    ending_sequence_ = true;
    CollectStoryItem(l);
}

void Game::UnlockDash() {
     for (auto s : scenes) {
        s->GetPlayer()->UnlockDash();
     }
}
