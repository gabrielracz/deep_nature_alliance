#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "path_config.h"
#include "transform.h"

#define STR_IMPL(A) #A
#define STR(A) STR_IMPL(A)
#define SHADER_COMMON common.glsl
#define SHADER_LIB_FILE STR(SHADER_DIRECTORY_TOKEN/SHADER_COMMON)


static const int MAX_LIGHTS    = 3;
static const int MAX_INSTANCES = 256;

// make sure these fields are alligned on 4 byte boundary
struct ShaderLight {
    glm::vec3 light_position = {0.0, 0.0, 0.0};
    float ambient_strength   = 0.2;
    glm::vec4 color          = {1.0, 1.0, 1.0, 1.0};
    glm::vec4 ambient_color  = {1.0, 1.0, 1.0, 1.0};
    glm::vec3 direction;
    float spread;
};

struct ShaderTransform {
    glm::mat4 transformation;
};

struct TransformsBlock {
    ShaderTransform transforms[MAX_INSTANCES];
};

struct LightsBlock {
    ShaderLight lights[MAX_LIGHTS];
};

class Light;

class Shader {

static const char* shader_lib;
public:

    LightsBlock lightsblock;
    TransformsBlock* transformsblock;
	unsigned int id;
	Shader(const char* vertex_path, const char* frag_path, bool instanced = false);
	Shader() = default;
	~Shader() = default;
	bool Load();
	void Reload();
	void Use() const;
    void Finalize(int num_lights);

    void SetupInstancing();

    void SetLights(std::vector<Light*>& lights);
    void SetInstances(std::vector<Transform>& transforms);
    
	void SetUniform1f(float u, const std::string& name);
	void SetUniform3f(const glm::vec3& u, const std::string& name);
	void SetUniform4f(const glm::vec4& u, const std::string& name);
	void SetUniform2m(const glm::mat3& u, const std::string& name);
	void SetUniform3m(const glm::mat3& u, const std::string& name);
	void SetUniform4m(const glm::mat4& u, const std::string& name);

    void SetUniform1i(int i, const std::string& name);
    void SetUniform1iv(int* v, int len, const std::string& name);

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

private:
	std::string vert_path;
	std::string frag_path;

    unsigned int lights_ubo;
    unsigned int instanced_ubo;


};

#endif
