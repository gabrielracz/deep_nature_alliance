#include "transform.h"

void Transform::Update(const glm::mat4& parent) {
    // if(dirty) {
        transf_local          = CalculateMatrix();
        transf_local_no_scale = RemoveScaling(transf_local);
        dirty = false;
    // }
    transf_world          = parent * transf_local;
    transf_world_no_scale = parent * transf_local_no_scale;
    // world_pos = parent * transf_world * glm::vec4(position, 1.0f);

}

glm::vec3 Transform::LocalAxis(Axis a){
    return orientation * axes[a];
};


void Transform::Rotate(const glm::quat& rot) {
    orientation = orientation * rot;
    orientation = glm::normalize(orientation);
    dirty = true;
}

void Transform::RotateOrbit(const glm::quat& rot) {
    orbit = orbit * rot;
    orbit = glm::normalize(orientation);
    dirty = true;
}

void Transform::Translate(const glm::vec3& trans) {
    position += trans;
    dirty = true;
}

void Transform::Pitch(float angle) {
    glm::quat rotation = glm::angleAxis(angle, LocalAxis(SIDE)); // not the correct axis
    Rotate(rotation);
}

void Transform::Yaw(float angle) {
    glm::quat rotation = glm::angleAxis(angle, LocalAxis(UP)); // not the correct axis
    Rotate(rotation);
}

void Transform::Roll(float angle) {
    glm::quat rotation = glm::angleAxis(angle, LocalAxis(FORWARD)); // what axis is used for rolling?
    Rotate(rotation);
}

// glm::mat4 Transform::Matrix() {

//     glm::mat4 rotation = glm::mat4_cast(orientation);
//     glm::mat4 translation = glm::translate(glm::mat4(1.0), position);
//     glm::mat4 joint_translate = glm::translate(glm::mat4(1.0), -joint);
//     glm::mat4 orb= glm::inverse(joint_translate) * glm::mat4_cast(orbit) * joint_translate;

//     glm::mat4 transf = translation * orb* rotation;

//     return transf;
// }

glm::mat4 Transform::CalculateMatrix() {
    glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale);
    glm::mat4 rotation = glm::mat4_cast(orientation);
    glm::mat4 translation = glm::translate(glm::mat4(1.0), position);
    glm::mat4 joint_translate = glm::translate(glm::mat4(1.0), -joint);
    glm::mat4 orb= glm::inverse(joint_translate) * glm::mat4_cast(orbit) * joint_translate;

    glm::mat4 transf = translation * orb * rotation * scaling;

    return transf;
}

glm::mat4 Transform::RemoveScaling(const glm::mat4 m) {
    glm::mat4 n = m;
    n[0] = glm::normalize(n[0]);
    n[1] = glm::normalize(n[1]);
    n[2] = glm::normalize(n[2]);
    return n;
}