#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

#include "defines.h"
#include "mesh.h"
#include "resource.h"
#include "resource_manager.h"

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// template <typename T>
// static inline void APPEND_VEC3(std::vector<T>& vector, const glm::vec3& glm_vec3) {
//     vector.insert(vector.end(), {(T)glm_vec3.x, (T)glm_vec3.y, (T)glm_vec3.z});
// }

const Layout generator_layout = Layout(
	{{FLOAT3, "vertex"},{FLOAT3, "normal"}, {FLOAT3, "color"}, {FLOAT2, "uv"}});


void ResourceManager::SetScreenSpaceShader(const std::string& name) {
	screenSpaceShader = name;
}

Shader* ResourceManager::GetScreenSpaceShader(){
	return GetShader(screenSpaceShader);
}

void ResourceManager::LoadShader(const std::string& name, const std::string& vert_path, const std::string& frag_path, const std::string& geom_path, bool instanced) {
    overwrite_emplace(shaders, name, Shader(vert_path.c_str(), frag_path.c_str(), geom_path.c_str(), instanced));
}

void ResourceManager::LoadMesh(const std::string& name, const std::string& path) {
	overwrite_emplace(meshes, name, Mesh(path));
}

void ResourceManager::AddMesh(const std::string& name, std::vector<float> verts, std::vector<unsigned int> inds, Layout layout) {
	overwrite_emplace(meshes, name, Mesh(verts, inds, layout));
}

Shader* ResourceManager::GetShader(const std::string &name) {
	auto it = shaders.find(name);
	if(it == shaders.end()) {
		std::cout << "RESMAN ERROR: loading shader\t" << name << std::endl;
		return nullptr;
	}
	return &it->second;
}

Mesh* ResourceManager::GetMesh(const std::string &name) {
	auto it = meshes.find(name);
	if(it == meshes.end()) {
        if(name != "") {
            std::cout << "RESMAN ERROR: loading mesh\t" << name << std::endl;
        }
		return nullptr;
	}
	return &it->second;
}

Texture* ResourceManager::GetTexture(const std::string &name) {
	auto it = textures.find(name);
	if(it == textures.end()) {
		std::cout << "RESMAN ERROR: loading texture\t" << name << std::endl;
		return nullptr;
	}
	return &it->second;
}

void ResourceManager::LoadTexture(const std::string& name, const std::string& file_path, int wrap_option, int sample_option) {
 	stbi_set_flip_vertically_on_load(1);
	//Texture
	int width, height, n_channels;
	unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &n_channels, 0);
	if (!data) {
		printf("ERROR: failed to load image %s\n", file_path.c_str());
		return;
	}

	overwrite_emplace(textures, name, Texture(data, width, height, n_channels, wrap_option, sample_option));
	stbi_image_free(data);

}

void ResourceManager::LoadCubemap(const std::string &name, const std::string &dir_path, bool legacyLoading) {
    std::vector<std::string> faces = {
        dir_path + "/right.png",
        dir_path + "/left.png",
        legacyLoading ? dir_path + "/bottom.png" : dir_path + "/top.png",
        legacyLoading ? dir_path + "/top.png" : dir_path + "/bottom.png",
        dir_path + "/front.png",
        dir_path + "/back.png"
    };

    stbi_set_flip_vertically_on_load(legacyLoading ? 1 : 0);

    int w, h, n_channels;
    unsigned char* data[6];
    for(int i = 0; i < faces.size(); i++) {
        data[i] = stbi_load(faces[i].c_str(), &w, &h, &n_channels, 0);
        if(!data[i]) {
            std::cout << "RESMAN ERROR Cube map face " << faces[i] << " failed to load" << std::endl;
        }
    }

    overwrite_emplace(textures, name, Texture(data, w, h, n_channels, GL_CLAMP_TO_EDGE, GL_LINEAR));
    
    for(int i = 0; i < faces.size(); i++) {
        stbi_image_free(data[i]); // Free individual face data
    }
}


