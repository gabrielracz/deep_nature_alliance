#include <glm/gtc/noise.hpp>

#include "terrain.h"
#include "terrain_data.h"
#include "defines.h"
#include "game.h"
#include "colliders/colliders.h"

// index into a 1D array as if it was 2D
#define GIX(x, z, width) ((x) + (z) * width)

float MAX_PASSABLE_SLOPE = 0.5f;

Terrain::Terrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, TerrainType t, float xwidth, float zwidth, float density, Game* game)
    : SceneNode(name, mesh_id, shader_id, texture_id), xwidth(xwidth), zwidth(zwidth), density(density), type(t), game(game) {

    // generate uniform grid
    num_xsteps = xwidth * density;
    num_zsteps = zwidth * density;
    xstep = xwidth / num_xsteps;
    zstep = zwidth / num_zsteps;

    // allocate space for attributes
    heights.resize(num_xsteps, std::vector<float>(num_zsteps, 0.0));
    normals.resize(num_xsteps, std::vector<glm::vec3>(num_zsteps, {0.0, 1.0, 0.0}));
    obstacles.resize(num_xsteps - 1, std::vector<bool>(num_zsteps - 1, true));
    impassable.resize(num_xsteps, std::vector<bool>(num_zsteps, false));
    tangents.resize(num_xsteps, std::vector<glm::vec3>(num_zsteps, {1.0, 0.0, 0.0}));
    uvs.resize(num_xsteps, std::vector<glm::vec2>(num_zsteps, {0.0, 0.0}));

    GenerateHeightmap(type);
    GenerateNormals();
    GenerateTangents();
    GenerateObstacles();
    GenerateImPassable();
    GenerateUV();
    GenerateMesh();

    SetCollider(new TerrainCollider(*this));
}

void Terrain::GenerateHeightmap(TerrainType type) {
    switch(type) {
        case TerrainType::MOON:
            GenerateQMoon();
            break;
        case TerrainType::FOREST:
            SampleTerrainForHeight(mountain_terrain, 500.0f);
            break;
        case TerrainType::DUNES:
            SampleTerrainForHeight(dunes_terrain, 700.0f);
            break;
        case TerrainType::LAVA:
            GenerateLava();
            break;
        default:
            break;
    }
}

