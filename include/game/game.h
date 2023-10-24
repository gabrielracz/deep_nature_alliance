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

#define PI glm::pi<float>()
#define PI_2 glm::pi<float>()/2.0f

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

typedef struct MouseType{
    bool first_captured = true;
    bool captured = true;
    double xprev, yprev;
} Mouse;

typedef struct {
    GLFWwindow* ptr;
    std::string title;
    int width;
    int height;
} Window;

enum GameState {
    RUNNING,
    WIN,
    LOSE
};

// Game application
class Game {
    public:

        RandGenerator rng {rng_seed};
        float wind_speed = 1.5f;

        // Constructor and destructor
        Game(void);
        ~Game();
        // Call Init() before calling any other method
        void Init(void); 
        // Set up resources for the game
        void SetupResources(void);
        // Set up initial scene
        void SetupScene(void);
        // Run the game: keep the application active
        void MainLoop(void); 

    private:
        // GLFW window
        Window win;

        // Scene graph containing all nodes to render
        SceneGraph scene_;

        // Resources available to the game
        ResourceManager resman_;

        // Camera abstraction
        Camera camera_;

        Mouse mouse;

        Player* player;

        // int rng_seed = rand();
        int rng_seed = 1804289383;


        std::vector<SceneNode*> beacons;
        int active_beacon_index = 0;

        std::vector<SceneNode*> powerups;

        std::vector<Enemy*> enemies;


        bool game_state = RUNNING;

        // Flag to turn animation on/off
        bool animating_;

        std::unordered_map<int, bool> key_controls;

        // Methods to initialize the game
        void InitWindow(void);
        void InitView(void);
        void InitEventHandlers(void);
        void InitControls();
        void CheckControls();
        void CheckCollisions();

        // Methods to handle events
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void ResizeCallback(GLFWwindow* window, int width, int height);
        static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

        // Asteroid field
        // Create instance of one asteroid
        Asteroid *CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name);
        // Create entire random asteroid field
        void CreatePlayer();
        void CreateTree();
        void GrowTree(SceneNode* root, int branches, float height, float width, int level, int max_iterations);
        void GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width);
        void CreateAsteroidField(int num_asteroids = 1500);
        void CreateRaceTrack();
        void CreateEnemies();
        void CreatePowerups();


}; // class Game

#endif // GAME_H_
