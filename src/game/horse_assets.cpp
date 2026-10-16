#include "horse_assets.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

namespace {

const float TAU = glm::two_pi<float>();

const Layout lofted_layout({
    {FLOAT3, "vertex"},
    {FLOAT3, "normal"},
    {FLOAT3, "color"},
    {FLOAT2, "uv"},
    {FLOAT3, "tangent"},
});

// One elliptical cross section of a lofted mesh.
// rx is the width along the ring's side axis, ry the height along its up axis.
// taper narrows the upper half of the section (withers, crest of the neck, ...)
struct Ring {
    glm::vec3 c;
    float rx;
    float ry;
    float taper;
};

// ring on a path running along z (body, neck, head)
Ring RZ(float z, float yc, float rx, float ry, float taper = 0.0f) {
    return {glm::vec3(0.0f, yc, z), rx, ry, taper};
}

// ring on a path running along y (limbs, ears, rider)
Ring RY(float y, float rx, float ry, float zc = 0.0f, float taper = 0.0f) {
    return {glm::vec3(0.0f, y, zc), rx, ry, taper};
}

float CatmullRom(float p0, float p1, float p2, float p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

std::vector<Ring> Subdivide(const std::vector<Ring>& in, int steps) {
    std::vector<Ring> out;
    int n = (int)in.size();
    for (int i = 0; i < n - 1; i++) {
        const Ring& p0 = in[glm::max(i - 1, 0)];
        const Ring& p1 = in[i];
        const Ring& p2 = in[i + 1];
        const Ring& p3 = in[glm::min(i + 2, n - 1)];
        for (int s = 0; s < steps; s++) {
            float t = (float)s / (float)steps;
            Ring r;
            for (int k = 0; k < 3; k++) {
                r.c[k] = CatmullRom(p0.c[k], p1.c[k], p2.c[k], p3.c[k], t);
            }
            r.rx    = glm::max(0.0f, CatmullRom(p0.rx, p1.rx, p2.rx, p3.rx, t));
            r.ry    = glm::max(0.0f, CatmullRom(p0.ry, p1.ry, p2.ry, p3.ry, t));
            r.taper = glm::clamp(CatmullRom(p0.taper, p1.taper, p2.taper, p3.taper, t), 0.0f, 0.95f);
            out.push_back(r);
        }
    }
    out.push_back(in.back());
    return out;
}

struct LoftOptions {
    glm::vec3 ref_up = {0.0f, 1.0f, 0.0f}; // reference used to orient each ring's up axis
    int radial = 28;                        // samples around each ring
    int smooth = 3;                         // catmull rom subdivisions between rings
    bool caps = true;                       // close the two ends
    float arc0 = 0.0f;                      // partial sections (blankets, skirts)
    float arc1 = TAU;
    bool double_sided = false;
};

void AddLoft(ResourceManager& rm, const std::string& name, std::vector<Ring> rings, const LoftOptions& opt) {
    if (opt.smooth > 1) {
        rings = Subdivide(rings, opt.smooth);
    }
    const int n = (int)rings.size();
    const bool closed = (opt.arc1 - opt.arc0) >= TAU - 1e-4f;
    const int cols = closed ? opt.radial : opt.radial + 1;

    // a frame for every ring
    std::vector<glm::vec3> T(n), S(n), U(n);
    for (int i = 0; i < n; i++) {
        glm::vec3 t = rings[glm::min(i + 1, n - 1)].c - rings[glm::max(i - 1, 0)].c;
        t = glm::length(t) > 1e-6f ? glm::normalize(t) : glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 side = glm::cross(t, opt.ref_up);
        if (glm::length(side) < 1e-4f) {
            side = glm::cross(t, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        side = glm::normalize(side);
        T[i] = t;
        S[i] = side;
        U[i] = glm::cross(side, t);
    }

    auto angle = [&](int j) {
        return opt.arc0 + (opt.arc1 - opt.arc0) * (float)j / (float)(closed ? cols : cols - 1);
    };
    auto point = [&](int i, float a) {
        const Ring& r = rings[i];
        float ca = std::cos(a);
        float sa = std::sin(a);
        float w = r.rx * ca * (1.0f - r.taper * glm::max(0.0f, sa));
        return r.c + S[i] * w + U[i] * (r.ry * sa);
    };

    std::vector<std::vector<glm::vec3>> P(n, std::vector<glm::vec3>(cols));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < cols; j++) {
            P[i][j] = point(i, angle(j));
        }
    }

    // texture v runs along the arc length of the path
    std::vector<float> vcoord(n, 0.0f);
    for (int i = 1; i < n; i++) {
        vcoord[i] = vcoord[i - 1] + glm::length(rings[i].c - rings[i - 1].c);
    }
    float total = glm::max(vcoord.back(), 1e-4f);
    for (float& v : vcoord) {
        v /= total;
    }

    std::vector<std::vector<glm::vec3>> N(n, std::vector<glm::vec3>(cols));
    std::vector<std::vector<glm::vec3>> TG(n, std::vector<glm::vec3>(cols));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < cols; j++) {
            int jn = closed ? (j + 1) % cols : glm::min(j + 1, cols - 1);
            int jp = closed ? (j - 1 + cols) % cols : glm::max(j - 1, 0);
            glm::vec3 du = P[i][jn] - P[i][jp];
            glm::vec3 dv = P[glm::min(i + 1, n - 1)][j] - P[glm::max(i - 1, 0)][j];
            glm::vec3 radial = P[i][j] - rings[i].c;
            glm::vec3 nrm = glm::cross(du, dv);
            if (glm::length(nrm) < 1e-7f) {
                nrm = glm::length(radial) > 1e-6f ? radial : (i == 0 ? -T[i] : T[i]);
            }
            nrm = glm::normalize(nrm);
            glm::vec3 outward = glm::length(radial) > 1e-5f ? radial : (i < n / 2 ? -T[i] : T[i]);
            if (glm::dot(nrm, outward) < 0.0f) {
                nrm = -nrm;
            }
            // rings that shrink to a point blend toward the path direction for rounded ends
            float rr = glm::max(rings[i].rx, rings[i].ry);
            if ((i == 0 || i == n - 1) && rr < 0.2f) {
                glm::vec3 axial = i == 0 ? -T[i] : T[i];
                nrm = glm::normalize(glm::mix(axial, nrm, rr / 0.2f));
            }
            N[i][j] = nrm;

            glm::vec3 tg = glm::length(dv) > 1e-7f ? glm::normalize(dv) : T[i];
            tg = tg - nrm * glm::dot(nrm, tg);
            if (glm::length(tg) < 1e-5f) {
                tg = glm::cross(nrm, S[i]);
            }
            TG[i][j] = glm::normalize(tg);
        }
    }

