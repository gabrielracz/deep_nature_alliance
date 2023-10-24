#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <memory>
#include "scene_graph.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Application;
class View {

typedef struct {
    GLFWwindow* ptr;
    std::string title;
    int width;
    int height;
} Window;

typedef struct MouseType{
    bool first_captured = true;
    bool captured = true;
    double xprev, yprev;
} Mouse;

private:
    Application& app;
    Window win;
    Camera camera;
    Mouse mouse;

    void InitWindow();
    void InitView();
    void InitEventHandlers();
    void InitControls();

    void Render(SceneGraph& scene);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void ResizeCallback(GLFWwindow* window, int width, int height);
    static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

public:
	View(const std::string& title, unsigned int width, unsigned int height, Application* app);	
    void Init();
    void Update(double dt);
};

#endif