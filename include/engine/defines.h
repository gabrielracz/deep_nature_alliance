#ifndef DEFINES_H
#define DEFINES_H
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <functional>
#include <chrono>
#include <random> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define PI glm::pi<float>()
#define PI_2 glm::pi<float>()/2.0f
#define PRINT_FIXED_FLOAT(x) std::fixed <<std::setprecision(2)<<(x)

#define APPEND_VEC2(vector, glm_vec2) vector.insert(vector.end(), {glm_vec2.x, glm_vec2.y})
#define APPEND_VEC3(vector, glm_vec3) vector.insert(vector.end(), {glm_vec3.x, glm_vec3.y, glm_vec3.z})

const unsigned int window_width_g = 1600;
const unsigned int window_height_g = 900;
const bool window_full_screen_g = false;

typedef std::unordered_map<int, bool> KeyMap;

struct Mouse {
    bool first_captured = true;
    bool captured = true;
    glm::vec2 prev;
    glm::vec2 move;
};

typedef struct
{
    GLFWwindow *ptr;
    std::string title;
    int width;
    int height;
} Window;

#define HEXCOLOR(h) {((h&0xFF0000)>>16)/255.0f, ((h&0x00FF00)>>8)/255.0f, (h&0x0000FF)/255.0f, 1.0f}
#define HEXCOLORALPH(h, a) {((h&0xFF0000)>>16)/255.0f, ((h&0x00FF00)>>8)/255.0f, (h&0x0000FF)/255.0f, a}
namespace Colors {
    const glm::vec4 White       = HEXCOLOR(0xFFFFFF);
    const glm::vec4 WarmWhite   = HEXCOLOR(0xFFEA90);
    const glm::vec4 SunLight    = HEXCOLOR(0xDAF0FF);
    const glm::vec4 Black       = HEXCOLOR(0x000000);
    const glm::vec4 SlimeGreen  = HEXCOLOR(0xAFAF00);
    const glm::vec4 Magenta     = HEXCOLOR(0xFF00FF);
    const glm::vec4 Transparent = HEXCOLORALPH(0x000000, 0.0f);
    const glm::vec4 Yellow      = HEXCOLOR(0xF8AA01);
    const glm::vec4 BrightYellow= HEXCOLOR(0xfcbb2d);
    const glm::vec4 Goldish     = HEXCOLOR(0xdec183);
    const glm::vec4 SeaBlue     = HEXCOLOR(0x83A598);
    const glm::vec4 Red         = HEXCOLOR(0xFF0010);
    const glm::vec4 PurpleLight = HEXCOLOR(0xD8B4FF);
    const glm::vec4 Amber       = HEXCOLOR(0xFFAF00);
    const glm::vec4 TranspBlack = HEXCOLORALPH(0x000000, 0.5f);
    const glm::vec4 DarkTranspBlack = HEXCOLORALPH(0x000000, 0.9f);
    const glm::vec4 TBrown      = HEXCOLORALPH(0xA2734C, 0.9f);
}

typedef std::function<void(Mouse& mouse)> MouseHandler;
typedef std::function<void(int width, int height)> ResizeHandler;


template <typename K, typename V>
void overwrite_emplace(std::unordered_map<K, V>& map, K key, V&& val) {
    auto it = map.find(key);
    if(it != map.end()) {
        map.erase(key);
    }
    map.emplace(key, val);
}

enum RenderMode
{
    FILL = 0,
    WIREFRAME,
    NUM_RENDERMODES
};

template <typename T>
std::vector<T> concatenate(std::vector<T>& A, std::vector<T>& B) {
    std::vector<T> AB;
    AB.reserve( A.size() + B.size() ); // preallocate memory
    AB.insert( AB.end(), A.begin(), A.end() );
    AB.insert( AB.end(), B.begin(), B.end() );
    return AB;
}


#endif