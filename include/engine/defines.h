#ifndef DEFINES_H
#define DEFINES_H
#include <unordered_map>
#include <glm/glm.hpp>
#include <functional>

#define PI glm::pi<float>()
#define PI_2 glm::pi<float>()/2.0f
#define PRINT_FIXED_FLOAT(x) std::fixed <<std::setprecision(2)<<(x)

typedef std::unordered_map<int, bool> KeyMap;

struct Mouse {
    bool first_captured = true;
    bool captured = true;
    glm::vec2 prev;
    glm::vec2 move;
};

typedef std::function<void(Mouse& mouse)> MouseHandler;

#endif