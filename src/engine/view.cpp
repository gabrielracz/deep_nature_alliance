#include "view.h"
#include "application.h"
#include "resource_manager.h"
#include <cmath>
#include <stdexcept>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

View::View(Application& app, ResourceManager& resman)
: app(app), resman(resman), depth_map_fbo(0), depth_map_texture(0) {}

View::~View() {
    glDeleteFramebuffers(1, &depth_map_fbo);
    glDeleteTextures(1, &depth_map_texture);
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


    for (auto light : scene.GetLights()){
        RenderDepthMap(scene, *light);
        break; //just 1 light for now
    }
    // GLubyte* image = new GLubyte[SHADOW_MAP_X * SHADOW_MAP_Y];

    // // Read the texture data into the image array
    // glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    // glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, image);

    // // Write the image data to a file
    // stbi_write_png("depthmap.png", SHADOW_MAP_X, SHADOW_MAP_X, 1, image, SHADOW_MAP_X);
    // Shader *temp = resman.GetShader("S_ShadowMap");
    // Mesh *m = resman.GetMesh("M_Quad");
    


    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    // glClear(GL_COLOR_BUFFER_BIT);
    

    // temp->Use();
    // // temp->SetUniform1i(0, "depthMap");
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    // m->Draw();
    
    for(auto node : scene) {
        RenderNode(node, scene.GetCamera(), scene.GetLights());
    }

    glfwSwapBuffers(win.ptr);
    glfwPollEvents();
}

void View::RenderDepthMap(SceneGraph& scene, const Light& light) {
    glm::mat4 light_view_matrix = glm::lookAt(light.transform.GetPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_projection_matrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, config::camera_near_clip_distance, config::camera_far_clip_distance);
 
    // Set up the depth map framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glViewport(0, 0, SHADOW_MAP_X, SHADOW_MAP_Y);
    glClear(GL_DEPTH_BUFFER_BIT);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "error" << std::endl;
 
    Shader* depth_map_shader = resman.GetShader("S_Depth");
    depth_map_shader->Use();
 
    depth_map_shader->SetUniform4m(light_projection_matrix * light_view_matrix, "lightSpaceMatrix");
 
    // Render the scene from the light's perspective to generate the depth map
    for (auto node : scene) {
        depth_map_shader->SetUniform4m(light_view_matrix, "light_view_matrix");
        depth_map_shader->SetUniform4m(light_projection_matrix, "light_projection_matrix");
        resman.GetMesh(node->GetMeshID())->Draw();
    }
 
    // Reset the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void View::RenderNode(SceneNode* node, Camera& cam, const std::vector<Light*>& lights, const glm::mat4& parent_matrix) {
    std::string shd_id  =  node->GetShaderID();
    std::string tex_id  = node->GetTextureID();
    std::string norm_id = node->GetNormalMap();
    std::string mesh_id = node->GetMeshID();

    // SHADER
    Shader* shd = resman.GetShader(shd_id);
    shd->Use();
    // if(active_shader != shd->id) {
    cam.SetProjectionUniforms(shd, node->GetDesiredProjection());

    shd->SetLights(lights);

    node->SetUniforms(shd, cam.GetViewMatrix());

    // TEXTURE
    if(!tex_id.empty()) {
        resman.GetTexture(tex_id)->Bind(shd, 0, "texture_map");
    }
    if(!norm_id.empty()) {
        resman.GetTexture(norm_id)->Bind(shd, 1, "normal_map");
        //only normal has shadow mapping code for now
        shd->SetUniform1i(3, "depth_map");
        glActiveTexture(GL_TEXTURE0+3);
        glBindTexture(GL_TEXTURE_2D, depth_map_texture);
        glm::mat4 light_view_matrix = glm::lookAt(lights[0]->transform.GetPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::mat4 light_view_matrix = glm::lookAt(lights[0]->transform.GetPosition(), node->transform.GetPosition(), config::camera_up);
        shd->SetUniform4m(light_view_matrix, "light_view_matrix");
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