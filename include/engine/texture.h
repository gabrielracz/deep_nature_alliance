#ifndef TEXTURE_H
#define TEXTURE_H

#include "shader.h"

class Texture {
    public:
	unsigned int id;
    bool alpha_enabled = false;
    Texture() = default;
    Texture(int id) : id(id){}
    Texture(unsigned char* data, int width, int height, int n_channels, int wrap_option, int sample_option);
    void Bind(Shader* shader,int offset = 0, const std::string& name = "");
};

#endif