    std::vector<float> verts;
    std::vector<unsigned int> inds;
    auto push_vertex = [&](const glm::vec3& p, const glm::vec3& nm, const glm::vec2& uv, const glm::vec3& tg) {
        verts.insert(verts.end(), {p.x, p.y, p.z, nm.x, nm.y, nm.z, 1.0f, 1.0f, 1.0f, uv.x, uv.y, tg.x, tg.y, tg.z});
    };
    auto vertex_pos = [&](unsigned int idx) {
        return glm::vec3(verts[idx * 14 + 0], verts[idx * 14 + 1], verts[idx * 14 + 2]);
    };
    auto vertex_nrm = [&](unsigned int idx) {
        return glm::vec3(verts[idx * 14 + 3], verts[idx * 14 + 4], verts[idx * 14 + 5]);
    };
    // keep triangles counter clockwise when seen from the side their normals face
    auto push_tri = [&](unsigned int a, unsigned int b, unsigned int c) {
        glm::vec3 face = glm::cross(vertex_pos(b) - vertex_pos(a), vertex_pos(c) - vertex_pos(a));
        glm::vec3 want = vertex_nrm(a) + vertex_nrm(b) + vertex_nrm(c);
        if (glm::dot(face, want) < 0.0f) {
            std::swap(b, c);
        }
        inds.insert(inds.end(), {a, b, c});
    };

    const int row = closed ? cols + 1 : cols; // closed rings duplicate the seam column for uvs
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < row; j++) {
            int jj = j % cols;
            push_vertex(P[i][jj], N[i][jj], glm::vec2((float)j / (float)(row - 1), vcoord[i]), TG[i][jj]);
        }
    }
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < row - 1; j++) {
            unsigned int a = i * row + j;
            unsigned int b = (i + 1) * row + j;
            unsigned int c = (i + 1) * row + j + 1;
            unsigned int d = i * row + j + 1;
            push_tri(a, b, c);
            push_tri(a, c, d);
        }
    }

    if (closed && opt.caps) {
        for (int end = 0; end < 2; end++) {
            int i = end == 0 ? 0 : n - 1;
            glm::vec3 axial = end == 0 ? -T[i] : T[i];
            unsigned int base = (unsigned int)(verts.size() / 14);
            push_vertex(rings[i].c, axial, glm::vec2(0.5f, vcoord[i]), S[i]);
            for (int j = 0; j < cols; j++) {
                push_vertex(P[i][j], axial, glm::vec2((float)j / cols, vcoord[i]), S[i]);
            }
            for (int j = 0; j < cols; j++) {
                push_tri(base, base + 1 + j, base + 1 + (j + 1) % cols);
            }
        }
    }

    if (opt.double_sided) {
        unsigned int count = (unsigned int)(verts.size() / 14);
        size_t num_inds = inds.size();
        for (unsigned int v = 0; v < count; v++) {
            glm::vec3 p = vertex_pos(v);
            glm::vec3 nm = -vertex_nrm(v);
            glm::vec3 tg(verts[v * 14 + 11], verts[v * 14 + 12], verts[v * 14 + 13]);
            push_vertex(p + nm * 0.04f, nm, glm::vec2(verts[v * 14 + 9], verts[v * 14 + 10]), tg);
        }
        for (size_t k = 0; k < num_inds; k += 3) {
            push_tri(inds[k] + count, inds[k + 2] + count, inds[k + 1] + count);
        }
    }

    rm.AddMesh(name, verts, inds, lofted_layout);
}

