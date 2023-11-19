#ifndef MOON_TERRAIN_H
#define MOON_TERRAIN_H

#include "terrain.h"
class MoonTerrain : public Terrain {
public:
    MoonTerrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, float xwidth, float zwidth, float density, Game* game)
    : Terrain(name, mesh_id, shader_id, texture_id, xwidth, zwidth, density, game) {}
};

#endif