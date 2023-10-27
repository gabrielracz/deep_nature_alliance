#ifndef VIEW_H
#define VIEW_H

#define GLM_FORCE_RADIANS

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>	//vector print
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <cstring>
#include <unordered_set>
#include <unordered_map>


#include "camera.h"
#include "paths.h"
#include "shader.h"
#include "shapes.h"
#include "defs.h"
#include "mesh.h"
#include "aircraft.h"

#define RAD_TO_DEG 180 / glm::pi<float>()
#define NUM_SHADERS 10
#define CLAMP(f) f / 255.0f

class Application;
class Simulation;

#define HEXCOLOR(h) {(h&0xFF000000)/255.0f, (h&0x00FF0000)/255.0f, (h&0x0000FF00)/255.0f, 1.0f};

namespace Colors {
    const glm::vec4 None        = {0.0f, 0.0f, 0.0f, 0.0f};
    const glm::vec4 Black       = {0.0f, 0.0f, 0.0f, 1.0f};
    const glm::vec4 Grey        = {0.45f, 0.45f, 0.45f, 1.0f};
    const glm::vec4 DGrey        = {0.20f, 0.20f, 0.20f, 1.0f};
    const glm::vec4 LGrey       = {0.7f, 0.7f, 0.7f, 1.0f};
    const glm::vec4 Cream       = {0xFF/255.0f, 0xD7/255.0f, 0xAF/255.0f, 1.0f};
    const glm::vec4 White       = {1.0f, 1.0f, 1.0f, 1.0f};
    const glm::vec4 Magenta     = {1.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 Purple      = {CLAMP(0x67), CLAMP(0x5c), CLAMP(0xff), 1.0f };
    const glm::vec4 LPurple     = {0xA4/255.0f, 0x9B/255.0f, 0xDC/255.0f, 1.0f};
    const glm::vec4 Red         = {1.0f, 0.0f, 0.0f, 1.0f};
    const glm::vec4 Green       = {0.0f, 1.0f, 0.0f, 1.0f};
    const glm::vec4 GGreen      = {0x21/255.0f, 0xff/255.0f, 0x85/255.0f, 1.0f};
    const glm::vec4 CRTGreen    = {144.0f/255.0f,242.0f/255.0f,21.0f/255.0f, 1.0f};
    const glm::vec4 Slime       = {0xAF/255.0f, 0xAF/255.0f, 0.0f, 1.0f};
    const glm::vec4 Gasoline    = {0x87/255.0f, 0xFF/255.0f, 0xAF/255.0f, 1.0f};
    const glm::vec4 Blue        = {0.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 Cyan        = {0.0f, 1.0f, 1.0f, 1.0f};
    const glm::vec4 Stormy      = {41/255.0f, 0x4F/255.0f, 0x4F/255.0f, 1.0f};
    const glm::vec4 Amber       = {0xFF/255.0f, 0xB0/255.0f, 0.0f, 1.0f};
    const glm::vec4 Orange      = {0xFF/255.0f, 0x87/255.0f, 0.0f, 1.0f};
    const glm::vec4 Pred        = {0xff/255.0f, 0x00/255.0f, 0x5d/255.0f, 1.0f};
    const glm::vec4 Brown       = {0x76/255.0f, 0x44/255.0f, 0x07/255.0f, 1.0f};

}

typedef struct VertexArray {
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
}VertexArray;

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

	enum Shaders {
		S_DEFAULT,
        S_DITHER,
        S_DITHER2,
        S_TOON,
		S_GRID,
		S_LIGHT,
		S_TEXT,
        S_LINE,
        S_SKYBOX,
        S_SKYBOX_DITHER,
        S_SKYBOX_TOON,
		S_TERRAIN,
        N_SHADERS
	};

    enum ShadingMode {
        SMOOTHE,
        DITHERED,
        TOON,
        N_MODES
    };

	enum Textures {
		T_CHARMAP = 0,
        T_CRATE,
		T_SKYBOX,
		T_PERLIN,
        N_TEXTURES
	};

    typedef struct ControlKey {
        int key_code;
        bool hold;
        ControlKey(int key, int hld)
        : key_code(key), hold(hld){}
    }ControlKey;

    enum TextPosition {
        TOPLEFT,
        BOTTOMLEFT,
        TOPRIGHT,
        BOTTOMRIGHT,
        CENTER
    };

public:
    Application* app;
    Simulation* sim;
	Window win;
	Camera camera;
	Mouse mouse;

    std::unordered_map<int, bool> key_controls;
    glm::vec3 light_pos;

	Shader shaders[N_SHADERS];
	Texture textures[N_TEXTURES];
    unsigned int text_vao;
	unsigned int skybox_tex;
	unsigned int skybox_vao;
    VertexArray line_vao;
    Mesh meshes[N_ENTITY_TYPES];
    ShadingMode active_shading_mode = ShadingMode::TOON;

    float framedelta = 0.0f;

	View(const std::string& win_title, int win_width, int win_height);
	View();
	int init(Application* app, Simulation* model);
    void load_shaders();
    int init_controls();
    int check_controls();
	int render(double dt);
    void render_aircraft(Aircraft& ent, const glm::vec4& color);
    void render_skybox();
    void render_terrain(const glm::vec4& color);
    void render_hud();
    void render_loading();
	void render_text(const std::string& text, float x, float y, float size, const glm::vec4& color, TextPosition text_position = TextPosition::TOPLEFT);
    void render_line(const glm::vec3& line, const glm::vec4& color = Colors::Red, float scale = 1.0f, const glm::vec3& shift = glm::vec3(0.0f));
    void render_wing_forces(Wing w, glm::mat4 parent_transform, glm::mat4 rotation);

    static bool DRAW_WIREFRAME;
    static bool DRAW_DEBUG;

private:

	static void callback_mouse_move(GLFWwindow* window, double xpos, double ypos);
	static void callback_scroll(GLFWwindow* window, double xpos, double ypos);
	static void callback_mouse_button(GLFWwindow* window, int button, int action, int mods);
	static void callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void callback_resize_framebuffer(GLFWwindow* window, int width, int height);

	static unsigned int init_quad();
    static unsigned int init_cube();
    static VertexArray init_line();
	static Texture load_texture(const std::string& file_path, int wrap_option = GL_REPEAT);
	static Texture load_cubemap(std::vector<std::string>& faces);
};
#endif
