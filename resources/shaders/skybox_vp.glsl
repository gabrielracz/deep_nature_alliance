#version 330 core

layout (location = 0) in vec3 vertex;

out vec3 texture_coordinates;

uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main() {
    mat4 view = mat4(mat3(view_mat));  // fix skybox at the origin of the camera
    vec4 pos = projection_mat * view * vec4(vertex, 1.0);
    texture_coordinates = vertex;
    gl_Position = pos.xyww;            // put a 1 in our depth value after persepective divide 
}
