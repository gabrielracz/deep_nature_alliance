#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#include <unordered_map>
#include <thread>
#include <chrono>
// #define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "asteroid.h"
#include "player.h"
#include "beacon.h"
#include "enemy.h"
#include "random.h"
#include "light.h"
#include "trigger.h"
#include "collision_manager.h"
#include "defines.h"
#include "fp_player.h"
#include "story_data.h"
#include "beacon.h"
#include "mooneye.h"
#include "mooncloud.h"

class Application;

// Exception type for the game
class GameException: public std::exception
{
    private:
        std::string message_;
    public:
        GameException(std::string message) : message_(message) {};
        virtual const char* what() const throw() { return message_.c_str(); };
        virtual ~GameException() throw() {};
};

enum GameState {
    RUNNING,
    WIN,
    LOSE
};

enum SceneEnum {
    SPACE = 0,
    FOREST,
    DESERT,
    MOON,
    FPTEST,
    MAIN_MENU,
    START,
    ENDING,
    NUM_SCENES,
    ALL
};

namespace config
{
    const float camera_near_clip_distance = 0.01;
    const float camera_far_clip_distance = 10000.0;
    const float camera_fov = 90.0; // Field-of-view of camera (degrees)
    const glm::vec3 camera_position(0.0, 1.0, 15.0);
    const glm::vec3 camera_look_at(0.0, 0.0, -1.0);
    const glm::vec3 camera_up(0.0, 1.0, 0.0);
    const glm::vec3 fp_camera_position(0.0, 10.0, 0.0);
    const glm::vec3 cockpit_cam_pos(0.000000, 0.322739, -1.604628);
    const glm::quat cockpit_cam_ori(0.999998, {0.002136, 0.000000, 0.000000});
};

// Game application
class Game {
    public:

        Game(Application& app, ResourceManager& resman) : app(app), resman(resman) {};

        void Init(void); 
        void SetupResources(void);
        void SetupScenes(void);

        void Update(double dt, KeyMap& keys);
        SceneGraph& ActiveScene() { return *active_scene; }
        //TODO: Run SceneGraph Init()
        void SetActiveScene(SceneEnum sceneNum) { active_scene = scenes[sceneNum]; }

        Application& app;
        ResourceManager& resman;
        int rng_seed = 1804289383;
        RandGenerator rng {rng_seed};

        // Callback Shit
        void ShipHitPlanet(glm::vec3 respawn_pos, std::string message = "Your ship is obliterated. You die instantly...");
        void PlayerHitRespawnMessage(glm::vec3 respawn_pos, std::string message);
        void PlayerHitShip(glm::vec3 spawn_pos);
        void BeaconOneHit(void) { ChangeSceneAndSpawn(FOREST, {-191.718155, 20.999252, -395.274536}); }
        void SpawnRocket(glm::vec3 position, glm::quat orientation, glm::vec3 initial_velocity);
        void SpawnExplosion(glm::vec3 position, glm::vec3 scale);
        void CollectStoryItem(StoryBeat l);
        void CollectEndingItem(StoryBeat l);

        const std::vector<std::vector<float>> readTerrain(const std::string& filePath);

    private:
        SceneGraph* active_scene;
        SceneEnum active_scene_num = MAIN_MENU;
        std::vector<SceneGraph*> scenes;

        float wind_speed = 1.5f;
        int camera_mode;

        glm::vec3 current_respawn_position = glm::vec3(0.0);

        bool ending_sequence_ = false;
        bool reading_item_ = false;

        void SetupFPScene(void);
        void SetupSpaceScene(void);
        void SetupForestScene(void);
        void SetupDesertScene();
        void SetupMainMenuScene();
        void SetupStartScene();
        void SetupCreditsScene();

        void LoadMeshes();
        void LoadShaders();
        void LoadTextures();

        void AddToScene(SceneEnum sceneNum, SceneNode* node);
        void AddColliderToScene(SceneEnum sceneNum, SceneNode* node);
        void AddPlayerToScene(SceneEnum sceneNum, Player* node);
        void AddLightToScene(SceneEnum sceneNum, Light* l);
        void AddTextToScene(SceneEnum sceneNum, Text* l);
        void AddStoryToScene(SceneEnum sceneNum, StoryBeat l);