// Create the geometry for a cylinder
void ResourceManager::CreateCylinder(std::string object_name, float height, float circle_radius, int num_height_samples, int num_circle_samples) {

	// Create a cylinder

	// Number of vertices and faces to be created
	const GLuint vertex_num = num_height_samples * num_circle_samples + 2; // plus two for top and bottom
	const GLuint face_num = num_height_samples * (num_circle_samples - 1) * 2 + 2 * num_circle_samples; // two extra rings worth for top and bottom

	// Number of attributes for vertices and faces
	const int vertex_att = 11;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
	const int face_att = 3; // Vertex indices (3)

	// Data buffers for the shape
	GLfloat* vertex = NULL;
	GLuint* face = NULL;

	GLuint vbo, ebo;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att];
		face = new GLuint[face_num * face_att];
	}
	catch (std::exception& e) {
		throw e;
	}

	// Create vertices 
	float theta; // Angle for circle
	float h; // height
	float s, t; // parameters zero to one
	glm::vec3 loop_center;
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;

	for (int i = 0; i < num_height_samples; i++) { // along the side

		s = i / (float)num_height_samples; // parameter s (vertical)
		h = (-0.5 + s) * height;
		for (int j = 0; j < num_circle_samples; j++) { // small circle
			t = j / (float)num_circle_samples;
			theta = 2.0 * glm::pi<GLfloat>() * t; // circle sample (angle theta)

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta), 0.0f, sin(theta));
			vertex_position = glm::vec3(cos(theta) * circle_radius, h, sin(theta) * circle_radius);
			vertex_color = glm::vec3(1.0 - s,
				t,
				s);
			vertex_coord = glm::vec2(s, t);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i * num_circle_samples + j) * vertex_att + k] = vertex_position[k];
				vertex[(i * num_circle_samples + j) * vertex_att + k + 3] = vertex_normal[k];
				vertex[(i * num_circle_samples + j) * vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i * num_circle_samples + j) * vertex_att + 9] = vertex_coord[0];
			vertex[(i * num_circle_samples + j) * vertex_att + 10] = vertex_coord[1];
		}
	}

	int topvertex = num_circle_samples * num_height_samples;
	int bottomvertex = num_circle_samples * num_height_samples + 1; // indices for top and bottom vertex

	vertex_position = glm::vec3(0, height * (num_height_samples - 1) / (float)num_height_samples - height * 0.5, 0); // location of top middle of cylinder
	vertex_normal = glm::vec3(0, 1, 0);
	vertex_color = glm::vec3(1, 0.6, 0.4);
	vertex_coord = glm::vec2(0, 0); // no good way to texture top and bottom

	for (int k = 0; k < 3; k++) {
		vertex[topvertex * vertex_att + k] = vertex_position[k];
		vertex[topvertex * vertex_att + k + 3] = vertex_normal[k];
		vertex[topvertex * vertex_att + k + 6] = vertex_color[k];
	}
	vertex[(topvertex)*vertex_att + 9] = vertex_coord[0];
	vertex[(topvertex)*vertex_att + 10] = vertex_coord[1];

	//================== bottom vertex

	vertex_position = glm::vec3(0, -0.5 * height, 0); // location of top middle of cylinder
	vertex_normal = glm::vec3(0, -1, 0);
	// leave the color and uv alone

	for (int k = 0; k < 3; k++) {
		vertex[bottomvertex * vertex_att + k] = vertex_position[k];
		vertex[bottomvertex * vertex_att + k + 3] = vertex_normal[k];
		vertex[bottomvertex * vertex_att + k + 6] = vertex_color[k];
	}
	vertex[(bottomvertex)*vertex_att + 9] = vertex_coord[0];
	vertex[(bottomvertex)*vertex_att + 10] = vertex_coord[1];

	//===================== end of vertices

	// Create triangles
	for (int i = 0; i < num_height_samples - 1; i++) {
		for (int j = 0; j < num_circle_samples; j++) {
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_height_samples) * num_circle_samples + j,
				i * num_circle_samples + ((j + 1) % num_circle_samples),
				i * num_circle_samples + j);
			glm::vec3 t2(((i + 1) % num_height_samples) * num_circle_samples + j,
				((i + 1) % num_height_samples) * num_circle_samples + ((j + 1) % num_circle_samples),
				i * num_circle_samples + ((j + 1) % num_circle_samples));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(i * num_circle_samples + j) * face_att * 2 + k] = (GLuint)t1[k];
				face[(i * num_circle_samples + j) * face_att * 2 + k + face_att] = (GLuint)t2[k];
			}
		}
	}
	int cylbodysize = num_circle_samples * (num_height_samples - 1) * 2; // amount of array filled so far, start adding from here
	// triangles for top disc (fan shape)
	int i = num_height_samples - 1;
	for (int j = 0; j < num_circle_samples; j++) {
		// Bunch of wedges pointing to the centre
		glm::vec3 topwedge(
			i * num_circle_samples + j,
			topvertex,
			i * num_circle_samples + (j + 1) % num_circle_samples
		);

		// note order reversed so that all triangles point outward
		glm::vec3 botwedge(
			0 + (j + 1) % num_circle_samples,
			bottomvertex,
			0 + j
		);

		// Add the triangles to the data buffer
		for (int k = 0; k < 3; k++) {
			face[(cylbodysize + j) * face_att + k] = (GLuint)topwedge[k];
			face[(cylbodysize + j + num_circle_samples) * face_att + k] = (GLuint)botwedge[k];
		}
	}

	// Create resource
	overwrite_emplace(meshes, object_name, Mesh(vertex, vertex_num * vertex_att, face, face_num * face_att, generator_layout));

	delete[] vertex;
	delete[] face;
}