void Terrain::GenerateQMoon() {
    SampleTerrainForHeight(moon_terrain, 10.0f);
    // float image_xstep = sizeof(terrain[0]) / sizeof(*terrain[0]) / num_xsteps;
    // float image_zstep = sizeof(terrain) / sizeof(*terrain) / num_zsteps;
    // for (int z = 0; z < num_zsteps; z++) {
    //     for (int x = 0; x < num_xsteps; x++) {
    //         float height;
    //         bool imageTerrain = true;
    //         if (imageTerrain) {
    //             float sampleX = x * image_xstep;
    //             float sampleZ = z * image_zstep;

    //             int x0 = static_cast<int>(std::floor(sampleX));
    //             int z0 = static_cast<int>(std::floor(sampleZ));

    //             // Clamp the coordinates to be within valid range
    //             x0 = glm::clamp(x0, 0, static_cast<int>(sizeof(terrain[0]) / sizeof(*terrain[0])) - 2);
    //             z0 = glm::clamp(z0, 0, static_cast<int>(sizeof(terrain) / sizeof(*terrain) - 2));

    //             // Get the fractional part of the coordinates
    //             float sx = sampleX - static_cast<float>(x0);
    //             float sz = sampleZ - static_cast<float>(z0);

    //             // Perform bilinear interpolation on the terrain heights
    //             float h00 = terrain[x0][z0] * 10;
    //             float h10 = terrain[x0 + 1][z0] * 10;
    //             float h01 = terrain[x0][z0 + 1] * 10;
    //             float h11 = terrain[x0 + 1][z0 + 1] * 10;

    //             float h0 = (1 - sx) * h00 + sx * h10;
    //             float h1 = (1 - sx) * h01 + sx * h11;

    //             glm::vec2 sample = glm::vec2(x * xstep, z * zstep) / 50.0f;
    //             float perlin = glm::perlin(sample);

    //             height = (1 - sz) * h0 + sz * h1 + perlin;
    //         } else {
    //             glm::vec2 sample = glm::vec2(x * xstep, z * zstep) / 100.0f;
    //             height = glm::perlin(sample) * 50.0;
    //             // shelf generation:
    //             // float height = 0.0;
    //             // if (x > 150){
    //             //     height = 100.0;
    //             // }
    //         }
    //         heights[x][z] = height;
    //     }
    // }

    int min_craters = 40;
    int max_craters = 60;
    float min_crater_radius = 2;
    float max_crater_radius = 20;
    float inner_crater_noise = 10;
    float min_crater_depth = 5.0f;
    float max_crater_depth = 20.0f;
    float bottom_crater_noise = -20.0f;
    float crater_ridge_size = 0.2f;

    int num_craters = glm::linearRand(min_craters, max_craters);

    for (int i = 0; i < num_craters; ++i) {
        glm::vec2 position = glm::linearRand(glm::vec2(0.0f), glm::vec2(num_xsteps, num_xsteps));
        float radius = glm::linearRand(min_crater_radius, max_crater_radius);
        float base_height = glm::linearRand(-max_crater_depth, -min_crater_depth);

        for (int y = 0; y < num_xsteps; ++y) {
            for (int x = 0; x < num_xsteps; ++x) {
                float distance = glm::distance(glm::vec2(x, y), position);

                if (distance < radius) {
                    float bottom_perlin = glm::perlin(glm::vec2(x * xstep, y * zstep) / 100.0f);
                    float bottom_noise = bottom_perlin * bottom_crater_noise;
                    float perlin_value = glm::perlin(glm::vec2(x, y) * inner_crater_noise);
                    float height_variation = perlin_value * inner_crater_noise;
                    heights[y][x] = base_height + height_variation;

                    float depth = 0.5f * (1.0f - (distance / radius));
                    if (depth > 0.0f) {
                        heights[y][x] -= depth;

                        if (depth < crater_ridge_size) {
                            heights[y][x] -= bottom_noise;
                        }
                    }
                }
            }
        }
    }

    float item_crater_radius = 20.0f;
    float item_crater_inner_radius = 6.0f;
    float spawn_canyon_radius = 40.0f;
    float item_crater_depth = 50.0f;
    float item_crater_spire_height = 20.0f;
    float max_spawn_canyon_depth = 500.0f;
    float min_spawn_canyon_depth = 200.0f;
    float player_platform_radius = 10.0f;
    float player_platform_depth = 8.0f;

    glm::vec2 spawn_position = glm::vec2(num_xsteps/2, num_zsteps/2);
    glm::vec2 item_position = glm::vec2((spawn_canyon_radius/2) + 1, (spawn_canyon_radius/2) + 1);

    for (int z = 0; z < num_zsteps; ++z) {
        for (int x = 0; x < num_xsteps; ++x) {
            float distance = glm::distance(glm::vec2(x, z), spawn_position);
            if (distance < spawn_canyon_radius && distance > player_platform_radius) {
                float spawn_canyon_depth = glm::linearRand(min_spawn_canyon_depth, max_spawn_canyon_depth);
                heights[z][x] -= spawn_canyon_depth;
                continue;
            } else if ( distance < player_platform_radius + 1) {
                float perlin_value = glm::perlin(glm::vec2(x, z) * 0.01f);
                float height_variation = perlin_value * glm::linearRand(perlin_value * 10.0f, perlin_value * 10.0f + 2.5f);
                heights[z][x] = -player_platform_depth + height_variation;
                continue;
            }

            float item_distance = glm::distance(glm::vec2(x, z), item_position);
            if (item_distance < item_crater_radius) {
                float bottom_perlin = glm::perlin(glm::vec2(x * xstep, z * zstep) / 100.0f);
                float bottom_noise = bottom_perlin * bottom_crater_noise;
                float perlin_value = glm::perlin(glm::vec2(x, z) * inner_crater_noise);
                float height_variation = perlin_value * inner_crater_noise;
                heights[z][x] -= item_crater_depth + height_variation;

                float depth = 0.5f * (1.0f - (distance / item_crater_radius));
                if (depth > 0.0f) {
                    heights[z][x] -= depth;

                    if (depth < crater_ridge_size) {
                        heights[z][x] -= bottom_noise;
                    }
                }
            }

            if (item_distance < item_crater_inner_radius) {
                float depth = 0.5f * (1.0f - (distance / item_crater_radius));
                if (depth < crater_ridge_size) {
                    float bottom_perlin = glm::perlin(glm::vec2(x * xstep, z * zstep) / 100.0f);
                    float bottom_noise = bottom_perlin * bottom_crater_noise;
                    heights[z][x] -= bottom_noise;
                } 
                
                heights[z][x] += item_crater_depth * 0.5;
            }

            if (item_distance < 2) {
                heights[z][x] += item_crater_spire_height;
            }

        }
    }
}