        void CreatePlayer();
        void CreatePlanets();
        void CreateTerrain();
        void CreateAsteroidField(int num_asteroids);
        void CreateHUD();
        void CreateTree();
        void CreateLights();
        void CreateTriggers();
        void CreateStory();
        // void GrowTree(SceneNode* root, int branches, float height, float width, int level, int max_iterations);
        // void GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width);

        void CheckControls(KeyMap& keys, float dt);
        void ResizeCameras(int width, int height);

        void ChangeScene(int sceneIndex);
        void ChangeSceneAndSpawn(int sceneIndex, glm::vec3 position);
        

}; // class Game

const float forest_trees[][5] = {
{-165.123, 4.5832, -24.7948, 1.97866, 4.42923},
{269.142, -8.49136, 264.797, 1.88782, 0.165652},
{-314.804, -2.64173, 293.899, 2.8874, 0.0991229},
{-283.68, -0.172871, 197.119, 2.32154, 5.13691},
{243.293, 0.916935, 229.858, 1.81249, 3.68667},
{38.1205, 1.22856, -108.033, 2.65043, 5.90481},
{309.901, -4.39703, -287.842, 1.23021, 0.311012},
{345.344, 4.80888, 204.17, 2.637, 3.8002},
{-219.106, 2.53806, -112.817, 1.36943, 0.0795998},
{338.203, 7.76958, 183.786, 2.02163, 4.01392},
{319.277, 1.23973, -307.366, 0.813693, 5.63694},
{-122.115, 19.9992, -392.976, 1.93076, 4.56443},
{-17.8048, 8.56256, -45.4643, 1.38155, 2.92382},
{384.456, -2.37631, -56.0832, 1.15636, 3.44391},
{-251.045, 1.69616, -112.378, 1.96221, 0.909891},
{305.841, -3.48328, 272.948, 2.51111, 1.96849},
{34.0854, -2.40623, 73.76, 0.76237, 5.41011},
{73.4589, -3.43998, 223.196, 1.9422, 3.97576},
{271.851, -7.56513, 195.884, 2.42231, 1.81023},
{-224.439, -6.3548, -148.049, 0.928315, 2.98918},
{251.047, 15.554, 147.69, 1.2206, 2.94809},
{323.554, 2.10469, 372.827, 0.813414, 0.115239},
{-196.483, 1.30375, 111.623, 1.90746, 0.569706},
{333.212, 8.68412, 292.445, 1.52456, 5.59743},
{135.422, -8.37599, 30.1374, 2.12212, 1.10686},
{216.615, 3.03255, -155.234, 1.06693, 2.21497},
{278.417, 13.6705, 157.32, 1.97516, 1.33694},
{-241.196, 3.47303, -183.729, 1.97397, 1.20099},
{34.038, -3.51428, 76.2843, 0.84218, 2.58338},
{-140.384, -14.7893, -227.227, 2.3633, 3.51855},
{272.661, -9.26884, 209.314, 2.83692, 3.94897},
{-275.087, -5.56248, -192.957, 2.30402, 0.42278},
{320.147, -5.65054, -89.9496, 2.42566, 5.13818},
{-233.08, -18.1334, 221.973, 2.71248, 0.397978},
{46.4503, -1.59827, -109.852, 2.11146, 3.28319},
{-193.371, 19.9993, -378.02, 2.40952, 2.05259},
{3.96976, 7.24635, 319.895, 0.734341, 2.67993},
{61.9091, 13.2646, -294.748, 1.05794, 5.7851},
{129.225, -11.2846, -116.967, 0.900273, 6.01636},
{-187.358, 4.53836, -120.422, 1.97169, 5.51097},
{-83.5353, -2.28206, 21.5108, 1.54034, 6.10285},
{103.689, 1.24938, 309.079, 2.65267, 6.15489},
{254.967, 19.9993, -365.945, 1.63387, 4.98},
{-337.807, -3.78581, 75.5466, 2.10126, 6.03109},
{139.574, -4.07207, 225.404, 2.89974, 6.23218},
{47.8632, 19.987, -364.04, 1.66321, 0.00425159},
{-101.865, -8.60215, -36.9743, 2.2925, 3.75107},
{234.85, -4.05432, -71.3956, 1.91701, 1.60287},
{41.4737, -4.27716, 248.268, 1.26462, 2.24569},
{-379.461, -7.49632, -4.48489, 1.1951, 0.0272664},
};
#endif // GAME_H_
