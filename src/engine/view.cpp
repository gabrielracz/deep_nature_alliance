#include "view.h"
#include <cmath>
#include <stdexcept>
#include "application.h"
#include "resource_manager.h"

View::View(Application& app, ResourceManager& resman)
    : app(app), resman(resman) {}

View::~View() {
    glDeleteFramebuffers(1, &depth_map_fbo);
    glDeleteTextures(1, &depth_map_texture);
    glfwTerminate();
}

void View::Render(SceneGraph& scene) {
    // UPDATE
    if (glfwWindowShouldClose(win.ptr)) {
        app.Quit();
        return;
    }

    // DRAW
    const glm::vec4 background_color = {0.0f, 0.0f, 0.0f, 1.0f};
    glClearColor(background_color[0],
                 background_color[1],
                 background_color[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (render_mode) {
        case RenderMode::FILL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case RenderMode::WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
    }

    for (auto light : scene.GetLights()) {
        RenderDepthMap(scene, *light);
        break;  // just 1 light for now
    }

    Shader* temp = resman.GetShader("S_ShadowMap");
    Mesh* m = resman.GetMesh("M_Quad");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    temp->Use();
    temp->SetUniform1i(0, "depthMap");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    m->Draw();
    glEnable(GL_DEPTH_TEST);

    // for(auto node : scene) {
    //     RenderNode(node, scene.GetCamera(), scene.GetLights());
    // }

    glfwSwapBuffers(win.ptr);
    glfwPollEvents();
}

void View::SetupDepthMap() {
    // Set up the depth map framebuffer
    glGenFramebuffers(1, &depth_map_fbo);  
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glGenTextures(1, &depth_map_texture);
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_X, SHADOW_MAP_Y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "error" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  


    // Set up the depth map framebuffer
    // glGenFramebuffers(1, &depth_map_fbo);
    // glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);

    // // Set up target texture for rendering
    // glGenTextures(1, &depth_map_texture);
    // glBindTexture(GL_TEXTURE_2D, depth_map_texture);

    // // Set up an image for the texture
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SHADOW_MAP_X, SHADOW_MAP_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

    // // Set up a depth buffer for rendering
    // glGenRenderbuffers(1, &depth_map_buffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, depth_map_buffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,SHADOW_MAP_X, SHADOW_MAP_Y);

    // // Configure frame buffer (attach rendering buffers)
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depth_map_texture, 0);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_map_fbo);
    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // glDrawBuffers(1, DrawBuffers);

    // // Check if frame buffer was setup successfully
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    //     throw(std::ios_base::failure(std::string("Error setting up frame buffer")));
    // }

    // Reset frame buffer
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void View::RenderDepthMap(SceneGraph& scene, const Light& light) {
    glm::mat4 light_view_matrix = glm::lookAt(light.transform.GetPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_projection_matrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, config::camera_near_clip_distance, config::camera_far_clip_distance);
    // glm::mat4 light_projection_matrix = glm::perspective(glm::radians(20.0f), (float)SHADOW_MAP_X / (float)SHADOW_MAP_Y, 1.0f, 50.0f);
    // Set up the depth map framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glViewport(0, 0, SHADOW_MAP_X, SHADOW_MAP_Y);


    Shader* depth_map_shader = resman.GetShader("S_Depth");
    depth_map_shader->Use();

    // Render the scene from the light's perspective to generate the depth map
    for (auto node : scene) {
        depth_map_shader->SetUniform4m(light_projection_matrix * light_view_matrix, "light_space_matrix");
        resman.GetMesh(node->GetMeshID())->Draw();
    }

    // Reset the framebuffer and viewport
    glViewport(0, 0, win.width, win.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(0);
    glUseProgram(0);
    
}

void View::RenderNode(SceneNode* node, Camera& cam, const std::vector<Light*>& lights, const glm::mat4& parent_matrix) {
    std::string shd_id = node->GetShaderID();
    std::string tex_id = node->GetTextureID();
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
    if (!tex_id.empty()) {
        resman.GetTexture(tex_id)->Bind(shd, 0, "texture_map");
    }
    if (!norm_id.empty()) {
        resman.GetTexture(norm_id)->Bind(shd, 1, "normal_map");
        // only normal has shadow mapping code for now
        shd->SetUniform1i(3, "depth_map");
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, depth_map_texture);
        glm::mat4 light_view_matrix = glm::lookAt(lights[0]->transform.GetPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::mat4 light_view_matrix = glm::lookAt(lights[0]->transform.GetPosition(), node->transform.GetPosition(), config::camera_up);
        shd->SetUniform4m(light_view_matrix, "light_view_matrix");
    }

    // MODEL
    if (node->IsAlphaEnabled()) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
    resman.GetMesh(mesh_id)->Draw();

    // HIERARCHY
    glm::mat4 tm = parent_matrix * Transform::RemoveScaling(node->GetCachedTransformMatrix());  // don't pass scaling to children
    for (auto child : node->GetChildren()) {
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
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char*)glewGetErrorString(err))));
    }

    GLint maxUBOSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUBOSize);
    std::cout << "Max UBO size: " << maxUBOSize << std::endl;

    GLint mvoc;
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &mvoc);
    std::cout << mvoc << std::endl;
}

void View::InitView() {
    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DITHER);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    // Use this to disable vsync
    //  glfwSwapInterval(0);

    SetupDepthMap();

    glViewport(0, 0, win.width, win.height);
}

void View::InitEventHandlers(void) {
    // Set event callbacks
    glfwSetKeyCallback(win.ptr, KeyCallback);
    glfwSetFramebufferSizeCallback(win.ptr, ResizeCallback);
    glfwSetCursorPosCallback(win.ptr, MouseMoveCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(win.ptr, (void*)this);
}

void View::InitControls() {
    mouse.prev = glm::vec2((float)win.width / 2, (float)win.height / 2);
    mouse.captured = true;
    mouse.first_captured = true;

    glfwSetInputMode(win.ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; key++) {
        key_controls.insert({key, false});
    }
}

void View::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    View* view = (View*)ptr;

    if (action == GLFW_PRESS) {
        view->key_controls[key] = true;
    } else if (action == GLFW_RELEASE) {
        view->key_controls[key] = false;
    }
}

void View::ResizeCallback(GLFWwindow* window, int width, int height) {
    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    View* view = (View*)ptr;
    view->win.width = width;
    view->win.height = height;
    view->mouse.first_captured = true;
    view->resize_handler(width, height);
}

void View::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    void* ptr = glfwGetWindowUserPointer(window);
    View* view = (View*)ptr;

    if (!view->mouse.captured) {
        return;
    }

    Mouse& mouse = view->mouse;
    if (mouse.first_captured) {
        mouse.prev = {xpos, ypos};
        mouse.first_captured = false;
    }

    mouse.move = glm::vec2(xpos, ypos) - mouse.prev;
    mouse.prev = {xpos, ypos};

    if (view->mouse_handler) {
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