#include "control.h"

Control::Control(void) {}
Control::Control(GLFWwindow *w) : window_(w) {}

Control::~Control() {}

void Control::Init(GLFWwindow *w) { 
    window_ = w; 
}

void Control::AddKey(int key, std::function<void(float dt)> f, int event){
    keys[key] = KEY(f, event); 
}   

void Control::GetInput(float dt){
    // For all the keys defined in this control map
    for (auto& [key, data] : keys)  {
        // Get current key state
        int e = glfwGetKey(window_, key);

        // Determine the actual state of the key based on its previous state
        if (e == GLFW_RELEASE && (data.action == GLFW_PRESS || data.action == GLFW_REPEAT)) { 
            data.action = GLFW_RELEASE;
        } else if (e == GLFW_PRESS &&  (data.action == GLFW_PRESS || data.action == GLFW_REPEAT)) {
            data.action = GLFW_REPEAT;
        } else if (e == GLFW_PRESS  && (data.action == -1 || data.action == GLFW_RELEASE)) {
            data.action = GLFW_PRESS;
        }

        /*
        To add a control to object example:
        control_.AddKey(
        GLFW_KEY_S,
        [this](float dt){this->Thrust(dt,-1.0f);}, 
        GLFW_REPEAT);
        */

        // If the current state matches that of its event value (when it should be pressed)
        // call the callback
        if (data.event == data.action) {
            data.callback(dt);
        }
    }
}