// ---------------------------------------------------------------------------------------------
// Tileable value noise for the procedural textures
// ---------------------------------------------------------------------------------------------

uint32_t Hash(int x, int y, int seed) {
    uint32_t h = (uint32_t)x * 374761393u + (uint32_t)y * 668265263u + (uint32_t)seed * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

float Lattice(int x, int y, int px, int py, int seed) {
    x = ((x % px) + px) % px;
    y = ((y % py) + py) % py;
    return (float)(Hash(x, y, seed) & 0xFFFFFF) / 16777215.0f;
}

// value noise that repeats every px by py lattice cells
float VNoise(float x, float y, int px, int py, int seed) {
    int xi = (int)std::floor(x);
    int yi = (int)std::floor(y);
    float fx = x - xi;
    float fy = y - yi;
    fx = fx * fx * (3.0f - 2.0f * fx);
    fy = fy * fy * (3.0f - 2.0f * fy);
    float a = Lattice(xi, yi, px, py, seed);
    float b = Lattice(xi + 1, yi, px, py, seed);
    float c = Lattice(xi, yi + 1, px, py, seed);
    float d = Lattice(xi + 1, yi + 1, px, py, seed);
    return glm::mix(glm::mix(a, b, fx), glm::mix(c, d, fx), fy);
}

// u, v in [0, 1); fu, fv are the integer base frequencies so the result tiles
float Fbm(float u, float v, int fu, int fv, int octaves, int seed) {
    float sum = 0.0f;
    float amp = 0.5f;
    float norm = 0.0f;
    for (int o = 0; o < octaves; o++) {
        sum += amp * VNoise(u * fu, v * fv, fu, fv, seed + o * 17);
        norm += amp;
        amp *= 0.5f;
        fu *= 2;
        fv *= 2;
    }
    return sum / norm;
}

void MakeTexture(ResourceManager& rm, const std::string& name, int size, const std::function<glm::vec3(float, float)>& shade) {
    std::vector<unsigned char> data(size * size * 3);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            glm::vec3 c = glm::clamp(shade((x + 0.5f) / size, (y + 0.5f) / size), 0.0f, 1.0f);
            unsigned char* px = &data[(y * size + x) * 3];
            px[0] = (unsigned char)(c.r * 255.0f);
            px[1] = (unsigned char)(c.g * 255.0f);
            px[2] = (unsigned char)(c.b * 255.0f);
        }
    }
    rm.AddTexture(name, data.data(), size, size, 3, GL_REPEAT, GL_LINEAR);
}

void MakeNormalMap(ResourceManager& rm, const std::string& name, int size, const std::function<glm::vec3(float, float)>& normal) {
    MakeTexture(rm, name, size, [&](float u, float v) {
        glm::vec3 n = glm::normalize(normal(u, v));
        return n * 0.5f + 0.5f;
    });
}

// hair grows along v (the loft path) so streaks vary quickly across u
float HairStreak(float u, float v, int across, int along, int seed) {
    return VNoise(u * across, v * along, across, along, seed) * 0.65f + VNoise(u * across * 2, v * along * 2, across * 2, along * 2, seed + 3) * 0.35f;
}

glm::vec3 CoatColor(float u, float v) {
    const glm::vec3 bay(0.47f, 0.25f, 0.11f);
    float broad = Fbm(u, v, 3, 3, 4, 11);
    float dapple = VNoise(u * 18.0f, v * 14.0f, 18, 14, 23);
    float hair = HairStreak(u, v, 300, 12, 31);
    float top = std::sin(TAU * u); // +1 along the topline, -1 under the belly
    glm::vec3 c = bay * (0.82f + 0.32f * broad);
    c *= 1.0f - 0.10f * top;                                   // counter shading
    c += glm::vec3(0.05f, 0.03f, 0.015f) * glm::smoothstep(0.62f, 0.8f, dapple);
    c *= 0.97f + 0.06f * hair;
    return c;
}

