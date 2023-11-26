#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "transform.h"
#include "shader.h"

class Camera {

    public:
        enum class Projection {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };

        Camera();
        ~Camera();

        void Update(float dt);

        // Set the view from camera parameters: initial position of camera,
        // point looking at, and up vector
        // Resets the current orientation and position of the camera
        void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);
        // Set projection from frustum parameters: field-of-view,
        // near and far planes, and width and height of viewport
        void SetPerspective(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h);
        void SetOrtho(GLfloat w, GLfloat h);
        void SetProjectionUniforms(Shader* shd, Projection = Projection::PERSPECTIVE);
        void SetScreenSize(float w, float h);

        void Attach(Transform* parent_transform, bool locked = false);
        void Detach();
        void Drop();
        void ToggleLock();
        void Move(const glm::vec3 newpos);
        void Reset();
        bool IsAttached();
        int GetWinWidth() {return saved_screen_width;}
        int GetWinHeight() {return saved_screen_height;}
        glm::mat4 GetPerspectiveMatrix() {return perspective_matrix;}
        void SetupViewMatrix(void);
        const glm::mat4& GetViewMatrix() {return view_matrix_;}


        Transform transform; 
    private:
        Transform* parent_transform {nullptr};
        bool locked = false;
        glm::mat4 view_matrix_; // View matrix
        glm::mat4 perspective_matrix; // Projection matrix
        glm::mat4 ortho_matrix; // Projection matrix
        glm::vec3 original_pos = {0.0, 0.0, 0.0};
        glm::vec3 move_queue = {0.0, 0.0, 0.0};

        // sane camera defaults
        float saved_fov           = 90.0f;
        float saved_near          = 0.01;;
        float saved_far           = 1000.0f;
        float saved_screen_width  = 1920;
        float saved_screen_height = 1080;

        // Create view matrix from current camera parameters

}; // class Camera

#endif // CAMERA_H_
