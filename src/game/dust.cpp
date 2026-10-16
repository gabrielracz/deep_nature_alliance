#include "dust.h"

Dust::Dust(float lifetime, float intensity)
    : SceneNode("Obj_Dust", "M_Dust", "S_Dust", ""), lifetime_(lifetime) {
    // the dust shader reads these as lifetime and intensity
    material.specular_power = lifetime;
    material.diffuse_strength = intensity;
    SetAlphaEnabled(true);
    alpha_depth_write = false;
    cast_shadow = false;
}

void Dust::Update(double dt) {
    if (elapsed > lifetime_) {
        deleted = true;
        return;
    }
    SceneNode::Update(dt);
}
