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

#include "mesh.h"
#include "resource_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define APPEND_VEC2(vector, glm_vec2) vector.insert(vector.end(), {glm_vec2.x, glm_vec2.y})
#define APPEND_VEC3(vector, glm_vec3) vector.insert(vector.end(), {glm_vec3.x, glm_vec3.y, glm_vec3.z})

// template <typename T>
// static inline void APPEND_VEC3(std::vector<T>& vector, const glm::vec3& glm_vec3) {
//     vector.insert(vector.end(), {(T)glm_vec3.x, (T)glm_vec3.y, (T)glm_vec3.z});
// }

const Layout generator_layout = Layout(
    {{FLOAT3, "vertex"},{FLOAT3, "normal"}, {FLOAT3, "color"}, {FLOAT2, "uv"}});

void ResourceManager::LoadShader(const std::string& name, const std::string& vert_path, const std::string& frag_path){
    shaders.emplace(name, Shader(vert_path.c_str(), frag_path.c_str()));
}

void ResourceManager::LoadMesh(const std::string& name, const std::string& path) {
    meshes.emplace(name, Mesh(path));
}

void ResourceManager::AddMesh(const std::string& name, std::vector<float> verts, std::vector<unsigned int> inds, Layout layout) {
    meshes.emplace(name, Mesh(verts, inds, layout));
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
        std::cout << "RESMAN ERROR: loading mesh\t" << name << std::endl;
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

void ResourceManager::LoadTexture(const std::string& name, const std::string& file_path, int wrap_option) {
 	stbi_set_flip_vertically_on_load(1);
	//Texture
	int width, height, n_channels;
	unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &n_channels, 0);
	if (!data) {
		printf("ERROR: failed to load image %s\n", file_path.c_str());
        return;
	}

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

	unsigned int tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	//Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_option);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_option);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

    textures.emplace(name, Texture(tex_id));
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
    meshes.emplace(object_name, Mesh(vertex, vertex_num * vertex_att, face, face_num * face_att, generator_layout));

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
            float phi = (glm::pi<float>()*2.0f) * j / num_circle_samples; // quantize the sampling of our cirlce
            glm::vec3 pos {cos(phi)*radius, y, sin(phi)*radius}; // parametric equation of a circle

            // get the tangent of our circle.
            glm::vec3 tangent = glm::rotate(glm::normalize(pos), -glm::pi<float>()/2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            // get a line perpendicular to the plane created by the cone's tip and the tangent of the circle we are sampling
            glm::vec3 norm = glm::cross(tip_pos, tangent);

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
    APPEND_VEC3(vertices, glm::vec3(0.0f, 0.0f, -height/2.0f));
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

    meshes.emplace(object_name, Mesh(vertices, faces, generator_layout));
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
    const GLuint vertex_num = num_loop_samples*num_circle_samples;
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

    for (int i = 0; i < num_loop_samples; i++){ // large loop
        
        theta = 2.0*glm::pi<GLfloat>()*i/num_loop_samples; // loop sample (angle theta)
        loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

        for (int j = 0; j < num_circle_samples; j++){ // small circle
            
            phi = 2.0*glm::pi<GLfloat>()*j/num_circle_samples; // circle sample (angle phi)
            
            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
            vertex_position = loop_center + vertex_normal*circle_radius;
            vertex_color = glm::vec3(1.0 - ((float) i / (float) num_loop_samples), 
                                            (float) i / (float) num_loop_samples, 
                                            (float) j / (float) num_circle_samples);
            // vertex_color = {1.0f, 0.0f, 0.0f};
            vertex_coord = glm::vec2(theta / (2.0*glm::pi<GLfloat>()),
                                     phi / (2.0*glm::pi<GLfloat>()));

            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*num_circle_samples+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*num_circle_samples+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*num_circle_samples+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create triangles
    for (int i = 0; i < num_loop_samples; i++){
        for (int j = 0; j < num_circle_samples; j++){
            // Two triangles per quad
            glm::vec3 t1(((i + 1) % num_loop_samples)*num_circle_samples + j, 
                         i*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + j);    
            glm::vec3 t2(((i + 1) % num_loop_samples)*num_circle_samples + j,
                         ((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + ((j + 1) % num_circle_samples));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*num_circle_samples+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*num_circle_samples+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    meshes.emplace(object_name, Mesh(vertex, vertex_num * vertex_att, face, face_num * face_att, generator_layout));

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

    meshes.emplace(object_name, Mesh(vertex, vertex_num * vertex_att, face, face_num * face_att, generator_layout));

    delete [] vertex;
    delete [] face;
}

void ResourceManager::CreatePointCloud(std::string object_name, int num_points, float size, glm::vec4 color) {
    std::vector<float> vertices;
    std::vector<unsigned int> inds;
    for(int i = 0 ; i < num_points; i++ ) {
        glm::vec3 pos = glm::ballRand(size);
        // if(color == glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) {
            color = glm::vec4(glm::ballRand(2.0), 1.0f);
        // }
        
        // hack to use the same shader as other objects (really only need pos and color)
        APPEND_VEC3(vertices, pos);
        APPEND_VEC3(vertices, glm::vec3(1.0, 0.0, 0.0));
        APPEND_VEC3(vertices, color);
        APPEND_VEC2(vertices, glm::vec2(1.0, 1.0));
    }

    meshes.emplace(object_name, Mesh(vertices, inds, generator_layout));
}

void ResourceManager::CreateQuad(std::string name) {
    std::vector<float> vertices = {
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		0, 2, 3
	};

    Layout l = Layout({
        {FLOAT3, "vertex"},
        {FLOAT2, "uv"}
    });
    
    meshes.emplace(name, Mesh(vertices, indices, l));
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
