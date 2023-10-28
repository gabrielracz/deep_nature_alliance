#ifndef TEXT_H
#define TEXT_H

#include <functional>
#include "scene_node.h"

class Game;

class Text : public SceneNode {
public:
    enum class Anchor {
        TOPLEFT,
        BOTTOMLEFT,
        TOPRIGHT,
        BOTTOMRIGHT,
        CENTER,
        TOPCENTER,
        BOTTOMCENTER
    };

    Text(const std::string name, Mesh* Mesh, Shader* shader, Game* game, const std::string& content)
    : SceneNode(name, Mesh, shader), content(content), game(game) {}

    virtual void Update(double dt) override;

    Anchor anchor = Anchor::TOPLEFT;
    void SetCallback(std::function<std::string(void)> f) {update_callback = f;}
    void SetColor(const glm::vec4& newcol) {color = newcol;}
    void SetBackgroundColor(const glm::vec4& newcol) {background_color = newcol;}
    void SetAnchor(Anchor a) {anchor = a;}
    void SetSize(float newsize) {size = newsize;}
protected:
    virtual void SetUniforms(Shader* shader, Camera *camera, const glm::mat4 &parent_matrix) override;

    float size = 13.125;
    std::string content = "";
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 background_color = {0.0f, 0.0f, 0.0f, 0.5f};
    std::function<std::string(void)> update_callback;
    Game* game;
};

#endif