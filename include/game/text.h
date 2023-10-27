#ifndef TEXT_H
#define TEXT_H

#include "scene_node.h"

class Game;

class Text : public SceneNode {
public:
    enum class Anchor {
        TOPLEFT,
        BOTTOMLEFT,
        TOPRIGHT,
        BOTTOMRIGHT,
        CENTER
    };

    Text(const std::string name, Mesh* Mesh, Shader* shader, Game* game, const std::string& content)
    : SceneNode(name, Mesh, shader), content(content), game(game) {}

    Anchor anchor = Anchor::TOPLEFT;
protected:
    virtual void SetUniforms(Shader* shader, Camera *camera, const glm::mat4 &parent_matrix) override;

    std::string content;
    glm::vec4 color;
    glm::vec4 background_color;
    Game* game;
};

#endif