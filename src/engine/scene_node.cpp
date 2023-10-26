#include <glm/fwd.hpp>
#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <time.h>
#include <glm/gtx/string_cast.hpp>

#include "scene_node.h"

SceneNode::SceneNode(const std::string name, Mesh* m, Shader* shd){

    // Set name of scene node
    name_ = name;

    // Set geometry
    // if (geometry->GetType() == PointSet){
    //     mode_ = GL_POINTS;
    // } else if (geometry->GetType() == MeshH){
    //     mode_ = GL_TRIANGLES;
    // } else {
    //     throw(std::invalid_argument(std::string("Invalid type of geometry")));
    // }

    // array_buffer_ = geometry->GetArrayBuffer();
    // element_array_buffer_ = geometry->GetElementArrayBuffer();
    // size_ = geometry->GetSize();
    mesh = m;

    // Set material (shader program)
    // if (material->GetType() != Material){
    //     throw(std::invalid_argument(std::string("Invalid type of material")));
    // }

    // material_ = material->GetResource();
    shader = shd;

    // Other attributes
    transform.scale = glm::vec3(1.0, 1.0, 1.0);
}


SceneNode::~SceneNode(){
    for(auto child : children) {
        delete child;
    }
}


const std::string SceneNode::GetName(void) const {

    return name_;
}


glm::vec3 SceneNode::GetPosition(void) const {

    return transform.position;
}


glm::quat SceneNode::GetOrientation(void) const {

    return transform.orientation;
}


glm::vec3 SceneNode::GetScale(void) const {

    return transform.scale;
}


void SceneNode::SetPosition(glm::vec3 position){

    transform.position = position;
}


void SceneNode::SetOrientation(glm::quat orientation){

    transform.orientation = orientation;
}


void SceneNode::SetScale(glm::vec3 scale){

    transform.scale = scale;
}


void SceneNode::Translate(glm::vec3 trans){

    transform.position += trans;
}


void SceneNode::Rotate(glm::quat rot){

    transform.orientation *= rot;
    transform.orientation = glm::normalize(transform.orientation);
}


void SceneNode::Scale(glm::vec3 scale){

    transform.scale *= scale;
}


GLenum SceneNode::GetMode(void) const {

    return mode_;
}


GLuint SceneNode::GetArrayBuffer(void) const {

    return array_buffer_;
}


GLuint SceneNode::GetElementArrayBuffer(void) const {

    return element_array_buffer_;
}


GLsizei SceneNode::GetSize(void) const {

    return size_;
}


GLuint SceneNode::GetMaterial(void) const {

    return material_;
}


void SceneNode::Draw(Camera* camera, const glm::mat4& parent_matrix){
    if(!active || !visible) {return;}

    shader->Use();
    camera->SetUniforms(shader);
    SetUniforms(shader, parent_matrix);
    // texture->Bind();
    mesh->Draw();

    // Set geometry to draw
    // glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

    // SetUniforms(shader, parent_matrix);

    // glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);

    // remove the scaling factor from our cached transf matrix (children shouldnt scale with parent)

    glm::mat4 t = transf_matrix;
    t[0] = glm::normalize(t[0]);
    t[1] = glm::normalize(t[1]);
    t[2] = glm::normalize(t[2]);

    // glm::mat4 tm = parent_matrix * transform.Matrix(); //temporary hack to avoid sending scale to children, better way coming soon
    glm::mat4 tm = parent_matrix * t; //temporary hack to avoid sending scale to children, better way coming soon
    for(auto child : children) {
        child->Draw(camera, tm);
    }
}


void SceneNode::Update(double dt){
    elapsed += dt;
    if(!active) {return;}

    transf_matrix = transform.ScaledMatrix();

    for(auto child : children) {
        child->Update(dt);
    }
}


void SceneNode::SetUniforms(Shader* shader, const glm::mat4& parent_matrix){

    // int program = shader->id;
    // // Set attributes for shaders
    // GLint vertex_att = glGetAttribLocation(program, "vertex");
    // glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), 0);    // glm::mat4 t = transf_matrix;
    // glEnableVertexAttribArray(vertex_att);

    // GLint normal_att = glGetAttribLocation(program, "normal");
    // glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
    // glEnableVertexAttribArray(normal_att);

    // GLint color_att = glGetAttribLocation(program, "color");
    // glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
    // glEnableVertexAttribArray(color_att);

    // GLint tex_att = glGetAttribLocation(program, "uv");
    // glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (9*sizeof(GLfloat)));
    // glEnableVertexAttribArray(tex_att);


    // transf_matrix = parent_matrix * transform.ScaledMatrix();

    shader->SetUniform4m(parent_matrix * transf_matrix, "world_mat");
    // GLint world_mat = glGetUniformLocation(program, "world_mat");
    // glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(parent_matrix * transf_matrix));

    // Timer
    // double current_time = glfwGetTime();
    shader->SetUniform1f(glfwGetTime(), "timer");

    shader->SetUniform1i(0, "inverted");
    // GLint inverted_var = glGetUniformLocation(program, "inverted");
    // glUniform1i(inverted_var, inverted);
}