// Create the geometry for a cone
void ResourceManager::CreateCone(std::string object_name, float height, float base_radius, int num_height_samples, int num_circle_samples, glm::vec4 color) {

	// Create a cone
	// The cone is built from a straight line with circles of increasing radius.
	// Vertices are sampled alon<glm/gtx/rotate_vector.hpp>g the circles

	// Number of attributes for vertices and faces
	const int vertex_att = 11;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
	const int face_att = 3; // Vertex indices (3)

	// Data buffers for the torus
	std::vector<GLuint> faces;
	std::vector<GLfloat> vertices;

	// how the height and radius increases as we progress through the sampling steps
	float y_step = height / (num_height_samples - 1);
	float radius_step = base_radius / (num_height_samples-1);

	// define a single point at the tip of the cone
	glm::vec3 tip_pos {0.0f, height/2, 0.0f};

	// generate the layers of the cone (excluding the tip)
	for(int i = 1; i < num_height_samples; i++) {
		float y = height/2 - y_step*i;
		float radius = radius_step * i;

        for(int j = 0; j < num_circle_samples; j++) {
            float phi = (glm::pi<float>()*2.0f) * (j / (float)num_circle_samples); // quantize the sampling of our cirlce
            glm::vec3 pos {cos(phi)*radius, y, sin(phi)*radius}; // parametric equation of a circle

            // get the tangent of our circle.
            glm::vec3 tangent = glm::rotate(glm::normalize(pos), -glm::pi<float>()/2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            // get a line perpendicular to the plane created by the cone's tip and the tangent of the circle we are sampling
            glm::vec3 norm = glm::vec3(cos(phi), 0.0f, sin(phi));

			if(color.a == 0.0f) {
				color =  {
					1.0 - ((float)i / (float)num_height_samples),
					(float)i / (float)num_height_samples,
					(float)j / (float)num_circle_samples,
					1.0f
				};
			}

			glm::vec2 uv {
				phi/(2.0f*glm::pi<float>()),
				(y+height/2.0f)/height
			};


			APPEND_VEC3(vertices, pos);
			APPEND_VEC3(vertices, norm);
			APPEND_VEC3(vertices, color);
			APPEND_VEC2(vertices, uv);
	   }
	}

	// Create triangles
	// t1:   t2:
	// 0      0--2
	// |\      \ |
	// | \      \|
	// 1--2      1
	// make sure to wrap around by one to close the ring
	for (int i = 0; i < num_height_samples - 2; i++) {
		for (int j = 0; j < num_circle_samples; j++) {
			glm::uvec3 t1 {
				num_circle_samples*i + j,
				num_circle_samples*(i+1) + j, 
				num_circle_samples*(i+1) + ((j+1) % (num_circle_samples)),
			};

			glm::uvec3 t2 {
				num_circle_samples*i + j,
				num_circle_samples*(i+1) + ((j+1) % (num_circle_samples)),
				num_circle_samples*i + ((j+1) % (num_circle_samples)),
			};

			APPEND_VEC3(faces, t1);
			APPEND_VEC3(faces, t2);
		}
	}

	// append the single bottom cap vertex
	unsigned int cap_index = vertices.size() / vertex_att;
	APPEND_VEC3(vertices, glm::vec3(0.0f, -height/2.0f, 0.0f));
	APPEND_VEC3(vertices, glm::vec3(0.0f, 0.0f, 1.0f));
	APPEND_VEC3(vertices, glm::vec3(0.0f, 1.0f, 1.0f));
	APPEND_VEC2(vertices, glm::vec2(1.0f, 1.0f));

	// attach the last vertex ring to this center cap to create a solid bottom
	for(int j = 0; j < num_circle_samples; j++) {
		glm::uvec3 t(
			cap_index,
			((num_height_samples-2)*num_circle_samples + (j+1) % num_circle_samples),
			(num_height_samples-2)*num_circle_samples + j
		);

		APPEND_VEC3(faces, t);
	}

	// tip's norm points straight "upwards", however we construct lying down on the z axis
	glm::vec3 tip_norm {0.0f, 0.0f, -1.0f};
	glm::vec3 tip_color { 1.0f, 0.0f, 0.0f};

	// append the tip vertex
	APPEND_VEC3(vertices, tip_pos);
	APPEND_VEC3(vertices, tip_norm);
	APPEND_VEC3(vertices, tip_color);
	APPEND_VEC2(vertices, glm::vec2(0.0f, 0.0f));


	// attach the first ring to this tip
	for(int i = 0; i < num_circle_samples; i++) {
		glm::uvec3 t {
			(vertices.size() - 1) / vertex_att,
			i,
			(i+1) % num_circle_samples
		};
		APPEND_VEC3(faces, t);
	}


	overwrite_emplace(meshes, object_name, Mesh(vertices, faces, generator_layout));
}

   void ResourceManager::CreateCone2(std::string object_name, float height, float circle_radius, int num_height_samples, int num_circle_samples)
    {
        // Number of attributes for vertices and faces
        const int vertex_att = 11; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        const int face_att = 3;    // Vertex indices (3)

        // Data buffers for the shape
        std::vector<GLuint> faces;
        std::vector<GLfloat> vertices;

        // Create vertices
        float theta;        // Angle for circle
        float local_radius; // radius of this spot on the cone
        float h;            // height
        float s, t;         // parameters zero to one
        glm::vec3 loop_center;
        glm::vec3 vertex_position;
        glm::vec3 vertex_normal;
        glm::vec3 vertex_color;
        glm::vec2 vertex_coord;

        glm::vec3 tip_pos {0.0f, height/2, 0.0f};

        for (int i = 0; i < num_height_samples; i++)
        { // along the side

            s = i / (float)num_height_samples; // parameter s (vertical)
            h = (-0.5 + s) * height;
            for (int j = 0; j < num_circle_samples; j++)
            { // small circle
                t = j / (float)num_circle_samples;
                theta = 2.0 * glm::pi<GLfloat>() * t; // circle sample (angle theta)

                // Define position, normal and color of vertex
                vertex_normal = glm::cross(vertex_position, tip_pos);
                local_radius = circle_radius * (num_height_samples - i) / num_height_samples; // fraction of the way along
                vertex_position = glm::vec3(cos(theta) * local_radius, h, sin(theta) * local_radius);
                vertex_color = glm::vec3(1.0 - s,t,s);
                vertex_coord = glm::vec2(s, t);
    			vertex_coord = glm::vec2(s,fabs(1-2*t)); // made seamless through mirroring

                APPEND_VEC3(vertices, vertex_position);
                APPEND_VEC3(vertices, vertex_normal);
                APPEND_VEC3(vertices, vertex_color);
                APPEND_VEC2(vertices, vertex_coord);
            }
        }

        int topvertex = num_circle_samples * num_height_samples;
        int bottomvertex = num_circle_samples * num_height_samples + 1; // indices for top and bottom vertex

        vertex_position = glm::vec3(0, height * (num_height_samples) / (float)num_height_samples - height * 0.5, 0); // location of tip
        vertex_normal = glm::vec3(0, 1, 0);
        vertex_color = glm::vec3(1, 1, 1);
        vertex_coord = glm::vec2(0, 0); // no good way to texture top and bottom

        APPEND_VEC3(vertices, vertex_position);
        APPEND_VEC3(vertices, vertex_normal);
        APPEND_VEC3(vertices, vertex_color);
        APPEND_VEC2(vertices, vertex_coord);

        //================== bottom vertex

        vertex_position = glm::vec3(0, -0.5 * height, 0); // location of middle of base of cone
        vertex_normal = glm::vec3(0, -1, 0);
        // leave the color and uv alone

        APPEND_VEC3(vertices, vertex_position);
        APPEND_VEC3(vertices, vertex_normal);
        APPEND_VEC3(vertices, vertex_color);
        APPEND_VEC2(vertices, vertex_coord);

        //===================== end of vertices

        // Create triangles
        for (int i = 0; i < num_height_samples - 1; i++)
        {
            for (int j = 0; j < num_circle_samples; j++)
            {
                // Two triangles per quad
                glm::uvec3 t1(((i + 1) % num_height_samples) * num_circle_samples + j,
                             i * num_circle_samples + ((j + 1) % num_circle_samples),
                             i * num_circle_samples + j);
                glm::uvec3 t2(((i + 1) % num_height_samples) * num_circle_samples + j,
                             ((i + 1) % num_height_samples) * num_circle_samples + ((j + 1) % num_circle_samples),
                             i * num_circle_samples + ((j + 1) % num_circle_samples));
                APPEND_VEC3(faces, t1);
                APPEND_VEC3(faces, t2);
            }
        }
        int cylbodysize = num_circle_samples * (num_height_samples - 1) * 2; // amount of array filled so far, start adding from here
        // triangles for top disc (fan shape)
        int i = num_height_samples - 1;
        for (int j = 0; j < num_circle_samples; j++)
        {
            // Bunch of wedges pointing to the centre
            glm::uvec3 topwedge(
                i * num_circle_samples + j,
                topvertex,
                i * num_circle_samples + ((j + 1) % num_circle_samples));

            // note order reversed so that all triangles point outward
            glm::uvec3 botwedge(
                0 + (j + 1) % num_circle_samples,
                bottomvertex,
                0 + j);

            APPEND_VEC3(faces, topwedge);
            APPEND_VEC3(faces, botwedge);
        }

        overwrite_emplace(meshes, object_name, Mesh(vertices, faces, generator_layout));
    }

/*
void ResourceManager::CreateCone(std::string object_name, float height, float circle_radius, int num_height_samples, int num_circle_samples) {

	// Create a cone (adapted from cylinder code)

	// Number of vertices and faces to be created
	const GLuint vertex_num = num_height_samples * num_circle_samples + 2; // plus two for top and bottom
	const GLuint face_num = num_height_samples * (num_circle_samples - 1) * 2 + 2 * num_circle_samples; // two extra rings worth for top and bottom

	// Number of attributes for vertices and faces
	const int vertex_att = 11;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
	const int face_att = 3; // Vertex indices (3)

	// Data buffers for the shape
	GLfloat* vertex = NULL;
	GLuint* face = NULL;

	GLuint vbo, ebo;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att];
		face = new GLuint[face_num * face_att];
	}
	catch (std::exception& e) {
		throw e;
	}

	// Create vertices 
	float theta; // Angle for circle
	float local_radius; // radius of this spot on the cone
	float h; // height
	float s, t; // parameters zero to one
	glm::vec3 loop_center;
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;

	for (int i = 0; i < num_height_samples; i++) { // along the side

		s = i / (float)num_height_samples; // parameter s (vertical)
		h = (-0.5 + s) * height;
		for (int j = 0; j < num_circle_samples; j++) { // small circle
			t = j / (float)num_circle_samples;
			theta = 2.0 * glm::pi<GLfloat>() * t; // circle sample (angle theta)

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta), 0.0f, sin(theta));
			local_radius = circle_radius * (num_height_samples - i) / num_height_samples; // fraction of the way along 
			vertex_position = glm::vec3(cos(theta) * local_radius, h, sin(theta) * local_radius);
			vertex_color = glm::vec3(1.0 - s,
				t,
				s);
			vertex_coord = glm::vec2(s, t);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i * num_circle_samples + j) * vertex_att + k] = vertex_position[k];
				vertex[(i * num_circle_samples + j) * vertex_att + k + 3] = vertex_normal[k];
				vertex[(i * num_circle_samples + j) * vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i * num_circle_samples + j) * vertex_att + 9] = vertex_coord[0];
			vertex[(i * num_circle_samples + j) * vertex_att + 10] = vertex_coord[1];
		}
	}

	int topvertex = num_circle_samples * num_height_samples;
	int bottomvertex = num_circle_samples * num_height_samples + 1; // indices for top and bottom vertex

	vertex_position = glm::vec3(0, height * (num_height_samples) / (float)num_height_samples - height * 0.5, 0); // location of tip
	vertex_normal = glm::vec3(0, 1, 0);
	vertex_color = glm::vec3(1, 0.6, 0.4);
	vertex_coord = glm::vec2(0, 0); // no good way to texture top and bottom

	for (int k = 0; k < 3; k++) {
		vertex[topvertex * vertex_att + k] = vertex_position[k];
		vertex[topvertex * vertex_att + k + 3] = vertex_normal[k];
		vertex[topvertex * vertex_att + k + 6] = vertex_color[k];
	}
	vertex[(topvertex)*vertex_att + 9] = vertex_coord[0];
	vertex[(topvertex)*vertex_att + 10] = vertex_coord[1];

	//================== bottom vertex

	vertex_position = glm::vec3(0, -0.5 * height, 0); // location of middle of base of cone
	vertex_normal = glm::vec3(0, -1, 0);
	// leave the color and uv alone

	for (int k = 0; k < 3; k++) {
		vertex[bottomvertex * vertex_att + k] = vertex_position[k];
		vertex[bottomvertex * vertex_att + k + 3] = vertex_normal[k];
		vertex[bottomvertex * vertex_att + k + 6] = vertex_color[k];
	}
	vertex[(bottomvertex)*vertex_att + 9] = vertex_coord[0];
	vertex[(bottomvertex)*vertex_att + 10] = vertex_coord[1];

	//===================== end of vertices

	// Create triangles
	for (int i = 0; i < num_height_samples - 1; i++) {
		for (int j = 0; j < num_circle_samples; j++) {
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_height_samples) * num_circle_samples + j,
				i * num_circle_samples + ((j + 1) % num_circle_samples),
				i * num_circle_samples + j);
			glm::vec3 t2(((i + 1) % num_height_samples) * num_circle_samples + j,
				((i + 1) % num_height_samples) * num_circle_samples + ((j + 1) % num_circle_samples),
				i * num_circle_samples + ((j + 1) % num_circle_samples));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(i * num_circle_samples + j) * face_att * 2 + k] = (GLuint)t1[k];
				face[(i * num_circle_samples + j) * face_att * 2 + k + face_att] = (GLuint)t2[k];
			}
		}
	}
	int cylbodysize = num_circle_samples * (num_height_samples - 1) * 2; // amount of array filled so far, start adding from here
	// triangles for top disc (fan shape)
	int i = num_height_samples - 1;
	for (int j = 0; j < num_circle_samples; j++) {
		// Bunch of wedges pointing to the centre
		glm::vec3 topwedge(
			i * num_circle_samples + j,
			topvertex,
			i * num_circle_samples + (j + 1) % num_circle_samples
		);

		// note order reversed so that all triangles point outward
		glm::vec3 botwedge(
			0 + (j + 1) % num_circle_samples,
			bottomvertex,
			0 + j
		);

		// Add the triangles to the data buffer
		for (int k = 0; k < 3; k++) {
			face[(cylbodysize + j) * face_att + k] = (GLuint)topwedge[k];
			face[(cylbodysize + j + num_circle_samples) * face_att + k] = (GLuint)botwedge[k];
		}
	}

	// Create OpenGL buffer for vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	// Create OpenGL buffer for faces
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Free data buffers
	delete[] vertex;
	delete[] face;


	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);

}
*/

void ResourceManager::CreateTorus(std::string object_name, float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples){

	// Create a torus
	// The torus is built from a large loop with small circles around the loop

	// Number of vertices and faces to be created
	// Check the construction algorithm below to understand the numbers
	// specified below
	const GLuint vertex_num = (num_loop_samples+1)*(num_circle_samples+1);
	const GLuint face_num = num_loop_samples*num_circle_samples*2;

	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	// Data buffers for the torus
	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
		face = new GLuint[face_num * face_att]; // 3 indices per face
	}
	catch  (std::exception &e){
		throw e;
	}

	// Create vertices 
	float theta, phi; // Angles for circles
	glm::vec3 loop_center;
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;
	float s, t;
	for (int i = 0; i < num_loop_samples+1; i++){ // large loop
		
		theta = 2.0*glm::pi<GLfloat>()*i/num_loop_samples; // loop sample (angle theta)
		loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

		for (int j = 0; j < num_circle_samples+1; j++){ // small circle
			
			phi = 2.0*glm::pi<GLfloat>()*j/num_circle_samples; // circle sample (angle phi)
			s = theta / (2.0*glm::pi<GLfloat>());
			t = ((phi) / (2.0*glm::pi<GLfloat>()));

			phi += glm::pi<GLfloat>();

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
			vertex_position = loop_center + vertex_normal*circle_radius;
			vertex_color = glm::vec3(1.0 - ((float) i / (float) num_loop_samples), 
											(float) i / (float) num_loop_samples, 
											(float) j / (float) num_circle_samples);
		
			vertex_coord = glm::vec2(s, t); // good parameterization but seam at last triangle
//			vertex_coord = glm::vec2(fabs(1-2*s),fabs(1-2*t)); // made seamless through mirroring
//			vertex_coord = glm::vec2((rand() % 2000) / 2000.0, (rand() % 2000) / 2000.0);
			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++){
				vertex[(i*(num_circle_samples+1)+j)*vertex_att + k] = vertex_position[k];
				vertex[(i*(num_circle_samples+1)+j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*(num_circle_samples+1)+j)*vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i*(num_circle_samples+1)+j)*vertex_att + 9] = vertex_coord[0];
			vertex[(i*(num_circle_samples+1)+j)*vertex_att + 10] = vertex_coord[1];
		}
	}

	// Create triangles
	for (int i = 0; i < num_loop_samples; i++){
		for (int j = 0; j < num_circle_samples; j++){
			// Two triangles per quad
			glm::vec3 t1((i + 1)*(num_circle_samples + 1) + j,
				i*(num_circle_samples + 1) + (j + 1),
				i*(num_circle_samples + 1) + j);
			glm::vec3 t2((i + 1)*(num_circle_samples + 1) + j,
				(i + 1)*(num_circle_samples + 1) + (j + 1),
				i*(num_circle_samples + 1) + (j + 1));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++){
				face[(i*(num_circle_samples)+j)*face_att*2 + k] = (GLuint) t1[k];
				face[(i*(num_circle_samples)+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
			}
		}
	}

	overwrite_emplace(meshes, object_name, Mesh(vertex, vertex_num * vertex_att, face, face_num * face_att, generator_layout));

	delete [] vertex;
	delete [] face;
}



