#ifndef UTILS_H
#define UTILS_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

    Transform() = default;
    Transform(const glm::vec3& p, const glm::quat o, const glm::vec3& s)
        : position(s), orientation(o), scale(s) {}

    glm::vec3 LocalAxis(Axis a) const {
        return orientation * axes[a];
    };

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
        glm::mat4 t;
        glm::scale(t, scale);
        t *= glm::mat4_cast(orientation);
        glm::translate(t, position);
        return t;
    }
};

#endif