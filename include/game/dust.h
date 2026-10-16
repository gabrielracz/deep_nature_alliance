#ifndef DUST_H_
#define DUST_H_

#include "scene_node.h"

// A short lived puff of sand. Oriented so that local +z points where the sand is thrown.
class Dust : public SceneNode {
public:
    Dust(float lifetime, float intensity);

    virtual void Update(double dt) override;

private:
    float lifetime_;
};

#endif // DUST_H_
