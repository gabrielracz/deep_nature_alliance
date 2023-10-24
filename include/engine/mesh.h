#ifndef MESH_H
#define MESH_H
#define GLM_FORCE_RADIANS
#include<glm/glm.hpp>
#include <initializer_list>
#include<string>
#include<vector>

#include "shader.h"

#define NAMELEN 32

struct Vertex {
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Texture {
	unsigned int id;
	char type[NAMELEN];
};

enum LAYOUT_TYPE {
	FLOAT1,
	FLOAT2,
	FLOAT3,
	FLOAT4,
	UINT,
	INT
};

class LayoutEntry {
public:
	 LAYOUT_TYPE type;
	 std::string name;

	 LayoutEntry() = default;
	 LayoutEntry(LAYOUT_TYPE t, std::string n);

	 size_t size();
	 unsigned int cnt();
	 unsigned int gltype();
};

class Layout {
public:
	std::vector<LayoutEntry> entries;
	size_t size;

	Layout() = default;
	Layout(std::initializer_list<LayoutEntry> ents);
};


//classic position, normal, texture 
static const Layout default_layout({
		{FLOAT3, "position"}, 
		{FLOAT3, "normal"},
		{FLOAT2, "uv"}
		});

class Mesh {
	public:
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		Layout layout;

        Mesh() = default;
        Mesh(const std::string& obj_file);
		Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds, std::vector<Texture> textures, Layout = default_layout);
		Mesh(const float* verts, size_t num_verts, const unsigned int* indices, size_t num_indices, const Texture* tex, unsigned int num_tex, Layout = default_layout);
		void Draw(Shader& shader);
		//void SetLayout(Layout& l);

	private:
		unsigned int VBO, EBO, VAO;
		void Setup();
		static size_t sz(LayoutEntry t);
		static unsigned int cnt(LayoutEntry t);
		static unsigned int gltype(LayoutEntry t);
};
#endif