void CreateTextures(ResourceManager& rm) {
    MakeTexture(rm, "T_HorseCoat", 512, CoatColor);

    // same coat with a white blaze down the face and a dark soft muzzle
    MakeTexture(rm, "T_HorseHead", 512, [](float u, float v) {
        glm::vec3 c = CoatColor(u, v);
        float width = 0.03f + 0.035f * glm::smoothstep(0.35f, 0.95f, v) + 0.02f * (Fbm(u, v, 4, 4, 2, 5) - 0.5f);
        float blaze = 1.0f - glm::smoothstep(width * 0.75f, width, std::abs(u - 0.25f));
        blaze *= glm::smoothstep(0.24f, 0.32f, v) * (1.0f - glm::smoothstep(0.86f, 0.9f, v));
        c = glm::mix(c, glm::vec3(0.86f, 0.83f, 0.78f) * (0.94f + 0.08f * HairStreak(u, v, 120, 8, 2)), blaze);
        float muzzle = glm::smoothstep(0.8f, 0.9f, v);
        c = glm::mix(c, glm::vec3(0.13f, 0.1f, 0.09f), muzzle * 0.85f);
        return c;
    });

    // black points: lower legs, mane and tail
    MakeTexture(rm, "T_HorsePoints", 256, [](float u, float v) {
        float hair = HairStreak(u, v, 96, 6, 41);
        return glm::vec3(0.075f, 0.058f, 0.05f) * (0.65f + 0.8f * hair);
    });

    MakeTexture(rm, "T_HorseHair", 256, [](float u, float v) {
        float hair = HairStreak(u, v, 128, 4, 43);
        float glint = glm::smoothstep(0.7f, 0.95f, VNoise(u * 64, v * 3, 64, 3, 47));
        return glm::vec3(0.07f, 0.05f, 0.042f) * (0.55f + 0.9f * hair) + glm::vec3(0.09f, 0.06f, 0.04f) * glint;
    });

    MakeTexture(rm, "T_HorseHoof", 128, [](float u, float v) {
        float ridge = VNoise(u * 48, v * 2, 48, 2, 7);
        return glm::vec3(0.21f, 0.18f, 0.15f) * (0.75f + 0.4f * ridge);
    });

    MakeTexture(rm, "T_HorseEye", 32, [](float u, float v) {
        return glm::vec3(0.035f, 0.025f, 0.022f);
    });

    MakeTexture(rm, "T_SaddleLeather", 256, [](float u, float v) {
        float grain = Fbm(u, v, 8, 8, 4, 61);
        float scuff = glm::smoothstep(0.55f, 0.8f, Fbm(u, v, 3, 3, 3, 67));
        glm::vec3 c = glm::vec3(0.38f, 0.2f, 0.09f) * (0.8f + 0.35f * grain);
        // tooled diamond pattern
        float d = std::abs(std::fmod(u * 24.0f + v * 6.0f, 1.0f) - 0.5f) + std::abs(std::fmod(u * 24.0f - v * 6.0f + 100.0f, 1.0f) - 0.5f);
        c *= 0.9f + 0.1f * glm::smoothstep(0.42f, 0.5f, d);
        return glm::mix(c, c * 1.3f, scuff * 0.4f);
    });

    MakeTexture(rm, "T_DarkLeather", 128, [](float u, float v) {
        float grain = Fbm(u, v, 8, 8, 3, 71);
        return glm::vec3(0.15f, 0.09f, 0.055f) * (0.75f + 0.45f * grain);
    });

    // navajo style woven saddle blanket, u = 0.5 runs along the horse's spine
    MakeTexture(rm, "T_SaddleBlanket", 256, [](float u, float v) {
        const glm::vec3 red(0.58f, 0.11f, 0.07f);
        const glm::vec3 cream(0.86f, 0.78f, 0.62f);
        const glm::vec3 black(0.09f, 0.07f, 0.06f);
        const glm::vec3 ochre(0.78f, 0.5f, 0.16f);
        float t = std::abs(u - 0.5f) * 2.0f; // 0 at the spine, 1 at the hanging edges
        glm::vec3 c = red;
        if (t > 0.9f) {
            c = black;
        } else if (t > 0.84f) {
            c = cream;
        } else if (t > 0.79f) {
            c = black;
        } else if (t > 0.35f && t < 0.72f) {
            // stepped diamonds between bands
            float dz = std::abs(std::fmod(v * 4.0f, 1.0f) - 0.5f) * 2.0f;
            float dt = std::abs((t - 0.535f) / 0.185f);
            float diamond = dz + dt;
            diamond = std::floor(diamond * 6.0f) / 6.0f;
            if (diamond < 0.5f) {
                c = black;
            } else if (diamond < 0.67f) {
                c = cream;
            } else if (diamond < 0.84f) {
                c = ochre;
            }
        } else if (t > 0.28f && t < 0.33f) {
            c = cream;
        }
        float weave = 0.88f + 0.12f * ((int)(u * 256) % 2 == (int)(v * 256) % 2 ? 1.0f : 0.0f);
        float wool = 0.85f + 0.3f * Fbm(u, v, 16, 16, 3, 83);
        return c * weave * wool;
    });

    MakeTexture(rm, "T_Tack", 64, [](float u, float v) {
        return glm::vec3(0.12f, 0.075f, 0.045f);
    });

    MakeTexture(rm, "T_TackMetal", 64, [](float u, float v) {
        return glm::vec3(0.62f, 0.6f, 0.56f);
    });

    // rider
    MakeTexture(rm, "T_RiderShirt", 256, [](float u, float v) {
        float weave = ((int)(u * 128) + (int)(v * 128)) % 2 == 0 ? 1.0f : 0.92f;
        float folds = Fbm(u, v, 4, 6, 3, 91);
        return glm::vec3(0.44f, 0.4f, 0.27f) * weave * (0.78f + 0.35f * folds);
    });

    MakeTexture(rm, "T_RiderPants", 256, [](float u, float v) {
        float weave = ((int)(u * 160) + (int)(v * 160)) % 2 == 0 ? 1.0f : 0.9f;
        float folds = Fbm(u, v, 4, 6, 3, 97);
        return glm::vec3(0.27f, 0.21f, 0.15f) * weave * (0.78f + 0.35f * folds);
    });

    MakeTexture(rm, "T_RiderSkin", 64, [](float u, float v) {
        return glm::vec3(0.72f, 0.5f, 0.37f) * (0.92f + 0.12f * Fbm(u, v, 4, 4, 2, 101));
    });

    // felt hat with a dark band just above the brim
    MakeTexture(rm, "T_RiderHat", 256, [](float u, float v) {
        glm::vec3 c = glm::vec3(0.6f, 0.49f, 0.32f) * (0.85f + 0.25f * Fbm(u, v, 6, 6, 3, 107));
        float band = glm::smoothstep(0.24f, 0.26f, v) * (1.0f - glm::smoothstep(0.39f, 0.41f, v));
        return glm::mix(c, glm::vec3(0.1f, 0.07f, 0.05f), band);
    });

    // fine hair grain, perturbs the normal across the direction the hair grows
    MakeNormalMap(rm, "T_HorseHair_n", 256, [](float u, float v) {
        float h = HairStreak(u, v, 128, 6, 113);
        float h2 = HairStreak(u + 1.0f / 256.0f, v, 128, 6, 113);
        return glm::vec3(0.0f, (h2 - h) * 5.0f, 1.0f);
    });

    MakeNormalMap(rm, "T_Cloth_n", 128, [](float u, float v) {
        float wx = std::sin(u * TAU * 64.0f);
        float wy = std::sin(v * TAU * 64.0f);
        return glm::vec3(wx * 0.18f, wy * 0.18f, 1.0f);
    });
}

