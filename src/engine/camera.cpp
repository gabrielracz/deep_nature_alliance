#include "transform.h"
#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "camera.h"

Camera::Camera(void){
}


Camera::~Camera(){
}


void Camera::Update() {
    transform.Pitch(angular_velocity.x);
    transform.Yaw(angular_velocity.y);
    transform.Roll(angular_velocity.z);
    transform.Translate(velocity);
    SetupViewMatrix();
}

void Camera::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up){

    // Store initial forward and side vectors
    // See slide in "Camera control" for details
    // probably will not be much used (directly setting view a rare occurrence in games)
    transform.up = up;
    transform.forward = -glm::normalize(look_at - position);
    transform.side = glm::normalize(glm::cross(transform.up, transform.forward));

    // Reset orientation and position of camera
    transform.position = position;
    transform.orientation = glm::quat();
}


void Camera::SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h){

    // Set projection based on field-of-view
    float top = tan((fov/2.0)*(glm::pi<float>()/180.0))*near;
    float right = top * w/h;
    projection_matrix_ = glm::frustum(-right, right, -top, top, near, far);
}


void Camera::SetupShader(GLuint program){

    // Set view matrix in shader
    GLint view_mat = glGetUniformLocation(program, "view_mat");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix_));
    
    // Set projection matrix in shader
    GLint projection_mat = glGetUniformLocation(program, "projection_mat");
    glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix_));
}


void Camera::SetupViewMatrix(void){

    //view_matrix_ = glm::lookAt(position, look_at, up);
    if(parent_transform) {
        // glm::vec3 eye = parent_transform->position + (parent_transform->orientation * transform.position);
        // glm::vec3 look_at = (transform.orientation * parent_transform->orientation * transform.axes[FORWARD]) + parent_transform->position;
        // view_matrix_ = glm::lookAt(eye, look_at, up);
        glm::vec3 eye = parent_transform->position + parent_transform->orientation * transform.position;
        glm::vec3 look_at = parent_transform->position;// + glm::vec3(target->rotm * glm::vec4(0.0f, 0.0f, -13.0f, 0.0f));
        glm::vec3 up = (transform.orientation * parent_transform->orientation * transform.axes[UP]);
        view_matrix_ = glm::lookAt(eye, look_at, up);

    } else {
        glm::vec3 eye = transform.position;
        glm::vec3 look_at = transform.LocalAxis(FORWARD);
        glm::vec3 up = transform.LocalAxis(UP);
        view_matrix_ = glm::lookAt(eye, look_at, up);
    }
}

void Camera::Attach(Transform *p) {
    parent_transform = p;
}
