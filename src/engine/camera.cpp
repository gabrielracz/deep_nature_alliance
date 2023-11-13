#include "defines.h"
#include "transform.h"
#include <glm/ext/quaternion_trigonometric.hpp>
#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
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
    // transform.Translate(velocity);
    SetupViewMatrix();
    if(parent_transform) {
        transform.Update(parent_transform->GetWorldMatrix());
    } else {
        transform.Update();
    }
}

void Camera::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up){

    // Store initial forward and side vectors
    // See slide in "Camera control" for details
    // probably will not be much used (directly setting view a rare occurrence in games)
    transform.SetAxis(UP, up);
    transform.SetAxis(FORWARD, -glm::normalize(look_at - position));
    transform.SetAxis(SIDE, glm::normalize(glm::cross(up, transform.GetAxis(FORWARD))));

    // Reset orientation and position of camera
    transform.SetPosition(position);
    transform.SetOrientation(glm::quat());
    original_pos = position;
}


void Camera::SetPerspective(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h){
    // Set projection based on field-of-view
    float top = tan((fov/2.0)*(glm::pi<float>()/180.0))*near;
    float right = top * w/h;
    perspective_matrix = glm::frustum(-right, right, -top, top, near, far);
}

void Camera::SetOrtho(GLfloat winwidth, GLfloat winheight){
    float w = (float)winwidth/2.0f;
    float h = (float)winheight/2.0f;
    ortho_matrix = glm::ortho<float>(-w, w, -h, h, -1.0f, 1.0f);
}


void Camera::SetProjectionUniforms(Shader* shd, Projection projtype){
    shd->SetUniform4m(view_matrix_,       "view_mat");

    glm::mat4& projection = projtype == Projection::PERSPECTIVE ? perspective_matrix : ortho_matrix;
    shd->SetUniform4m(projection, "projection_mat");
}


void Camera::SetupViewMatrix(void){

    //view_matrix_ = glm::lookAt(position, look_at, up);
    if(parent_transform) {
        //TODO FIX THIS

        glm::mat4 p = parent_transform->GetWorldMatrix();
        glm::vec3 eye = transform.GetWorldPosition();
        // glm::vec3 eye = p * glm::vec4(transform.GetPosition(), 1.0f);
        glm::vec3 look_at = p * glm::vec4(0.0, 0.0, -2.0, 1.0);
        glm::vec3 up = p * glm::vec4(transform.GetAxis(UP), 0.0f);
        view_matrix_ = glm::lookAt(eye, look_at, up);

    } else {
        glm::vec3 eye = transform.GetPosition();
        glm::vec3 look_at = transform.GetPosition() + -transform.LocalAxis(FORWARD) * 10.0f;
        glm::vec3 up = transform.LocalAxis(UP);
        view_matrix_ = glm::lookAt(eye, look_at, up);
    }
}

bool Camera::IsAttached() {
    return parent_transform != nullptr;
}

void Camera::Attach(Transform *p) {
    transform.SetOrientation(glm::quat(0, 0, 0, 0)); //reset local camera orientation
    transform.SetPosition(original_pos); // reset local lock point
    parent_transform = p;
    SetupViewMatrix();
}

void Camera::Detach() {
    parent_transform = nullptr;
}

void Camera::Reset() {
    transform.SetOrientation(glm::quat(0, 0, 0, 0)); //reset local camera orientation
    transform.SetPosition(original_pos); // reset local lock point
    SetupViewMatrix();
}

void Camera::Drop() {
    glm::vec3 eye = transform.GetWorldPosition();
    transform.SetOrientation(transform.GetWorldOrientation()); // take the parent's orientation as we will no longer get it from the 'inheritance'
    transform.SetPosition(eye); 
    parent_transform = nullptr;
    SetupViewMatrix();
}

void Camera::MoveTo(const glm::vec3 newpos) {
    transform.SetPosition(newpos);
    SetupViewMatrix();
}