#include <glm/gtc/noise.hpp>

#include "terrain.h"
#include "defines.h"
#include "game.h"

// const float terrain[10][10] = {
//     {40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000},
//     {40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000},
//     {40.000000, 40.000000, 40.000000, 39.215686, 38.431373, 38.745098, 40.000000, 40.000000, 40.000000, 40.000000},
//     {40.000000, 40.000000, 39.215686, 33.568627, 23.372549, 27.450980, 36.549020, 40.000000, 40.000000, 40.000000},
//     {40.000000, 40.000000, 37.960784, 21.647059, 3.137255, 9.098039, 29.333333, 39.529412, 40.000000, 40.000000},
//     {40.000000, 40.000000, 37.960784, 21.647059, 3.137255, 9.098039, 29.333333, 39.529412, 40.000000, 40.000000},
//     {40.000000, 40.000000, 39.215686, 33.568627, 23.372549, 27.450980, 36.549020, 40.000000, 40.000000, 40.000000},
//     {40.000000, 40.000000, 40.000000, 39.215686, 38.431373, 38.745098, 40.000000, 40.000000, 40.000000, 40.000000},
//     {40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000},
//     {40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000, 40.000000},
// };



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
        float sampleX = x * xstep;
        float sampleZ = z * zstep;

        int x0 = static_cast<int>(std::floor(sampleX));
        int z0 = static_cast<int>(std::floor(sampleZ));

        // Clamp the coordinates to be within valid range
        x0 = glm::clamp(x0, 0, static_cast<int>(sizeof(terrain[0])/sizeof(*terrain[0])) - 2);
        z0 = glm::clamp(z0, 0, static_cast<int>(sizeof(terrain)/sizeof(*terrain) - 2));

        // Get the fractional part of the coordinates
        float sx = sampleX - static_cast<float>(x0);
        float sz = sampleZ - static_cast<float>(z0);

        // Perform bilinear interpolation on the terrain heights
        float h00 = terrain[x0][z0];
        float h10 = terrain[x0 + 1][z0];
        float h01 = terrain[x0][z0 + 1];
        float h11 = terrain[x0 + 1][z0 + 1];

        float h0 = (1 - sx) * h00 + sx * h10;
        float h1 = (1 - sx) * h01 + sx * h11;

        float height = -260 + ((1 - sz) * h0 + sz * h1);

        heights[x][z] = height;

        // Create this vertex
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
