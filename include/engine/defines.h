#ifndef DEFINES_H
#define DEFINES_H
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
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

#define HEXCOLOR(h) {((h&0xFF0000)>>16)/255.0f, ((h&0x00FF00)>>8)/255.0f, (h&0x0000FF)/255.0f, 1.0f};
#define HEXCOLORALPH(h, a) {((h&0xFF0000)>>16)/255.0f, ((h&0x00FF00)>>8)/255.0f, (h&0x0000FF)/255.0f, a};
namespace Colors {
    const glm::vec4 White      = HEXCOLOR(0xFFFFFF);
    const glm::vec4 Black      = HEXCOLOR(0x000000);
    const glm::vec4 SlimeGreen = HEXCOLOR(0xAFAF00);
}

typedef std::function<void(Mouse& mouse)> MouseHandler;

#endif