void ResourceManager::CreateSphere(std::string object_name, float radius, int num_samples_theta, int num_samples_phi){

	// Create a sphere using a well-known parameterization

	// Number of vertices and faces to be created
	const GLuint vertex_num = num_samples_theta*num_samples_phi;
	const GLuint face_num = num_samples_theta*(num_samples_phi-1)*2;

	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	// Data buffers 
	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
		face = new GLuint[face_num * face_att]; // 3 indices per face
	}
	catch  (std::exception &e){
		throw e;
	}

	// Create vertices 
	float theta, phi; // Angles for parametric equation
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;
   
	for (int i = 0; i < num_samples_theta; i++){
			
		theta = 2.0*glm::pi<GLfloat>()*i/(num_samples_theta-1); // angle theta
			
		for (int j = 0; j < num_samples_phi; j++){
					
			phi = glm::pi<GLfloat>()*j/(num_samples_phi-1); // angle phi

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta)*sin(phi), sin(theta)*sin(phi), -cos(phi));
			// We need z = -cos(phi) to make sure that the z coordinate runs from -1 to 1 as phi runs from 0 to pi
			// Otherwise, the normal will be inverted
			vertex_position = glm::vec3(vertex_normal.x*radius, 
										vertex_normal.y*radius, 
										vertex_normal.z*radius),
			vertex_color = glm::vec3(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi), ((float)j)/((float)num_samples_phi));
			// vertex_color = {1.0f, 1.0f, 0.0f};
			vertex_coord = glm::vec2(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi));

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++){
				vertex[(i*num_samples_phi+j)*vertex_att + k] = vertex_position[k];
				vertex[(i*num_samples_phi+j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*num_samples_phi+j)*vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i*num_samples_phi+j)*vertex_att + 9] = vertex_coord[0];
			vertex[(i*num_samples_phi+j)*vertex_att + 10] = vertex_coord[1];
			// vertex[(i*num_samples_phi+j)*vertex_att + 9] = theta/num_samples_theta;
			// vertex[(i*num_samples_phi+j)*vertex_att + 10] = phi/num_samples_phi;
		}
	}

	// Create faces
	for (int i = 0; i < num_samples_theta; i++){
		for (int j = 0; j < (num_samples_phi-1); j++){
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_samples_theta)*num_samples_phi + j, 
						 i*num_samples_phi + (j + 1),
						 i*num_samples_phi + j);
			glm::vec3 t2(((i + 1) % num_samples_theta)*num_samples_phi + j, 
						 ((i + 1) % num_samples_theta)*num_samples_phi + (j + 1), 
						 i*num_samples_phi + (j + 1));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++){
				face[(i*(num_samples_phi-1)+j)*face_att*2 + k] = (GLuint) t1[k];
				face[(i*(num_samples_phi-1)+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
			}
		}
	}

	overwrite_emplace(meshes, object_name, Mesh(vertex, vertex_num * vertex_att, face, face_num * face_att, generator_layout));

	delete [] vertex;
	delete [] face;
}

