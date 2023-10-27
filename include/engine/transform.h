#ifndef UTILS_H
#define UTILS_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform.hpp>

typedef enum {
    SIDE = 0,
    UP = 1,
    FORWARD = 2
} Axis;
struct Transform {
    glm::vec3 position    {glm::vec3()};
    glm::quat orientation {};//{0, 0, 0, 0};
    glm::vec3 scale       {glm::vec3()};
    glm::quat orbit {};
    glm::vec3 joint = {0, 0, 0};

    glm::mat3 axes {{1.0f, 0.0f, 0.0f},
                   {0.0f, 1.0f, 0.0f},
                   {0.0f, 0.0f, 1.0f}};

    glm::vec3& side    = axes[Axis::SIDE];
    glm::vec3& forward = axes[Axis::FORWARD];
    glm::vec3& up      = axes[Axis::UP];

private:
    glm::mat4 cached_matrix;
    bool changed = false;

public:
    Transform() = default;
    Transform(const glm::vec3& p, const glm::quat o, const glm::vec3& s)
        : position(s), orientation(o), scale(s) {Matrix();}

    glm::vec3 LocalAxis(Axis a) const {
        return orientation * axes[a];
    };

    void SetPosition(const glm::vec3& newpos) {
        position = newpos;
    } 

    void Rotate(const glm::quat& rot) {
        orientation = rot * orientation;
        orientation = glm::normalize(orientation);
    }

    void Translate(const glm::vec3& trans) {
        position += orientation * trans;
    }

    void Pitch(float angle) {
        glm::quat rotation = glm::angleAxis(angle, LocalAxis(SIDE)); // not the correct axis
        Rotate(rotation);
    }

    void Yaw(float angle) {
        glm::quat rotation = glm::angleAxis(angle, LocalAxis(UP)); // not the correct axis
        Rotate(rotation);
    }

    void Roll(float angle) {
        glm::quat rotation = glm::angleAxis(angle, LocalAxis(FORWARD)); // what axis is used for rolling?
        Rotate(rotation);
    }

    glm::mat4 Matrix() {

        glm::mat4 rotation = glm::mat4_cast(orientation);
        glm::mat4 translation = glm::translate(glm::mat4(1.0), position);
        glm::mat4 joint_translate = glm::translate(glm::mat4(1.0), -joint);
        glm::mat4 orb= glm::inverse(joint_translate) * glm::mat4_cast(orbit) * joint_translate;

        glm::mat4 transf = translation * orb* rotation;

        return transf;
    }

    glm::mat4 ScaledMatrix() {
        glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale);
        glm::mat4 rotation = glm::mat4_cast(orientation);
        glm::mat4 translation = glm::translate(glm::mat4(1.0), position);
        glm::mat4 joint_translate = glm::translate(glm::mat4(1.0), -joint);
        glm::mat4 orb= glm::inverse(joint_translate) * glm::mat4_cast(orbit) * joint_translate;

        glm::mat4 transf = translation * orb * rotation * scaling;

        return transf;
    }

};

#endif