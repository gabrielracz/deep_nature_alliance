#include <glm/gtc/noise.hpp>

#include "terrain.h"
#include "defines.h"
#include "game.h"


#define GIX(x, z, num_xsteps) ((x) + (z) * num_xsteps)

Terrain::Terrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, float xwidth, float zwidth, float density, Game* game)
    : SceneNode(name, mesh_id, shader_id), xwidth(xwidth), zwidth(zwidth) {
    Layout layout({{FLOAT3, "vertex"},
                   {FLOAT3, "normal"},
                   {FLOAT3, "color"},
                   {FLOAT2, "uv"}});

    // generate uniform grid

    num_xsteps = xwidth * density;
    num_zsteps = zwidth * density;
    float xstep = xwidth / num_xsteps;
    float zstep = zwidth / num_zsteps;

    heights.resize(std::ceil(num_xsteps), std::vector<float>(std::ceil(num_zsteps), 0.0));

    for (int z = 0; z < num_zsteps; z++) {
        for (int x = 0; x < num_xsteps; x++) {
            glm::vec2 sample = glm::vec2(x * xstep, z * zstep) / 100.0f;
            float height = glm::perlin(sample) * 50.0;
            // float height = 0.0;
            // if (x > 150){
            //     height = 100.0;
            // }

            heights[x][z] = height;
            // create this vertex
            glm::vec3 pos = {x * xstep, height, z * zstep};
            glm::vec3 normal = {0.0, 1.0, 0.0};
            glm::vec3 color = {Colors::SeaBlue};
            glm::vec2 uv = {0.0, 0.0};

            APPEND_VEC3(vertices, pos);
            APPEND_VEC3(vertices, normal);
            APPEND_VEC3(vertices, color);
            APPEND_VEC2(vertices, uv);
        }
    }

// triangulate the grid of points
    for (int z = 0; z < num_zsteps - 1; z++) {
        for (int x = 0; x < num_xsteps - 1; x++) {
            indices.push_back(GIX(x, z, num_xsteps));
            indices.push_back(GIX(x, z + 1, num_xsteps));
            indices.push_back(GIX(x + 1, z + 1, num_xsteps));

            indices.push_back(GIX(x, z, num_xsteps));
            indices.push_back(GIX(x + 1, z, num_xsteps));
            indices.push_back(GIX(x + 1, z + 1, num_xsteps));
        }
    }

    game->resman.AddMesh(mesh_id, vertices, indices, layout);
}

float Terrain::SampleHeight(float x, float z) {
    float terrainX = x / (xwidth / (heights.size() - 1)) + (num_xsteps / 2.0);
    float terrainZ = z / (zwidth / (heights[0].size() - 1)) + (num_zsteps / 2.0);

    // Get the integer coordinates of the cell
    int x0 = static_cast<int>(std::floor(terrainX));
    int z0 = static_cast<int>(std::floor(terrainZ));

    // Clamp the coordinates to be within valid range
    x0 = glm::clamp(x0, 0, static_cast<int>(heights.size()) - 2);
    z0 = glm::clamp(z0, 0, static_cast<int>(heights[0].size()) - 2);

    // Get the fractional part of the coordinates
    float sx = terrainX - static_cast<float>(x0);
    float sz = terrainZ - static_cast<float>(z0);

    // Perform bilinear interpolation
    float h00 = heights[x0][z0];
    float h10 = heights[x0 + 1][z0];
    float h01 = heights[x0][z0 + 1];
    float h11 = heights[x0 + 1][z0 + 1];

    float h0 = (1 - sx) * h00 + sx * h10;
    float h1 = (1 - sx) * h01 + sx * h11;

    return (1 - sz) * h0 + sz * h1;
}
