#include "view.h"
#include "application.h"
#include "resource_manager.h"
#include <cmath>
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
    
    // DRAW
    const glm::vec4 background_color = {0.0f, 0.0f, 0.0f, 1.0f};
    glClearColor(background_color[0], 
                 background_color[1],
                 background_color[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch(render_mode) {
        case RenderMode::FILL:
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            break;
        case RenderMode::WIREFRAME:
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            break;
    }

    for(auto node : scene) {
        RenderNode(node, scene.GetCamera(), scene.GetLights());
    }

    glfwSwapBuffers(win.ptr);
    glfwPollEvents();
}

void View::RenderNode(SceneNode* node, Camera& cam, std::vector<Light*>& lights, const glm::mat4& parent_matrix) {
    std::string shd_id  = node->GetShaderID();
    std::string tex_id  = node->GetTextureID();
    std::string norm_id = node->GetNormalMap();
    std::string mesh_id = node->GetMeshID();

    // SHADER
    Shader* shd = resman.GetShader(shd_id);
    shd->Use();
    // if(active_shader != shd->id) {
    cam.SetProjectionUniforms(shd, node->GetDesiredProjection());

    if(node->GetInstances().size() > 0) {
        shd->SetInstances(node->GetInstances());
    }
    shd->SetLights(lights);

    node->SetUniforms(shd, cam.GetViewMatrix());

    // TEXTURE
    if(!tex_id.empty()) {
        resman.GetTexture(tex_id)->Bind(shd, 0, "texture_map");
    }
    if(!norm_id.empty()) {
        resman.GetTexture(norm_id)->Bind(shd, 1, "normal_map");
    }

    // MODEL
    if(node->IsAlphaEnabled()) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
    resman.GetMesh(mesh_id)->Draw(node->GetInstances().size());

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

void View::InitWindow(const std::string& title, int width, int height) {
    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw((std::runtime_error(std::string("Could not initialize the GLFW library"))));
    }

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

    GLint maxUBOSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUBOSize);
    std:: cout << "Max UBO size: " << maxUBOSize << std::endl;

    GLint mvoc;
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &mvoc);
    std::cout << mvoc << std::endl;
}

void View::InitView(){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DITHER);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);  

	//Use this to disable vsync
	// glfwSwapInterval(0);

    glViewport(0, 0, win.width, win.height);
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
    view->mouse.first_captured = true;
    view->resize_handler(width, height);
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


void View::ToggleRenderMode() {
    render_mode = (render_mode + 1) % RenderMode::NUM_RENDERMODES;
}