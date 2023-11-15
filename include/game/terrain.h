#ifndef TERRAIN_H
#define TERRAIN_H
#include "scene_node.h"
class Game;

class Terrain: public SceneNode {
public:
    Terrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, float xwidth, float zwidth, float density, Game* game);

    static std::vector<float> GenerateHeightmap(float xwidth, float zwidth, float density);
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

#endif