void ResourceManager::CreatePointCloud(std::string object_name, int num_points, float size, glm::vec4 col) {
	std::vector<float> vertices;
	std::vector<unsigned int> inds;
	for(int i = 0 ; i < num_points; i++ ) {
		glm::vec3 pos = glm::ballRand(size);
		glm::vec3 color = {};
		if(col == glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) {
            float b = rand() % 6 == 0 ? 1.0 : 0.0;
			color = glm::vec4(i, b, 0.0, 0.0); // encode id in red channel
		}
		
		// hack to use the same shader as other objects (really only need pos and color)
		APPEND_VEC3(vertices, pos);
		APPEND_VEC3(vertices, glm::normalize(pos));
		APPEND_VEC3(vertices, color);
		APPEND_VEC2(vertices, glm::vec2(1.0, 1.0));
	}

	overwrite_emplace(meshes, object_name, Mesh(vertices, inds, generator_layout));
}

// Winter wonderland
// y value sets the height where to spawn the particles needless to say u want this above the player
void ResourceManager::CreateSnowParticles(std::string object_name, int num_particles, float spread_range, int density, float yposition){

    // Create a set of points which will be the particles
    // This is similar to drawing a sphere: we will sample points on a sphere, but will allow them to also deviate a bit from the sphere along the normal (change of radius)
	std::vector<float> vertices;
	std::vector<unsigned int> inds;

    int num_clusters = num_particles / density;

    // We want to cluster the snow flakes so they can be together like they have been deposited by some cloud
    for (int i = 0; i < num_clusters; i++) {
        // Pick random spot to put cluster of snow particles (think of this like a cloud)
        float x_center = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * spread_range - (spread_range / 2.0f);
        float z_center = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * spread_range - (spread_range / 2.0f);

        // Spawn particles in specified radius and spawn how many dense particles
        for (int j = 0; j < density; j++) {
            int index = i * density + j;
            
            float radius = 10; // just a good area to spawn the particles
            float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * glm::two_pi<float>();
            float x_offset = radius * cos(angle);
            float z_offset = radius * sin(angle);

            float x = x_center + x_offset;
            float z = z_center + z_offset;
            float y = yposition;

            glm::vec3 position(x, y, z);

            // Add position and color to the data buffer
            // Note: we randomize everything that is unused so the shader can use it
            // as a seed for its random function!
			glm::vec3 normals = glm::vec3(static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
			glm::vec3 colours = glm::vec3(static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
			APPEND_VEC3(vertices, position);
			APPEND_VEC3(vertices, normals);
			APPEND_VEC3(vertices, colours);
			APPEND_VEC2(vertices, glm::vec2(1.0, 1.0));
        }
    }

	overwrite_emplace(meshes, object_name, Mesh(vertices, inds, generator_layout));
}

void ResourceManager::CreateQuad(std::string name) {
	std::vector<float> vertices = {
		 1.0f,  1.0f, 0.0f,    0.0, 0.0, 1.0,    1.0, 0.0, 1.0,    1.0f, 1.0f,   1.0, 0.0, 0.0,
		 1.0f, -1.0f, 0.0f,    0.0, 0.0, 1.0,    1.0, 0.0, 1.0,    1.0f, 0.0f,   1.0, 0.0, 0.0,
		-1.0f, -1.0f, 0.0f,    0.0, 0.0, 1.0,    1.0, 0.0, 1.0,    0.0f, 0.0f,   1.0, 0.0, 0.0,
		-1.0f,  1.0f, 0.0f,    0.0, 0.0, 1.0,    1.0, 0.0, 1.0,    0.0f, 1.0f,   1.0, 0.0, 0.0
	};

	std::vector<unsigned int> indices = {
		3, 2, 1,
		3, 1, 0
	};

	Layout l = Layout({
		{FLOAT3, "vertex"},
		{FLOAT3, "normal"},
		{FLOAT3, "color"},
		{FLOAT2, "uv"},
		{FLOAT3, "tangent"}
	});
	
	overwrite_emplace(meshes, name, Mesh(vertices, indices, l));
}

void ResourceManager::CreateSimpleQuad(std::string name) {
	std::vector<float> vertices = {
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
	};

	std::vector<unsigned int> indices = {
		3, 2, 1,
		3, 1, 0
	};

	Layout l = Layout({
		{FLOAT3, "vertex"},
		{FLOAT2, "uv"}
	});
	
	overwrite_emplace(meshes, name, Mesh(vertices, indices, l));
}

void ResourceManager::CreateSaplingQuad(std::string name) {
    std::vector<float> vertices = {
        // Front Face
		 0.5f, -0.5f, 0.0f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  1.0f, 0.0f, // Bottom Right
         0.5f,  0.5f, 0.0f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  1.0f, 1.0f, // Top Right
		-0.5f, -0.5f, 0.0f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  0.0f, 1.0f, // Top Left

        // Back face
         0.5f,  0.5f, 0.0f,  0.0, 0.0, -1.0,  1.0, 1.0, 1.0,  0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  0.0, 0.0, -1.0,  1.0, 1.0, 1.0,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0, 0.0, -1.0,  1.0, 1.0, 1.0,  0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0, 0.0, -1.0,  1.0, 1.0, 1.0,  1.0f, 0.0f,

        0.0f, -0.5f, -0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  1.0f, 0.0f, // Bottom-left
        0.0f,  0.5f, -0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  1.0f, 1.0f, // Top-left
        0.0f, -0.5f,  0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  0.0f, 0.0f, // Bottom-right
        0.0f,  0.5f,  0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  0.0f, 1.0f,  // Top-right

        0.0f, -0.5f,  0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  1.0f, 0.0f, // Bottom-left
        0.0f,  0.5f,  0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  1.0f, 1.0f, // Top-left
        0.0f, -0.5f, -0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  0.0f, 0.0f, // Bottom-right
        0.0f,  0.5f, -0.5f,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0,  0.0f, 1.0f,  // Top-right
	};

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 1, 3,

        4, 5, 6,
        6, 5, 7,

        8, 9, 10,
        10, 9, 11,

        12, 13, 14,
        14, 13, 15
    };
    
    overwrite_emplace(meshes, name, Mesh(vertices, indices, generator_layout));
}

void ResourceManager::CreateSimpleCube(std::string object_name) {
const std::vector<float> cube_vertices = {
	// Vertices
	-0.5f, -0.5f, -0.5f,  // Vertex 0
	 0.5f, -0.5f, -0.5f,  // Vertex 1
	 0.5f,  0.5f, -0.5f,  // Vertex 2
	-0.5f,  0.5f, -0.5f,  // Vertex 3
	-0.5f, -0.5f,  0.5f,  // Vertex 4
	 0.5f, -0.5f,  0.5f,  // Vertex 5
	 0.5f,  0.5f,  0.5f,  // Vertex 6
	-0.5f,  0.5f,  0.5f   // Vertex 7
};

const std::vector<unsigned int> cube_indices = {
	// Indices
	0, 1, 2,  // Front face
	2, 3, 0,
	1, 5, 6,  // Right face
	6, 2, 1,
	7, 6, 5,  // Top face
	5, 4, 7,
	4, 0, 3,  // Left face
	3, 7, 4,
	4, 5, 1,  // Bottom face
	1, 0, 4,
	3, 2, 6,  // Back face
	6, 7, 3
};

	Layout layout({{FLOAT3, "position"}});
	overwrite_emplace(meshes, object_name, Mesh(cube_vertices, cube_indices, layout));
}

std::string ResourceManager::LoadTextFile(const char *filename){

	// Open file
	std::ifstream f;
	f.open(filename);
	if (f.fail()){
		throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
	}

	// Read file
	std::string content;
	std::string line;
	while(std::getline(f, line)){
		content += line + "\n";
	}

	// Close file
	f.close();

	return content;
}