// ---------------------------------------------------------------------------------------------
// Meshes. Units are roughly decimetres; the horse stands ~17 units at the withers.
// Horse space: forward is -z, up is +y. Limb meshes hang from their joint along -y.
// ---------------------------------------------------------------------------------------------

void CreateHorseMeshes(ResourceManager& rm) {
    LoftOptions along_z;                      // body, neck, head
    along_z.radial = 36;

    LoftOptions limb;                         // anything hanging along -y
    limb.ref_up = glm::vec3(0.0f, 0.0f, -1.0f);
    limb.radial = 24;

    // barrel, chest and hindquarters; path from chest (-z) to rump (+z)
    AddLoft(rm, "M_HorseBody", {
        RZ(-11.4f, 0.9f, 0.3f, 0.4f),
        RZ(-11.1f, 0.8f, 1.9f, 2.4f, 0.2f),
        RZ(-10.2f, 0.6f, 2.85f, 3.55f, 0.35f),
        RZ(-8.5f, 0.75f, 3.2f, 4.25f, 0.5f),
        RZ(-5.9f, 0.35f, 3.6f, 4.15f, 0.35f),
        RZ(-2.7f, 0.0f, 3.95f, 4.0f, 0.2f),
        RZ(0.9f, 0.0f, 3.95f, 3.85f, 0.15f),
        RZ(4.0f, 0.35f, 3.75f, 3.7f, 0.15f),
        RZ(6.75f, 0.8f, 3.7f, 4.0f, 0.2f),
        RZ(8.8f, 0.8f, 3.35f, 3.9f, 0.25f),
        RZ(10.25f, 0.7f, 2.6f, 3.3f, 0.3f),
        RZ(11.1f, 0.8f, 1.4f, 2.0f, 0.3f),
        RZ(11.4f, 0.9f, 0.3f, 0.5f),
    }, along_z);

    // neck runs forward along -z from its base, the crest is on +y
    AddLoft(rm, "M_HorseNeck", {
        RZ(2.4f, -0.8f, 2.7f, 3.9f, 0.3f),
        RZ(0.0f, -0.3f, 2.4f, 3.5f, 0.3f),
        RZ(-2.5f, 0.0f, 1.95f, 3.0f, 0.35f),
        RZ(-5.0f, 0.15f, 1.6f, 2.5f, 0.4f),
        RZ(-7.5f, 0.2f, 1.35f, 2.1f, 0.4f),
        RZ(-9.5f, 0.1f, 1.25f, 1.85f, 0.3f),
        RZ(-10.6f, 0.0f, 1.15f, 1.6f, 0.2f),
        RZ(-11.2f, 0.0f, 0.6f, 0.8f),
    }, along_z);

    // head from the poll toward the muzzle, +y is the face, -y the jaw
    AddLoft(rm, "M_HorseHead", {
        RZ(0.9f, 0.0f, 0.8f, 0.9f),
        RZ(0.3f, -0.1f, 1.2f, 1.45f, 0.2f),
        RZ(-1.1f, -0.65f, 1.45f, 1.95f, 0.3f),
        RZ(-2.5f, -0.5f, 1.3f, 1.65f, 0.3f),
        RZ(-3.9f, -0.3f, 1.0f, 1.25f, 0.2f),
        RZ(-5.0f, -0.25f, 0.88f, 1.07f, 0.15f),
        RZ(-5.9f, -0.33f, 0.97f, 1.1f, 0.1f),
        RZ(-6.5f, -0.38f, 0.82f, 0.92f),
        RZ(-6.85f, -0.42f, 0.35f, 0.45f),
    }, along_z);

    LoftOptions up_y;                          // ears, horn, rider torso/head grow along +y
    up_y.ref_up = glm::vec3(0.0f, 0.0f, -1.0f);
    up_y.radial = 16;
    AddLoft(rm, "M_HorseEar", {
        RY(-0.2f, 0.32f, 0.22f),
        RY(0.3f, 0.42f, 0.3f),
        RY(0.9f, 0.33f, 0.22f, -0.05f),
        RY(1.5f, 0.06f, 0.05f, -0.1f),
    }, up_y);

    LoftOptions small = along_z;
    small.radial = 16;
    AddLoft(rm, "M_HorseEye", {
        RZ(0.32f, 0.0f, 0.02f, 0.02f),
        RZ(0.2f, 0.0f, 0.25f, 0.25f),
        RZ(0.0f, 0.0f, 0.33f, 0.33f),
        RZ(-0.2f, 0.0f, 0.25f, 0.25f),
        RZ(-0.32f, 0.0f, 0.02f, 0.02f),
    }, small);

    // a lock of mane hanging from the crest; thin across x, wide along the neck
    LoftOptions strip = limb;
    strip.radial = 12;
    AddLoft(rm, "M_HorseManeLock", {
        RY(0.6f, 0.22f, 0.85f),
        RY(0.0f, 0.3f, 1.0f),
        RY(-1.4f, 0.22f, 0.85f, 0.15f),
        RY(-2.6f, 0.12f, 0.55f, 0.35f),
        RY(-3.3f, 0.03f, 0.15f, 0.5f),
    }, strip);

    AddLoft(rm, "M_HorseTailDock", {
        RY(0.6f, 0.6f, 0.6f),
        RY(0.0f, 0.68f, 0.68f),
        RY(-1.3f, 0.6f, 0.62f),
        RY(-2.6f, 0.55f, 0.6f),
    }, limb);

    // hair fans out flat (wide across x, thin front to back) as it falls
    AddLoft(rm, "M_HorseTailHair", {
        RY(0.3f, 0.5f, 0.55f),
        RY(-1.3f, 0.45f, 0.75f),
        RY(-2.9f, 0.42f, 0.85f),
    }, limb);

    AddLoft(rm, "M_HorseTailTip", {
        RY(0.3f, 0.42f, 0.85f),
        RY(-1.5f, 0.36f, 0.8f),
        RY(-2.8f, 0.2f, 0.42f),
        RY(-3.4f, 0.05f, 0.04f),
    }, limb);

    // legs: the upper segments extend into the body so the joints never show a gap
    AddLoft(rm, "M_HorseFrontUpper", {
        RY(2.6f, 1.85f, 2.5f),
        RY(1.2f, 1.75f, 2.3f),
        RY(0.0f, 1.5f, 1.95f),
        RY(-1.5f, 1.15f, 1.45f),
        RY(-3.2f, 0.85f, 1.0f),
        RY(-5.0f, 0.78f, 0.9f, -0.05f),
        RY(-5.6f, 0.78f, 0.86f, -0.08f),
    }, limb);

    AddLoft(rm, "M_HorseFrontCannon", {
        RY(0.3f, 0.78f, 0.88f, -0.05f),
        RY(-0.7f, 0.55f, 0.62f),
        RY(-3.2f, 0.5f, 0.58f),
        RY(-4.0f, 0.62f, 0.76f, 0.08f),
        RY(-4.4f, 0.58f, 0.7f, 0.05f),
    }, limb);

    AddLoft(rm, "M_HorseHindUpper", {
        RY(3.2f, 2.15f, 2.9f),
        RY(1.6f, 2.05f, 2.75f),
        RY(0.0f, 1.8f, 2.3f, 0.1f),
        RY(-1.5f, 1.35f, 1.8f, 0.15f),
        RY(-3.0f, 0.95f, 1.25f, 0.15f),
        RY(-5.2f, 0.75f, 1.0f, 0.2f),
        RY(-5.8f, 0.75f, 0.95f, 0.3f),
        RY(-6.2f, 0.7f, 0.8f, 0.15f),
    }, limb);

    AddLoft(rm, "M_HorseHindCannon", {
        RY(0.4f, 0.78f, 0.95f),
        RY(-0.8f, 0.55f, 0.68f),
        RY(-4.2f, 0.5f, 0.6f),
        RY(-5.0f, 0.62f, 0.78f, 0.08f),
        RY(-5.4f, 0.58f, 0.7f, 0.05f),
    }, limb);

    AddLoft(rm, "M_HorsePastern", {
        RY(0.25f, 0.6f, 0.72f),
        RY(-0.6f, 0.48f, 0.55f),
        RY(-1.25f, 0.56f, 0.63f),
        RY(-1.45f, 0.6f, 0.66f),
    }, limb);

    AddLoft(rm, "M_HorseHoof", {
        RY(0.15f, 0.58f, 0.64f, 0.12f),
        RY(0.0f, 0.63f, 0.7f, 0.1f),
        RY(-0.55f, 0.74f, 0.84f, -0.05f),
        RY(-0.95f, 0.82f, 0.95f, -0.18f),
        RY(-1.0f, 0.78f, 0.9f, -0.18f),
    }, limb);

    // ---- tack ----
    LoftOptions blanket = along_z;
    blanket.arc0 = glm::radians(-38.0f);
    blanket.arc1 = glm::radians(218.0f);
    blanket.caps = false;
    blanket.double_sided = true;
    blanket.radial = 40;
    AddLoft(rm, "M_SaddleBlanket", {
        RZ(-7.0f, 0.55f, 3.6f, 4.45f, 0.4f),
        RZ(-6.2f, 0.4f, 3.85f, 4.5f, 0.32f),
        RZ(-3.0f, 0.05f, 4.2f, 4.3f, 0.18f),
        RZ(0.6f, 0.0f, 4.18f, 4.12f, 0.15f),
        RZ(1.6f, 0.05f, 4.1f, 4.08f, 0.15f),
    }, blanket);

    LoftOptions skirt = blanket;
    skirt.arc0 = glm::radians(-8.0f);
    skirt.arc1 = glm::radians(188.0f);
    AddLoft(rm, "M_SaddleSkirt", {
        RZ(-5.4f, 0.5f, 4.0f, 4.65f, 0.32f),
        RZ(-3.0f, 0.1f, 4.38f, 4.5f, 0.2f),
        RZ(0.0f, 0.05f, 4.38f, 4.35f, 0.16f),
        RZ(0.9f, 0.1f, 4.3f, 4.3f, 0.16f),
    }, skirt);

    AddLoft(rm, "M_SaddleSeat", {
        RZ(-5.7f, 4.6f, 0.5f, 0.45f),
        RZ(-5.3f, 4.75f, 1.5f, 0.85f, 0.25f),
        RZ(-4.5f, 4.65f, 2.5f, 0.95f, 0.3f),
        RZ(-2.5f, 4.2f, 2.95f, 0.75f, 0.2f),
        RZ(-0.5f, 4.35f, 2.85f, 0.8f, 0.2f),
        RZ(0.6f, 4.85f, 2.35f, 0.95f, 0.3f),
        RZ(1.2f, 5.05f, 1.5f, 0.8f),
        RZ(1.5f, 5.05f, 0.5f, 0.35f),
    }, along_z);

    AddLoft(rm, "M_SaddleHorn", {
        RY(-0.4f, 0.4f, 0.4f),
        RY(0.6f, 0.3f, 0.3f),
        RY(1.0f, 0.55f, 0.55f),
        RY(1.2f, 0.5f, 0.5f),
        RY(1.28f, 0.08f, 0.08f),
    }, up_y);

    LoftOptions band = along_z;
    band.smooth = 1;
    AddLoft(rm, "M_Girth", {
        RZ(-6.0f, 0.3f, 3.82f, 4.3f, 0.32f),
        RZ(-5.4f, 0.25f, 3.86f, 4.3f, 0.3f),
    }, band);

    AddLoft(rm, "M_StirrupStrap", {
        RY(0.0f, 0.12f, 0.45f),
        RY(-6.0f, 0.12f, 0.45f),
    }, LoftOptions{glm::vec3(0.0f, 0.0f, -1.0f), 8, 1, true});

    AddLoft(rm, "M_Noseband", {
        RZ(0.15f, 0.0f, 1.0f, 1.12f),
        RZ(-0.15f, 0.0f, 1.0f, 1.12f),
    }, band);

    AddLoft(rm, "M_Headstall", {
        RZ(0.2f, 0.0f, 1.32f, 1.62f, 0.15f),
        RZ(-0.2f, 0.0f, 1.32f, 1.62f, 0.15f),
    }, band);

    // a unit length rope hanging along -y, stretched between hands and bit at runtime
    AddLoft(rm, "M_Rein", {
        RY(0.0f, 0.09f, 0.09f),
        RY(-1.0f, 0.09f, 0.09f),
    }, LoftOptions{glm::vec3(0.0f, 0.0f, -1.0f), 8, 1, false});
}

