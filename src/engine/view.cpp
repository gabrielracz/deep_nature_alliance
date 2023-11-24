#include "view.h"
#include "application.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "resource_manager.h"
#include "scene_graph.h"
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

    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    RenderDepthMap(scene);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocess_fbo);
    RenderScene(scene);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    RenderPostProcessing(scene);
    RenderScreenspace(scene);

    glfwSwapBuffers(win.ptr);
    glfwPollEvents();
}

void View::RenderScene(SceneGraph& scene) {
    glViewport(0,0, PPWIDTH, PPHEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto node : scene) {
        RenderNode(node, scene.GetCamera(), scene.GetLights());
    }

    // always render skybox last
    if(scene.GetSkybox()) {
        glDepthFunc(GL_LEQUAL);
        RenderNode(scene.GetSkybox(), scene.GetCamera(), scene.GetLights());
        glDepthFunc(GL_LESS);
    }
}

void View::RenderScreenspace(SceneGraph& scene) {
    glDisable(GL_DEPTH_TEST);
    glViewport(0,0,win.width,win.height);
    for(auto node : scene.GetScreenSpaceNodes()) {
        RenderNode(node, scene.GetCamera(), scene.GetLights());
    }
}

void View::RenderPostProcessing(SceneGraph& scene) {
    glViewport(0,0,win.width,win.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bool postprocess = true;
    if(postprocess) {
        Mesh* scrquad  = resman.GetMesh("M_Quad");
        Shader* scrshd = resman.GetShader("S_Texture");
        scrshd->Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postprocess_tex);

        scrquad->Draw();
    } else {
        Mesh* scrquad  = resman.GetMesh("M_Quad");
        Shader* scrshd = resman.GetShader("S_ShowDepth");
        scrshd->Use();

        scrshd->SetUniform1i(0, "depth_map");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_tex);
        scrquad->Draw();
    }
}

void View::RenderDepthMap(SceneGraph& scene) {
    glViewport(0, 0, DEPTHWIDTH, DEPTHHEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Shader* shd = resman.GetShader("S_Depth");

    // glm::mat4 view_mat = scene.GetCamera().GetViewMatrix();
    // Light* l = scene.GetLights().back();
    glm::mat4 view_mat = glm::lookAt({300.0, 800.0, 0.0}, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 proj_mat = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 20.0f, 1300.0f);

    // glm::mat4 view_mat = scene.GetCamera().GetViewMatrix();
    // glm::mat4 proj_mat = scene.GetCamera().GetPerspectiveMatrix();

    // glm::mat4 proj_mat = glm::perspective(45.0f, 16.0f/9.0f, 0.1f, 400.0f);
    // glm::mat4 proj_mat = glm::perspective(90.0f, 16.0f/9.0f, 0.1f, 100.0f);

    for(auto node : scene) {
        Mesh* mesh = resman.GetMesh(node->GetMeshID());
        shd->Use();
        // set world_mat
        shd->SetUniform4m(node->transform.GetLocalMatrix(), "world_mat");
        // set light_mat
        shd->SetUniform4m(proj_mat * view_mat, "light_mat");
        mesh->Draw();
    }
}

void View::RenderNode(SceneNode* node, Camera& cam, std::vector<Light*>& lights, const glm::mat4& parent_matrix) {
    std::string shd_id  = node->GetShaderID();
    std::string tex_id  = node->GetTextureID();
    std::string norm_id = node->GetNormalMap();
    std::string mesh_id = node->GetMeshID();

    glDisable(GL_BLEND);
    // SHADER
    Shader* shd = resman.GetShader(shd_id);

    shd->Use();
    // if(active_shader != shd->id) {
    cam.SetProjectionUniforms(shd, node->GetDesiredProjection());

    shd->SetLights(lights);
    // Light* l = lights.back();
    glm::mat4 view_mat = glm::lookAt({300.0, 800.0, 0.0}, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 proj_mat = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 20.0f, 1300.0f);


    shd->SetUniform4m(proj_mat * view_mat, "shadow_light_mat");
    node->SetUniforms(shd, cam.GetViewMatrix());

    // TEXTURE
    if(!tex_id.empty()) {
        resman.GetTexture(tex_id)->Bind(shd, 0, "texture_map");
    }
    if(!norm_id.empty()) {
        resman.GetTexture(norm_id)->Bind(shd, 1, "normal_map");
    }
    if(shd_id == "S_NormalMap") {
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, depth_tex);
        shd->SetUniform1i(2, "shadow_map");
    }

    // MODEL
    if(node->IsAlphaEnabled()) {
        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        // glDepthMask(false);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    };
    
    // check if there is anything to render
    if(!mesh_id.empty()) {
        resman.GetMesh(mesh_id)->Draw(node->GetNumInstances());
    }

    // HIERARCHY
    glm::mat4 tm = parent_matrix * Transform::RemoveScaling(node->GetCachedTransformMatrix());  // don't pass scaling to children
    for(auto child : node->GetChildren()) {
        RenderNode(child, cam, lights, tm);
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void View::InitFramebuffers() {
    // Create framebuffer to render a normal scene to a texture output.
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
    glGenFramebuffers(1, &postprocess_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocess_fbo);

    glGenTextures(1, &postprocess_tex);

    glBindTexture(GL_TEXTURE_2D, postprocess_tex);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, PPWIDTH, PPHEIGHT, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, PPWIDTH, PPHEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, postprocess_tex, 0);
    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    GLenum DrawBuffers[1] = {GL_DEPTH_ATTACHMENT};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer init errors" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &depth_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    // Depth framebuffer
    glGenTextures(1, &depth_tex);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                DEPTHWIDTH, DEPTHWIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

void View::Init(const std::string& title, int width, int height) {
    InitWindow(title, width, height);
    InitView();
    InitEventHandlers();
    InitControls();
    InitFramebuffers();
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
	glfwSwapInterval(0);

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