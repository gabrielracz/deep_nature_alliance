#include <glm/gtc/noise.hpp>

#include "defines.h"
#include "game.h"

// index into a 1D array as if it was 2D
#define GIX(x, z, width) ((x) + (z) * width)

Terrain::Terrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, float xwidth, float zwidth, float density, Game* game)
    : SceneNode(name, mesh_id, shader_id, texture_id), xwidth(xwidth), zwidth(zwidth), game(game) {
    // generate uniform grid
    num_xsteps = xwidth * density;
    num_zsteps = zwidth * density;
    xstep = xwidth / num_xsteps;
    zstep = zwidth / num_zsteps;

    GenerateHeightmap();
    GenerateNormals();
    GenerateTangents();
    GenerateObstacles();
    GenerateUV();
    GenerateMesh();
}

void Terrain::GenerateHeightmap() {
    heights.resize(num_xsteps, std::vector<float>(num_zsteps, 0.0));
    if (true) {
        for (int z = 0; z < num_zsteps; z++) {
            for (int x = 0; x < num_xsteps; x++) {
                float sampleX = x * xstep;
                float sampleZ = z * zstep;

                int x0 = static_cast<int>(std::floor(sampleX));
                int z0 = static_cast<int>(std::floor(sampleZ));

                // Clamp the coordinates to be within valid range
                x0 = glm::clamp(x0, 0, static_cast<int>(sizeof(terrain[0]) / sizeof(*terrain[0])) - 2);
                z0 = glm::clamp(z0, 0, static_cast<int>(sizeof(terrain) / sizeof(*terrain) - 2));

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
    } else {
        for (int z = 0; z < num_zsteps; z++) {
            for (int x = 0; x < num_xsteps; x++) {
                glm::vec2 sample = glm::vec2(x * xstep, z * zstep) / 100.0f;
                float height = glm::perlin(sample) * 50.0;
                // shelf generation:
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
    }
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
    float interp = (1 - sz) * h0 + sz * h1;

    return interp + transform.GetPosition().y;
}

float Terrain::SampleSlope(float x, float z, glm::vec3 dir) {
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
    glm::vec3 n00 = normals[x0][z0];
    glm::vec3 n10 = normals[x0 + 1][z0];
    glm::vec3 n01 = normals[x0][z0 + 1];
    glm::vec3 n11 = normals[x0 + 1][z0 + 1];

    glm::vec3 n0 = (1 - sx) * n00 + sx * n10;
    glm::vec3 n1 = (1 - sx) * n01 + sx * n11;
    glm::vec3 interp = glm::normalize((1 - sz) * n0 + sz * n1);

    float slope = 0.0f;
    if (dir == glm::vec3(0.0f)) {
        float slopeX = glm::abs(glm::dot(interp, {1.0, 0.0, 0.0}));
        float slopeY = glm::abs(glm::dot(interp, {0.0, 0.0, 1.0}));
        slope = glm::max(slopeX, slopeY);
    } else {
        slope = glm::abs(glm::dot(dir, interp));
    }

    return slope;
}

glm::vec3 Terrain::InterpNormals(int x0, int z0, float sx, float sz) {
    glm::vec3 n00 = normals[x0][z0];
    glm::vec3 n10 = normals[x0 + 1][z0];
    glm::vec3 n01 = normals[x0][z0 + 1];
    glm::vec3 n11 = normals[x0 + 1][z0 + 1];

    glm::vec3 n0 = (1 - sx) * n00 + sx * n10;
    glm::vec3 n1 = (1 - sx) * n01 + sx * n11;
    glm::vec3 interp = glm::normalize((1 - sz) * n0 + sz * n1);
    return interp;
}