#include "shader.h"
#include "light.h"
#include <exception>
#include "defines.h"


Shader::Shader(const char* vertex_path, const char* fragment_path) {
	vert_path = vertex_path;
	frag_path = fragment_path;
    Load();
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
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infolog);
		std::cout << "[ERROR][SHADER] program linking failed\n" << infolog << std::endl;
		return false;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);

    GLuint uniformLocation = glGetUniformLocation(id, "world_lights");
    // Pass the entire array to the shader
    glUniformBlockBinding(id, uniformLocation, 0);  // Assuming binding point 0

    glGenBuffers(1, &ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
    glBufferData(GL_UNIFORM_BUFFER, MAX_LIGHTS * sizeof(ShaderLight), &lights[0], GL_STREAM_DRAW);
	return true;
}

void Shader::Use() const{
	glUseProgram(id);
}

void Shader::SetLights(std::vector<Light*>& world_lights) {
    int i = 0;
    for(; i < MIN(world_lights.size(), Shader::MAX_LIGHTS); i++) {
        world_lights[i]->SetUniforms(lights[i]);
    }
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light) * i, lights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    std::cout << sizeof(lights) << " " << i << std::endl;
    SetUniform1i(i, "num_world_lights");
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
