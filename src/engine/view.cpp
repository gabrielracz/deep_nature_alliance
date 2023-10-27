#include "view.h"
#include "application.h"
#include <stdexcept>


View::View(Application& app)
: app(app) {}

View::~View() {
    glfwTerminate();
}

void View::Render(SceneGraph& scene) {
    if(glfwWindowShouldClose(win.ptr)) {
        app.Quit();
        return;
    }

    camera.Update();
    scene.Draw(&camera);

    // Push buffer drawn in the background onto the display
    glfwSwapBuffers(win.ptr);

    // Update other events like input handling
    glfwPollEvents();

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

}

void View::InitView(){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	//Use this to disable vsync
	// glfwSwapInterval(0);

    glViewport(0, 0, win.width, win.height);

    camera.SetView(config::camera_position, config::camera_look_at, config::camera_up);
    camera.SetProjection(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, win.width, win.height);
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
    view->camera.SetProjection(config::camera_fov, config::camera_near_clip_distance, config::camera_far_clip_distance, width, height);
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