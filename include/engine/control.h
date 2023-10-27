#ifndef CONTROL_H_
#define CONTROL_H_

#include <GLFW/glfw3.h>
#include <functional>
#include <map>
#include <iostream>

namespace game
{

    class Control
    {
    public:
        Control(void);
        explicit Control(GLFWwindow *w);
        ~Control();

        //Must init control with window so input can be polled for
        void Init(GLFWwindow *w);
        //Add a key to the control map
        void AddKey(int key, std::function<void(float dt)> f, int event);
        //Get input on all keys
        void GetInput(float dt);

    private:
        // A key is just abstracted as:
        // A current action (what it's current state is)
        // A trigger event (when it should run its callback)
        // A function to run on trigger (a lambda function)
        // Note: delta time is taken as a parameter because some inputs need delta time to be sent as a parameter so
        // it is passed along with the input
        struct KEY
        {
            KEY()=default;
            KEY(std::function<void(float dt)> f, int e) : callback(f), action(GLFW_RELEASE), event(e) {}

            std::function<void(float dt)> callback;
            int action = -1;
            int event = -1;
        };

        GLFWwindow *window_;

        std::map<int, KEY> keys;
    };
}
#endif