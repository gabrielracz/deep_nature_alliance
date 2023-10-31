#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "resource_manager.h"
#include "scene_graph.h"
#include "scene_node.h"
#include "light.h"
#include "defines.h"

class Application;

namespace config {
    const float camera_near_clip_distance = 0.01;
    const float camera_far_clip_distance = 10000.0;
    const float camera_fov = 90.0; // Field-of-view of camera (degrees)
    const glm::vec3 viewport_background_color(0.0, 0.0, 0.0);
    const glm::vec3 camera_position(0.0, 1.0, 15.0);
    const glm::vec3 camera_look_at(0.0, 0.0, 0.0);
    const glm::vec3 camera_up(0.0, 1.0, 0.0);
};

class View {

typedef struct {
    GLFWwindow* ptr;
    std::string title;
    int width;
    int height;
} Window;

private:

    Application& app;

    Window win;
    Camera camera;
    Mouse mouse;
    KeyMap key_controls;

    void InitWindow(const std::string& title, int width, int height);
    void InitView();
    void InitEventHandlers();
    void InitControls();


    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void ResizeCallback(GLFWwindow* window, int width, int height);
    static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

    std::function<void(Mouse& mouse)> mouse_handler;

public:
	View(Application& app);	
    ~View();
    void Init(const std::string& title, int width, int height);
    void Update(double dt);
    void Render(SceneGraph& scene, Camera& camera, std::vector<Light*>& lights);
    void RenderNode(SceneNode* node, Camera& cam, std::vector<Light*>& lights, const glm::mat4& parent_matrix = glm::mat4(1.0f));

    void ToggleMouseCapture();
    void SetMouseHandler(MouseHandler h) {mouse_handler = h;}

    KeyMap& GetKeys() {return key_controls;}
    Mouse& GetMouse() {return mouse;}
    Camera& GetCamera() {return camera;}

    int GetWidth() {return win.width;}
    int GetHeight() {return win.height;}

};

#endif