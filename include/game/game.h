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
#include "defines.h"

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

// Game application
class Game {
    public:

        Game(Application& app, ResourceManager& resman) : app(app), resman(resman) {};

        void Init(void); 
        void SetupResources(void);
        void SetupScene(void);

        void Update(double dt, KeyMap& keys);

        SceneGraph& ActiveScene() {return scene;}
        Camera& ActiveCamera() {return camera;}
        std::vector<Light*>& ActiveLights() {return lights;}

        Application& app;
        ResourceManager& resman;
        int rng_seed = 1804289383;
        RandGenerator rng {rng_seed};

    private:
        SceneGraph scene;
        Camera camera;

        std::vector<Light*> lights;
        Player* player;

        float wind_speed = 1.5f;
        int camera_mode;

        void LoadMeshes();
        void LoadShaders();
        void LoadTextures();

        void CreatePlayer();
        void CreatePlanets();
        void CreateAsteroidField(int num_asteroids);
        void CreateHUD();
        void CreateTree();
        void CreateLights();
        void GrowTree(SceneNode* root, int branches, float height, float width, int level, int max_iterations);
        void GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width);

        void CheckControls(KeyMap& keys);
        void MouseControls(Mouse& mouse);
        void CheckCollisions();

}; // class Game

#endif // GAME_H_
