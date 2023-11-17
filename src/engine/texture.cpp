#include "texture.h"

Texture::Texture(unsigned char* data, int width, int height, int n_channels, int wrap_option, float repitition) : texture_repetition(repitition) {
    GLenum format;
    switch(n_channels) {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGB;
            break;
    }

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	//Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_option);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_option);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Bind(Shader* shader, int offset, const std::string& name) {
    if(!name.empty()) {
        // GLint loc = glGetUniformLocation(shader->id, name.c_str());
        // glUniform1i(loc, 0);
        shader->SetUniform1i(offset, name);

        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, id);

        return;
    }
    shader->SetUniform1f(texture_repetition, "texture_repetition");
    // glActiveTexture(GL_TEXTURE0 + offset);
    glBindTexture(GL_TEXTURE_2D, id);
}