void Terrain::SampleTerrainForHeight(const std::vector<std::vector<float>>& terrainArray, float heightMultiplier, int tileX, int tileZ) {
    float image_xstep = terrainArray[0].size() / num_xsteps;
    float image_zstep = terrainArray.size() / num_zsteps;
    for (int z = 0; z < num_zsteps; z++) {
        for (int x = 0; x < num_xsteps; x++) {
            // Calculate the tiled sample coordinates
            float sampleX = fmod(x * image_xstep * tileX, terrainArray[0].size());
            float sampleZ = fmod(z * image_zstep * tileZ, terrainArray.size());

            int x0 = static_cast<int>(sampleX);
            int z0 = static_cast<int>(sampleZ);

            // Wrap the coordinates within valid range
            x0 = (x0 + terrainArray[0].size()) % terrainArray[0].size();
            z0 = (z0 + terrainArray.size()) % terrainArray.size();

            // Get the fractional part of the coordinates
            float sx = sampleX - static_cast<float>(x0);
            float sz = sampleZ - static_cast<float>(z0);

            // Perform bilinear interpolation on the terrain heights
            float h00 = terrainArray[x0][z0] * heightMultiplier;
            float h10 = terrainArray[(x0 + 1) % terrainArray[0].size()][z0] * heightMultiplier;
            float h01 = terrainArray[x0][(z0 + 1) % terrainArray.size()] * heightMultiplier;
            float h11 = terrainArray[(x0 + 1) % terrainArray[0].size()][(z0 + 1) % terrainArray.size()] * heightMultiplier;

            float h0 = (1 - sx) * h00 + sx * h10;
            float h1 = (1 - sx) * h01 + sx * h11;

            heights[x][z] = (1 - sz) * h0 + sz * h1;
        }
    }
}

void Terrain::GenerateLava() {
    for (int z = 0; z < num_zsteps; z++) {
        for (int x = 0; x < num_xsteps; x++) {
                glm::vec2 sample = glm::vec2(x * 0.5f, z * 0.5f) ;
                float height = glm::perlin(sample) * 5.0f;
                heights[z][x] = height;
        }
    }
}

void Terrain::GenerateNormals() {
    // method from: https://stackoverflow.com/a/21660173
    // for now ignore the outer edge ring
    for (int z = 1; z < num_zsteps-1; z++) {
        for (int x = 1; x < num_xsteps-1; x++) {
            float hl =  heights[x-1][z];
            float hr =  heights[x+1][z];
            float hu =  heights[x][z+1];
            float hd =  heights[x][z-1];
            float hul = heights[x-1][z+1];
            float hur = heights[x+1][z+1];
            float hdl = heights[x-1][z-1];
            float hdr = heights[x+1][z-1];

            glm::vec3 norm = {(2*(hl - hr) - hur + hdl + hu - hd) / xstep,
                              6,
                              (2*(hd - hu) + hur + hdl - hu - hl) / zstep};
            norm = glm::normalize(norm);
            normals[x][z] = norm;
        }
    }
}
void Terrain::GenerateObstacles() {
    for (int z = 1; z < num_zsteps-2; z++) {
        for (int x = 1; x < num_xsteps-2; x++) {
            glm::vec3 norm = InterpNormals(x, z, 0.5, 0.5);
            float slopeX = glm::abs(glm::dot(norm, {1.0, 0.0, 0.0}));
            float slopeY = glm::abs(glm::dot(norm, {0.0, 0.0, 1.0})); 
            float slope = glm::max(slopeX, slopeY);

            if(slope > MAX_PASSABLE_SLOPE) {
                obstacles[x][z] = true;
            } else {
                obstacles[x][z] = false;
            }
        }
    }
}
void Terrain::GenerateImPassable() {
    for (int x = 0; x <= num_xsteps-1; ++x) {
        impassable[x][0] = true; // bottom edge
        impassable[x][num_zsteps - 2] = true; // top edge
    }
    for (int z = 0; z <= num_zsteps-1; ++z) {
        impassable[0][z] = true; // left edge
        impassable[num_xsteps - 2][z] = true; // right edge
    }
}

void Terrain::GenerateTangents() {
    for (int z = 0; z < num_zsteps-1; z++) {
        for (int x = 0; x < num_xsteps-1; x++) {
            const glm::vec3 right = {1.0, 0.0, 0.0};
            glm::vec3 norm = normals[x][z];
            glm::vec3 estimate = glm::cross(right, norm); //original estimate of tangent
            glm::vec3 tan = glm::cross(estimate, norm); // get closer to the true tangent
            tangents[x][z] = tan;
        }
    }
}