void CreateRiderMeshes(ResourceManager& rm) {
    LoftOptions up_y;
    up_y.ref_up = glm::vec3(0.0f, 0.0f, -1.0f);
    up_y.radial = 24;

    LoftOptions limb;
    limb.ref_up = glm::vec3(0.0f, 0.0f, -1.0f);
    limb.radial = 18;

    AddLoft(rm, "M_RiderTorso", {
        RY(-0.4f, 1.6f, 1.25f),
        RY(0.6f, 1.85f, 1.35f),
        RY(2.0f, 1.6f, 1.1f),
        RY(3.6f, 1.85f, 1.2f),
        RY(5.0f, 2.1f, 1.25f),
        RY(6.0f, 2.15f, 1.1f),
        RY(6.6f, 1.4f, 0.9f),
        RY(7.0f, 0.55f, 0.55f),
        RY(7.7f, 0.5f, 0.5f),
    }, up_y);

    AddLoft(rm, "M_RiderHead", {
        RY(0.0f, 0.5f, 0.5f),
        RY(0.4f, 0.82f, 0.92f),
        RY(1.2f, 0.98f, 1.08f, -0.05f),
        RY(2.0f, 0.9f, 1.0f),
        RY(2.55f, 0.55f, 0.6f),
        RY(2.75f, 0.08f, 0.08f),
    }, up_y);

    LoftOptions hat = up_y;
    hat.smooth = 1;
    hat.radial = 32;
    AddLoft(rm, "M_RiderHat", {
        RY(0.0f, 2.35f, 2.45f),
        RY(0.14f, 2.4f, 2.5f),
        RY(0.18f, 1.15f, 1.25f),
        RY(1.15f, 1.08f, 1.15f),
        RY(1.42f, 0.86f, 0.92f),
        RY(1.5f, 0.1f, 0.1f),
    }, hat);

    AddLoft(rm, "M_RiderUpperArm", {
        RY(0.35f, 0.62f, 0.62f),
        RY(0.0f, 0.64f, 0.64f),
        RY(-1.5f, 0.52f, 0.56f),
        RY(-3.0f, 0.43f, 0.46f),
        RY(-3.25f, 0.4f, 0.4f),
    }, limb);

    AddLoft(rm, "M_RiderForearm", {
        RY(0.25f, 0.44f, 0.46f),
        RY(-1.0f, 0.42f, 0.44f),
        RY(-2.6f, 0.34f, 0.32f),
        RY(-2.85f, 0.3f, 0.28f),
    }, limb);

    AddLoft(rm, "M_RiderHand", {
        RY(0.1f, 0.33f, 0.26f),
        RY(-0.4f, 0.42f, 0.34f),
        RY(-0.85f, 0.36f, 0.3f),
        RY(-1.0f, 0.12f, 0.1f),
    }, limb);

    AddLoft(rm, "M_RiderThigh", {
        RY(0.5f, 0.95f, 0.95f),
        RY(0.0f, 1.0f, 1.05f),
        RY(-2.5f, 0.82f, 0.88f),
        RY(-5.0f, 0.58f, 0.62f),
        RY(-5.3f, 0.55f, 0.58f),
    }, limb);

    AddLoft(rm, "M_RiderShin", {
        RY(0.3f, 0.58f, 0.62f),
        RY(-0.6f, 0.6f, 0.66f),
        RY(-2.6f, 0.48f, 0.52f),
    }, limb);

    // boot shaft from mid shin down, then the foot pointing forward (-z)
    AddLoft(rm, "M_RiderBoot", {
        RY(0.2f, 0.56f, 0.6f),
        RY(-1.6f, 0.5f, 0.55f),
        RY(-2.0f, 0.5f, 0.62f, -0.2f),
        RY(-2.3f, 0.48f, 0.85f, -0.65f),
        RY(-2.45f, 0.42f, 0.8f, -0.75f),
        RY(-2.55f, 0.1f, 0.2f, -0.6f),
    }, limb);
}

void CreateDustMesh(ResourceManager& rm) {
    // each point carries a random direction (position) and three random seeds (color)
    std::vector<float> verts;
    std::vector<unsigned int> inds;
    const int count = 28;
    for (int i = 0; i < count; i++) {
        glm::vec3 dir(
            Lattice(i, 0, 1 << 20, 4, 1) * 2.0f - 1.0f,
            Lattice(i, 1, 1 << 20, 4, 1) * 2.0f - 1.0f,
            Lattice(i, 2, 1 << 20, 4, 1) * 2.0f - 1.0f);
        glm::vec3 seeds(Lattice(i, 3, 1 << 20, 8, 2), Lattice(i, 4, 1 << 20, 8, 2), Lattice(i, 5, 1 << 20, 8, 2));
        verts.insert(verts.end(), {dir.x, dir.y, dir.z, 0.0f, 1.0f, 0.0f, seeds.x, seeds.y, seeds.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f});
    }
    rm.AddMesh("M_Dust", verts, inds, lofted_layout);
}

} // namespace

namespace HorseAssets {

void Load(ResourceManager& resman) {
    CreateTextures(resman);
    CreateHorseMeshes(resman);
    CreateRiderMeshes(resman);
    CreateDustMesh(resman);
}

}
