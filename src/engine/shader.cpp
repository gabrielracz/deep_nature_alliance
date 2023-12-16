#include "shader.h"
#include "light.h"
#include <GL/glext.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "defines.h"

Shader::Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path, bool instanced) {
	vert_path = vertex_path;
	frag_path = fragment_path;
    geom_path = geometry_path;
    memset(lightsblock.lights, 0, MAX_LIGHTS * sizeof(ShaderLight));
    Load();
    SetupLighting();
    if(instanced) {
        SetupInstancing();
    }
}

static bool read_file(std::string path, std::string& dest) {
	std::ifstream file_stream;
	file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		file_stream.open(path);
        if(!file_stream.good()) {
            std::cout << "error on file " << path <<  " open" << std::endl;
        }
		std::stringstream read_stream;
		read_stream << file_stream.rdbuf();
		file_stream.close();

		dest = read_stream.str();
	}
	catch (std::exception& e) {
		std::cout << "[ERROR][SHADER] file read error " << path << " " << e.what() << std::endl;
		return false;
	}
    return true;
}

static int compile_shader(std::string code, GLuint type, std::string path) {
	int success = 0;
	char infolog[512];
    int shader_id = 0;
    const char* source = code.c_str();
	shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader_id, 512, NULL, infolog);
		std::cout << "[ERROR][SHADER] shader compilation failed " << path << ":\n"
        << infolog << std::endl;
	}
    return shader_id;
}


bool Shader::Load() {
	//Read Files
	std::string vertex_code = "";
	std::string frag_code   = "";
	std::string geom_code   = "";

    read_file(vert_path, vertex_code);
    read_file(frag_path, frag_code);
    if(!geom_path.empty()) {
        read_file(geom_path, geom_code);
    }

	//Compile and link shaders
	unsigned int vertex_shader, frag_shader, geom_shader;

    vertex_shader = compile_shader(vertex_code, GL_VERTEX_SHADER, vert_path);
    frag_shader = compile_shader(frag_code, GL_FRAGMENT_SHADER, frag_path);
    if(!geom_code.empty()) {
        geom_shader = compile_shader(geom_code, GL_GEOMETRY_SHADER, geom_path);
    }

	id = glCreateProgram();
	glAttachShader(id, vertex_shader);
	glAttachShader(id, frag_shader);
    if(!geom_code.empty()) {
        glAttachShader(id, geom_shader);
    }
	glLinkProgram(id);

	int success;
	char infolog[512];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infolog);
		std::cout << "[ERROR][SHADER] program linking failed\n" << infolog << std::endl;
		return false;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);
    if(!geom_code.empty()) {
        glDeleteShader(geom_shader);
    }

	return true;
}

void Shader::SetupLighting() {
    glGenBuffers(1, &lights_ubo);
    GLuint bindingPoint = 0;  // Choose a suitable binding point
    GLuint lightsBlockIndex = glGetUniformBlockIndex(id, "LightsBlock");
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, lights_ubo);
    glUniformBlockBinding(id, lightsBlockIndex, bindingPoint);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightsBlock), nullptr, GL_STREAM_DRAW);
    SetUniform1i(0, "LightsBlock");
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

void Shader::SetLights(std::vector<std::shared_ptr<Light>>& world_lights) {
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
        if(view_point.z < 50.0f || !cull) {
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
