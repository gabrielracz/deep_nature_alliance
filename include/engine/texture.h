#ifndef TEXTURE_H
#define TEXTURE_H

#include "shader.h"

class Texture {
    public:
	unsigned int id;
    float texture_repetition = 1;
    bool alpha_enabled = false;
    Texture() = default;
    Texture(int id, float rep) : id(id), texture_repetition(rep){}
    Texture(unsigned char* data, int width, int height, int n_channels, int wrap_option);
    void Bind(Shader* shader);
};

#endif