void Terrain::GenerateUV() {
    for (int z = 0; z < num_zsteps; z++) {
        for (int x = 0; x < num_xsteps; x++) {
            glm::vec2 uv = {
                (x*xstep)/xwidth,
                (z*zstep)/zwidth
            };
            uvs[x][z] = uv;
        }
    }
}

void Terrain::GenerateMesh() {
    Layout layout({{FLOAT3, "vertex"},
                   {FLOAT3, "normal"},
                   {FLOAT3, "color"},
                   {FLOAT2, "uv"},
                   {FLOAT3, "tangent"}
                   });

    assert(heights.size() == normals.size() && normals.size() == tangents.size());
    for(int x = 0; x < heights.size(); x++){ 
        for(int z = 0; z < heights[0].size(); z++) {
            glm::vec3 pos = {x * xstep, heights[x][z], z * zstep};
            glm::vec3 normal = normals[x][z];
            glm::vec3 tangent = tangents[x][z];
            // color impassable regions magenta
            glm::vec3 color = {Colors::SeaBlue};
            if(obstacles[glm::clamp(x-1, 0, (int)obstacles.size())][glm::clamp(z-1, 0, (int)obstacles[0].size())]) {
                color = Colors::Magenta;
            }
            if(impassable[glm::clamp(x-1, 0, (int)impassable.size())][glm::clamp(z-1, 0, (int)impassable[0].size())]) {
                
                color = Colors::Yellow;
            }
            glm::vec2 uv = uvs[x][z];

            APPEND_VEC3(vertices, pos);
            APPEND_VEC3(vertices, normal);
            APPEND_VEC3(vertices, color);
            APPEND_VEC2(vertices, uv);
            APPEND_VEC3(vertices, tangent);
        }
    }

// triangulate the grid of points
    for (int z = 0; z < num_zsteps - 1; z++) {
        for (int x = 0; x < num_xsteps - 1; x++) {
            indices.push_back(GIX(x, z, num_xsteps));
            indices.push_back(GIX(x + 1, z + 1, num_xsteps));
            indices.push_back(GIX(x, z + 1, num_xsteps));

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

    float h00 = heights[x0][z0];
    float h10 = heights[x0 + 1][z0];
    float h01 = heights[x0][z0 + 1];
    float h11 = heights[x0 + 1][z0 + 1];

    float h0 = (1 - sx) * h00 + sx * h10;
    float h1 = (1 - sx) * h01 + sx * h11;
    float interp = (1 - sz) * h0 + sz * h1;

    return interp + transform.GetPosition().y;
}

bool Terrain::SamplePassable(float x, float z) {
    float terrainX = x / (xwidth / (impassable.size() - 1)) + (num_xsteps / 2.0);
    float terrainZ = z / (zwidth / (impassable[0].size() - 1)) + (num_zsteps / 2.0);

    // Get the integer coordinates of the cell
    int x0 = static_cast<int>(std::floor(terrainX));
    int z0 = static_cast<int>(std::floor(terrainZ));

    // Clamp the coordinates to be within valid range
    x0 = glm::clamp(x0, 0, static_cast<int>(impassable.size() - 1));
    z0 = glm::clamp(z0, 0, static_cast<int>(impassable[0].size() - 1));

    return impassable[x0][z0];
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
    if(dir == glm::vec3(0.0f)) {
        float slopeX = glm::abs(glm::dot(interp, {1.0, 0.0, 0.0}));
        float slopeY = glm::abs(glm::dot(interp, {0.0, 0.0, 1.0})); 
        slope = glm::max(slopeX, slopeY);
    } else {
        slope = glm::abs(glm::dot(dir, interp));
    }

    return slope;
}

glm::vec3 Terrain::SampleNormal(float x, float z) {
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
    return glm::normalize((1 - sz) * n0 + sz * n1);
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


bool Terrain::SampleOn(float x , float z)
{
    float terrainX = x / (xwidth / (heights.size() - 1)) + (num_xsteps / 2.0);
    float terrainZ = z / (zwidth / (heights[0].size() - 1)) + (num_zsteps / 2.0);

    // Get the integer coordinates of the cell
    int x0 = static_cast<int>(std::floor(terrainX));
    int z0 = static_cast<int>(std::floor(terrainZ));
    return x0 > 0 && x0 < static_cast<int>(heights.size()) && z0 > 0 && z0  < static_cast<int>(heights[0].size());

}