#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "resource_manager.h"
#include "scene_graph.h"
#include "scene_node.h"
#include "light.h"
#include "defines.h"

class Application;

namespace config2
{
    const float camera_near_clip_distance = 0.01;
    const float camera_far_clip_distance = 10000.0;
    const float camera_fov = 90.0; // Field-of-view of camera (degrees)
    const glm::vec3 viewport_background_color(0.0, 0.0, 0.0);
    const glm::vec3 camera_position(0.0, 1.0, 15.0);
    const glm::vec3 camera_look_at(0.0, 0.0, -1.0);
    const glm::vec3 camera_up(0.0, 1.0, 0.0);
    const glm::vec3 fp_camera_position(0.0, 2.0, 0.0);
};

class View
{

public:

    View(Application &app, ResourceManager &resman);
    ~View();
    void Init(const std::string &title, int width, int height);
    void Update(double dt);
    void Render(SceneGraph &scene);

    void ToggleMouseCapture();
    void SetMouseHandler(MouseHandler h) { mouse_handler = h; }
    void SetResizeHandler(ResizeHandler h) { game_resize_handler = h; }

    void ToggleRenderMode();

    KeyMap &GetKeys() { return key_controls; }
    Mouse &GetMouse() { return mouse; }

    int GetWidth() { return win.width; }
    int GetHeight() { return win.height; }

    Window GetWindow() { return win; }

private:
    Application &app;
    ResourceManager &resman;

    Window win;
    Mouse mouse;
    KeyMap key_controls;

    int PPWIDTH = 512;
    int PPHEIGHT = 512;
    GLuint postprocess_fbo;
    GLuint postprocess_tex;
    GLuint rbo;

    GLuint depth_fbo;    
    GLuint depth_tex;
    int DEPTHWIDTH = 4096 + 2048;
    int DEPTHHEIGHT = 4096 + 2048;



    int render_mode = RenderMode::FILL;
    int active_shader = -1;

    void InitWindow(const std::string &title, int width, int height);
    void InitView();
    void InitEventHandlers();
    void InitControls();
    void InitFramebuffers();

    void RenderScene(SceneGraph& scene);
    void RenderScreenspace(SceneGraph& scene);
    void RenderPostProcessing(SceneGraph& scene);
    void RenderDepthMap(SceneGraph& scene);
    void RenderNode(SceneNode *node, Camera &cam, std::vector<Light *> &lights, const glm::mat4 &parent_matrix = glm::mat4(1.0f));
    void ResizeBuffers(int width, int height);

    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    static void MouseMoveCallback(GLFWwindow *window, double xpos, double ypos);

    MouseHandler mouse_handler;
    ResizeHandler game_resize_handler;
};

#endif