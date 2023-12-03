#include "shader.h"
#include "light.h"
#include <GL/glext.h>
#include <exception>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "defines.h"

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path, bool instanced, const std::string& geometry_path) {
	vert_path = vertex_path;
	frag_path = fragment_path;
	geom_path = geometry_path;
    memset(lightsblock.lights, 0, MAX_LIGHTS * sizeof(ShaderLight));
    Load();
    if(instanced) {
        SetupInstancing();
    }
}

bool Shader::Load() {
	//Read Files
	std::string vertex_code;
	std::string frag_code;
	std::ifstream vshader_file;
	std::ifstream fshader_file;

	vshader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fshader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vshader_file.open(vert_path);
        if(!vshader_file.good()) {
            std::cout << "error on vertex shader file open" << std::endl;
        }
		fshader_file.open(frag_path);
        if(!fshader_file.good()) {
            std::cout << "error on fragment shader file open" << std::endl;
        }
		std::stringstream vshader_stream, fshader_stream;
		vshader_stream << vshader_file.rdbuf();
		fshader_stream << fshader_file.rdbuf();

		vshader_file.close();
		fshader_file.close();

		vertex_code = vshader_stream.str();
		frag_code = fshader_stream.str();
	}
	catch (std::exception& e) {
		std::cout << "[ERROR][SHADER] file read error"<< e.what() << std::endl;
		return false;
	}

	//Compile and link shaders
	const char* vertex_source = vertex_code.c_str();
	const char* frag_source = frag_code.c_str();

	unsigned int vertex_shader, frag_shader;
	int success;
	char infolog[512];

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, infolog);
		std::cout << "[ERROR][SHADER] vertex compilation failed " << vert_path << ":\n"
        << infolog << std::endl;
		return false;
	}

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_source, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, infolog);
		std::cout << "[ERROR][SHADER] fragment compilation failed " << frag_path << ":\n" << infolog << std::endl;
		return false;
	}

	id = glCreateProgram();
	glAttachShader(id, vertex_shader);
	glAttachShader(id, frag_shader);

	//geometry shader stuff, is seperate because it is optional
	std::string geom_code;
	std::ifstream gshader_file;

    if (!geom_path.empty()) {
        gshader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            gshader_file.open(geom_path);
            if (!gshader_file.good()) {
                std::cout << "error on geometry shader file open" << std::endl;
            }
            std::stringstream gshader_stream;
            gshader_stream << gshader_file.rdbuf();
            gshader_file.close();
            geom_code = gshader_stream.str();
        } catch (std::exception& e) {
            std::cout << "[ERROR][SHADER] file read error" << e.what() << std::endl;
            return false;
        }
    }

    unsigned int geom_shader;
    if (!geom_code.empty()) {
        const char* geom_source = geom_code.c_str();
        geom_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom_shader, 1, &geom_source, NULL);
        glCompileShader(geom_shader);
        glGetShaderiv(geom_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geom_shader, 512, NULL, infolog);
            std::cout << "[ERROR][SHADER] geometry compilation failed " << geom_path << ":\n"
                      << infolog << std::endl;
            return false;
        }
        glAttachShader(id, geom_shader);
    }

	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infolog);
		std::cout << "[ERROR][SHADER] program linking failed\n" << infolog << std::endl;
		return false;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);
	if (geom_shader) {
		glDeleteShader(geom_shader);
	}

    glGenBuffers(1, &lights_ubo);
    GLuint bindingPoint = 0;  // Choose a suitable binding point
    GLuint lightsBlockIndex = glGetUniformBlockIndex(id, "LightsBlock");
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, lights_ubo);
    glUniformBlockBinding(id, lightsBlockIndex, bindingPoint);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightsBlock), nullptr, GL_STREAM_DRAW);
    SetUniform1i(0, "LightsBlock");

    // Associate the buffer with the block index
	return true;
}


void Shader::SetupInstancing() {
    transformsblock = new TransformsBlock();
    memset(transformsblock, 0, sizeof(TransformsBlock));
    
    glGenBuffers(1, &instanced_ubo);
    GLuint bindingPoint = 1;  // Choose a suitable binding point
    GLuint block_index = glGetUniformBlockIndex(id, "TransformsBlock");
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, instanced_ubo);
    glUniformBlockBinding(id, block_index, bindingPoint);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(TransformsBlock), nullptr, GL_STREAM_DRAW);
    SetUniform1i(1, "TransformsBlock");
}

void Shader::Use() const{
	glUseProgram(id);
}

void Shader::SetLights(std::vector<Light*>& world_lights) {
    int i = 0;
    for(; i < MIN(world_lights.size(), MAX_LIGHTS); i++) {
        world_lights[i]->SetUniforms(lightsblock.lights[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, lights_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lights_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightsBlock), &lightsblock);
    SetUniform1i(i, "num_world_lights");
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int Shader::SetInstances(std::vector<Transform> &transforms, const glm::mat4& view_matrix, bool cull) {
    int i = 0;
    int j = 0;
    int rejected = 0;
    // Prototype for culling objects not in front of the camera. this should be in view
    for(; i < transforms.size() && j < MAX_INSTANCES; i++) {
        glm::mat4 t = transforms[i].GetLocalMatrix();
        glm::vec3 view_point = view_matrix * t * glm::vec4(0, 0, 0, 1);
        if(view_point.z < 10.0f || !cull) {
            transformsblock->transforms[j].transformation = t;
            transformsblock->transforms[j].normal_matrix = glm::transpose(glm::inverse(view_matrix * t));
            j++;
        } else {
            rejected++;
        }
    }

    glBindBuffer(GL_UNIFORM_BUFFER, instanced_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, instanced_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderTransform) * j, &transformsblock->transforms);
    // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TransformsBlock), &transformsblock);
    SetUniform1i(j, "num_instances");
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return j;
}

void Shader::Finalize(int num_lights) {
}

void Shader::SetUniform1f(float u, const std::string& name) {
	int location = glGetUniformLocation(id, name.c_str());
	glUniform1f(location, u);
}

void Shader::SetUniform3f(const glm::vec3& u, const std::string& name) {
	int location = glGetUniformLocation(id, name.c_str());
	glUniform3f(location, u.x, u.y, u.z);
}

void Shader::SetUniform4f(const glm::vec4& u, const std::string& name) {
	int location = glGetUniformLocation(id, name.c_str());
	glUniform4f(location, u.x, u.y, u.z, u.w);
}

void Shader::SetUniform4m(const glm::mat4& u, const std::string& name) {
	int location = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(u));
}

void Shader::SetUniform3m(const glm::mat3& u, const std::string& name) {
	int location = glGetUniformLocation(id, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(u));
}

void Shader::SetUniform2m(const glm::mat3& u, const std::string& name) {
	int location = glGetUniformLocation(id, name.c_str());
	glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(u));
}

void Shader::SetUniform1i(int u, const std::string& name) {
    int location = glGetUniformLocation(id, name.c_str());
    glUniform1i(location, u);
}

void Shader::SetUniform1iv(int *v, int len, const std::string &name) {
    int location = glGetUniformLocation(id, name.c_str());
    glUniform1iv(location, len, v);
}
