#include "asteroid.h"

Asteroid::Asteroid(const std::string name, const Resource *geometry, Shader* shd) : SceneNode(name, geometry, shd) {
}


Asteroid::~Asteroid(){
}


glm::quat Asteroid::GetAngM(void) const {

    return angm_;
}


void Asteroid::SetAngM(glm::quat angm){

    angm_ = angm;
}


void Asteroid::Update(void){

    Rotate(angm_);
}
            