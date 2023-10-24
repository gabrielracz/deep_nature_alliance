#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "transform.h"

    // Abstraction of a camera
class Camera {

    public:
        Camera(void);
        ~Camera();

        void Update(float dt);

        // Get global camera attributes
        glm::vec3 GetPosition(void) const;
        glm::quat GetOrientation(void) const;

        // Set global camera attributes
        void SetPosition(glm::vec3 position);
        void SetOrientation(glm::quat orientation);
        
        // Perform global transformations of camera
        void Translate(glm::vec3 trans);
        void Rotate(glm::quat rot);

        // Get relative attributes of camera
        glm::vec3 GetForward(void) const;
        glm::vec3 GetSide(void) const;
        glm::vec3 GetUp(void) const;

        // Perform relative transformations of camera
        // void Pitch(float angle);
        // void Yaw(float angle);
        // void Roll(float angle);

        // Set the view from camera parameters: initial position of camera,
        // point looking at, and up vector
        // Resets the current orientation and position of the camera
        void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);
        // Set projection from frustum parameters: field-of-view,
        // near and far planes, and width and height of viewport
        void SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h);
        // Set all camera-related variables in shader program
        void SetupShader(GLuint program);

        void Attach(Transform* parent_transform);

        float pitch_speed = 0.0f;
        float yaw_speed = 0.0f;
        float roll_speed = 0.0f;
        glm::vec3 angular_velocity {0.0f, 0.0f, 0.0f};
        glm::vec3 velocity {0.0f, 0.0f, 0.0f};

    private:
        Transform transform; 
        Transform* parent_transform {nullptr};

        glm::mat4 view_matrix_; // View matrix
        glm::mat4 projection_matrix_; // Projection matrix


        // Create view matrix from current camera parameters
        void SetupViewMatrix(void);

}; // class Camera

#endif // CAMERA_H_
