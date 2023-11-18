#include "defines.h"
#include "glm/ext/matrix_transform.hpp"
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


void Camera::Update(float dt) {
    transform.Translate(move_queue * dt); // allow translating  to dt
    move_queue = {0.0, 0.0, 0.0};

    if(parent_transform) {
        transform.Update(parent_transform->GetWorldMatrix());
    } else {
        transform.Update();
    }
    SetupViewMatrix();
}

void Camera::SetScreenSize(float w, float h) {
    // update our projections
    SetPerspective(fov, near, far, w, h);
    SetOrtho(w, h);
}

void Camera::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up){

    transform.SetPosition(position);
    transform.SetOrientation(glm::inverse(glm::lookAt(position, look_at, up)));
    original_pos = position;
}


void Camera::SetPerspective(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h){
    fov           = fov;
    near          = near;
    far           = far;
    screen_width  = w;
    screen_height = h;

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

    if(parent_transform) {
        glm::mat4 p = parent_transform->GetWorldMatrix();
        glm::vec3 eye = transform.GetWorldPosition();
        glm::vec3 look_at = locked ? p * glm::vec4(0.0, 0.0, -1.0, 1.0)
                                   : p * transform.GetLocalMatrix() * glm::vec4(0.0, 0.0, -1.0, 1.0);

        glm::vec3 side = transform.GetWorldMatrix() * glm::vec4(transform.GetAxis(SIDE), 0.0f);
        glm::vec3 up = glm::cross(side, glm::vec3(p * glm::vec4(0.0, 0.0, -2.0, 0.0)));

        view_matrix_ = glm::lookAt(eye, look_at, up);
    } else {
        glm::vec3 eye = transform.GetPosition();
        glm::vec3 look_at = transform.GetLocalMatrix() * glm::vec4(0.0, 0.0, -1.0, 1.0);
        glm::vec3 up = transform.LocalAxis(UP);
        view_matrix_ = glm::lookAt(eye, look_at, up);
    }
}

bool Camera::IsAttached() {
    return parent_transform != nullptr;
}

void Camera::Attach(Transform *p, bool lock_to_target) {
    if (IsAttached()) {
        Drop();
    }
    locked = lock_to_target;
    parent_transform = p;
    Reset();
}

void Camera::Detach() {
    transform.SetOrientation(transform.GetWorldOrientation());
    parent_transform = nullptr;
    locked = false;
}

void Camera::Reset() {
    transform.SetOrientation(glm::quat(0, 0, 0, 0)); //reset local camera orientation
    transform.SetPosition(original_pos); // reset local lock point
    SetupViewMatrix();
}

void Camera::Drop() {
    transform.SetPosition(transform.GetWorldPosition()); 
    // inverse of the view matrix gives us the camera's transformation matrix
    transform.SetOrientation(glm::inverse(view_matrix_));
    parent_transform = nullptr;
    SetupViewMatrix();
    locked = false;
}

void Camera::Move(const glm::vec3 newpos) {
    move_queue += newpos;
}

void Camera::ToggleLock() {
    locked = !locked;
}