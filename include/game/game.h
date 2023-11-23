#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#include <unordered_map>
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
    const glm::vec3 fp_camera_position(0.0, 4.0, 0.0);
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

    private:
        SceneGraph* active_scene;
        std::vector<SceneGraph*> scenes;

        float wind_speed = 1.5f;
        int camera_mode;

        void SetupFPScene(void);
        void SetupSpaceScene(void);
        void SetupForestScene(void);

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

}; // class Game

#endif // GAME_H_
