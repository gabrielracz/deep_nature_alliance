#include "texture.h"

Texture::Texture(unsigned char* data, int width, int height, int n_channels, int wrap_option, int sample_option) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample_option);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample_option);
	glGenerateMipmap(GL_TEXTURE_2D);
    gl_texture_type = GL_TEXTURE_2D;
}

Texture::Texture(unsigned char* data[6], int width, int height, int n_channels, int wrap_option, int sample_option) {
    GLenum format = (n_channels == 4) ? GL_RGBA : GL_RGB;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for (int i = 0; i < 6; i++){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data[i]);
    }
    
	//Wrapping
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_option);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_option);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_option);
    //Filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sample_option);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sample_option);
    gl_texture_type = GL_TEXTURE_CUBE_MAP;
}

void Texture::Bind(Shader* shader, int offset, const std::string& name) {
    if(!name.empty()) {
        shader->SetUniform1i(offset, name);
    }
    glActiveTexture(GL_TEXTURE0 + offset);
    glBindTexture(gl_texture_type, id);
}