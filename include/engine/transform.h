#ifndef UTILS_H
#define UTILS_H

#include "glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"
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

class Transform { 
private:
    glm::vec3 position    {glm::vec3(0.0f)};
    glm::quat orientation {glm::quat(0.0, 0.0, 0.0, 0.0)};
    glm::vec3 scale       {glm::vec3(1.0f)};
    glm::quat orbit {};
    glm::vec3 joint = {0.0f, 0.0f, 0.0f};
    glm::mat3 axes {{1.0f, 0.0f, 0.0f},
                   {0.0f, 1.0f, 0.0f},
                   {0.0f, 0.0f, -1.0f}};

    glm::mat4 transf_local_no_scale;
    glm::mat4 transf_local;
    glm::mat4 transf_world;
    glm::mat4 transf_world_no_scale;

    Transform* parent;
    glm::mat4 cached_matrix;
    bool dirty = true;

public:
    Transform() = default;
    Transform(const glm::vec3& p, const glm::quat o, const glm::vec3& s)
        : position(s), orientation(o), scale(s) {Update();}
    Transform(const Transform& o);

    void Update(const glm::mat4& parent = glm::mat4(1.0f));
    glm::vec3 LocalAxis(Axis a);
    void Rotate(const glm::quat& rot);
    void RotateOrbit(const glm::quat& rot);
    void Translate(const glm::vec3& trans);
    void TranslateRelative(const glm::vec3& trans);
    void Pitch(float angle);
    void Yaw(float angle);
    void Roll(float angle);
    glm::mat4 CalculateMatrix();
    // glm::mat4 ScaledMatrix();

    void SetPosition(const glm::vec3& newpos) { position = newpos; dirty = true;} 
    void SetScale(const glm::vec3& newscale) { scale = newscale; dirty = true;} 
    void SetOrientation(const glm::quat& newori) { orientation = newori; dirty = true;} 
    void SetAxis(Axis a, const glm::vec3& v ) {axes[a] = v;}
    void SetOrbit(glm::quat q) {orbit = q;}

    const glm::mat4 GetLocalMatrix() {return CalculateMatrix();}
    const glm::mat4& GetWorldMatrix() const {return transf_world;}
    const glm::mat4& GetWorldMatrixNoScale() const {return transf_world;}
    const glm::vec3& GetPosition() const { return position;}
    glm::vec3 GetWorldPosition() const {return transf_world * glm::vec4(0.0, 0.0, 0.0, 1.0f);}
    const glm::quat& GetOrientation() const { return orientation;}
    glm::quat GetWorldOrientation() const {return glm::normalize(glm::toQuat(transf_world));}
    const glm::quat& GetOrbit() const { return orbit;}
    const glm::vec3& GetJoint() const { return joint;}
    const glm::vec3& GetAxis(Axis a) const { return axes[a];}

    static glm::mat4 RemoveScaling(const glm::mat4 m);

};

#endif