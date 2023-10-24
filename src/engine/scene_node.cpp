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

SceneNode::SceneNode(const std::string name, const Resource *geometry, const Resource *material){

    // Set name of scene node
    name_ = name;

    // Set geometry
    if (geometry->GetType() == PointSet){
        mode_ = GL_POINTS;
    } else if (geometry->GetType() == Mesh){
        mode_ = GL_TRIANGLES;
    } else {
        throw(std::invalid_argument(std::string("Invalid type of geometry")));
    }

    array_buffer_ = geometry->GetArrayBuffer();
    element_array_buffer_ = geometry->GetElementArrayBuffer();
    size_ = geometry->GetSize();

    // Set material (shader program)
    if (material->GetType() != Material){
        throw(std::invalid_argument(std::string("Invalid type of material")));
    }

    material_ = material->GetResource();

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


void SceneNode::Draw(Camera *camera, const glm::mat4& parent_matrix){
    if(!active || !visible) {return;}

    // Select proper material (shader program)
    glUseProgram(material_);

    // Set geometry to draw
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

    // Set globals for camera
    camera->SetupShader(material_);

    // Set world matrix and other shader input variables
    SetupShader(material_, parent_matrix);

    // Draw geometry
    if (mode_ == GL_POINTS){
        glDrawArrays(mode_, 0, size_);
    } else {
        glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);
    }

    glm::mat4 rotation = glm::mat4_cast(transform.orientation);
    glm::mat4 translation = glm::translate(glm::mat4(1.0), transform.position);
    glm::mat4 joint_translate = glm::translate(glm::mat4(1.0), -transform.joint);
    glm::mat4 orbit = glm::inverse(joint_translate) * glm::mat4_cast(transform.orbit) * joint_translate;
    glm::mat4 t = parent_matrix * translation * orbit * rotation; //temporary hack to avoid sending scale to children, better way coming soon
    for(auto child : children) {
        child->Draw(camera, t);
    }
}


void SceneNode::Update(double dt){
    elapsed += dt;
    if(!active) {return;}
    for(auto child : children) {
        child->Update(dt);
    }
    // Do nothing for this generic type of scene node
}


void SceneNode::SetupShader(GLuint program, const glm::mat4& parent_matrix){

    // Set attributes for shaders
    GLint vertex_att = glGetAttribLocation(program, "vertex");
    glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(vertex_att);

    GLint normal_att = glGetAttribLocation(program, "normal");
    glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(normal_att);

    GLint color_att = glGetAttribLocation(program, "color");
    glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(color_att);

    GLint tex_att = glGetAttribLocation(program, "uv");
    glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (9*sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_att);

    // World transformation
    glm::mat4 scaling = glm::scale(glm::mat4(1.0), transform.scale);
    glm::mat4 rotation = glm::mat4_cast(transform.orientation);
    glm::mat4 translation = glm::translate(glm::mat4(1.0), transform.position);

    glm::mat4 joint_translate = glm::translate(glm::mat4(1.0), -transform.joint);
    glm::mat4 orbit = glm::inverse(joint_translate) * glm::mat4_cast(transform.orbit) * joint_translate;

    glm::mat4 transf = parent_matrix * translation * orbit * rotation * scaling;

    GLint world_mat = glGetUniformLocation(program, "world_mat");
    glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

    // Timer
    GLint timer_var = glGetUniformLocation(program, "timer");
    double current_time = glfwGetTime();
    glUniform1f(timer_var, (float) current_time);

    GLint inverted_var = glGetUniformLocation(program, "inverted");
    glUniform1i(inverted_var, inverted);
}
