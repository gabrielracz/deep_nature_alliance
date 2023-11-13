#include "view.h"
#include "application.h"
#include "resource_manager.h"
#include <stdexcept>

View::View(Application& app, ResourceManager& resman)
: app(app), resman(resman) {}

View::~View() {
    glfwTerminate();
}

void View::Render(SceneGraph& scene) {
    // UPDATE
    if(glfwWindowShouldClose(win.ptr)) {
        app.Quit();
        return;
    }

    // temporary. use the game-owned camera eventually
    camera.Update();
    
    // DRAW
    const glm::vec4 background_color = {0.0f, 0.0f, 0.0f, 1.0f};
    glClearColor(background_color[0], 
                 background_color[1],
                 background_color[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto node : scene) {
        RenderNode(node, scene.GetCamera(), scene.GetLights());
    }

    glfwSwapBuffers(win.ptr);
    glfwPollEvents();
}

void View::RenderNode(SceneNode* node, Camera& cam, std::vector<Light*>& lights, const glm::mat4& parent_matrix) {
    std::string shd_id  = node->GetShaderID();
    std::string tex_id  = node->GetTextureID();
    std::string mesh_id = node->GetMeshID();

    // SHADER
    Shader* shd = resman.GetShader(shd_id);
    shd->Use();
    camera.SetProjectionUniforms(shd, node->GetDesiredProjection());
    for(auto l : lights) {
        l->SetUniforms(shd);
    }
    node->SetUniforms(shd, parent_matrix);

    // TEXTURE
    if(!tex_id.empty()) {
        resman.GetTexture(tex_id)->Bind(shd);
    }

    // MODEL
    if(node->IsAlphaEnabled()) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
    resman.GetMesh(mesh_id)->Draw();

    // HIERARCHY
    glm::mat4 tm = parent_matrix * Transform::RemoveScaling(node->GetCachedTransformMatrix());  // don't pass scaling to children
    for(auto child : node->GetChildren()) {
        RenderNode(child, cam, lights, tm);
    }
}

void View::Init(const std::string& title, int width, int height) {
    InitWindow(title, width, height);
    InitView();
    InitEventHandlers();
    InitControls();
}

void error_callback(int error, const char* description)
{
    std::ofstream file("out.txt");
    file << description << std::endl << glGetError() << std::endl;
    fprintf(stderr, "Error: %s\n", description);
}


void View::InitWindow(const std::string& title, int width, int height) {
    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw((std::runtime_error(std::string("Could not initialize the GLFW library"))));
    }

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create a window and its OpenGL context
    win.width = width;
    win.height = height;
    win.title = title;

    // win.ptr = glfwCreateWindow(win.width, win.height, win.title.c_str(), glfwGetPrimaryMonitor(), NULL);
    win.ptr = glfwCreateWindow(win.width, win.height, win.title.c_str(), NULL, NULL);

    if (!win.ptr) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's context the current 
    glfwMakeContextCurrent(win.ptr);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }

}

void View::InitView(){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	//Use this to disable vsync
	glfwSwapInterval(0);

    glViewport(0, 0, win.width, win.height);

    camera.SetView(config::camera_position, config::camera_look_at, config::camera_up);
    camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, win.width, win.height);
    camera.SetOrtho(win.width, win.height);
}

void View::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(win.ptr, KeyCallback);
    glfwSetFramebufferSizeCallback(win.ptr, ResizeCallback);
    glfwSetCursorPosCallback(win.ptr, MouseMoveCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(win.ptr, (void *) this);
}

void View::InitControls() {
	mouse.prev = glm::vec2((float) win.width / 2, (float) win.height / 2);
    mouse.captured = true;
    mouse.first_captured = true;

	glfwSetInputMode(win.ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    for(int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; key++) {
        key_controls.insert({key, false});
    }
}

void View::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    View *view = (View *) ptr;

	if(action == GLFW_PRESS) {
		view->key_controls[key] = true;
	}else if(action == GLFW_RELEASE) {
		view->key_controls[key] = false;
	}
}

void View::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    View *view = (View *) ptr;
    view->win.width = width;
    view->win.height = height;
    view->camera.SetPerspective(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, width, height);
    view->camera.SetOrtho(width, height);
    view->mouse.first_captured = true;
}

void View::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    void* ptr = glfwGetWindowUserPointer(window);
    View *view = (View *) ptr;

    if(!view->mouse.captured) {
        return;
    }

	Mouse& mouse = view->mouse;
	if (mouse.first_captured) {
        mouse.prev = {xpos, ypos};
		mouse.first_captured = false;
	}

    mouse.move = glm::vec2(xpos, ypos) - mouse.prev;
    mouse.prev = {xpos, ypos};

    if(view->mouse_handler) {
        view->mouse_handler(mouse);
    }
}


void View::ToggleMouseCapture() {
    mouse.captured = !mouse.captured;
    mouse.first_captured = true;
    glfwSetInputMode(win.ptr, GLFW_CURSOR, mouse.captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}