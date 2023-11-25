#ifndef TEXT_H
#define TEXT_H

#include <functional>
#include "camera.h"
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
        BOTTOMCENTER,
        LEFTCENTER,
        RIGHTCENTER
    };

    Text(std::string content, const glm::vec4 col = {1.0f, 1.0f, 1.0f, 1.0f}, const glm::vec4 back_col = {0.0f, 0.0f, 0.0f, 0.5f}, Anchor anchor = Anchor::RIGHTCENTER, const glm::vec3& pos = {1.0, 0.0, 0.0}, float delay = 0.05f);
    Text(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, const std::string& content = "");

    void Update(double dt, int win_width, int win_height);
    void SetContent(const std::string& c);

    Anchor anchor = Anchor::TOPLEFT;
    void SetCallback(std::function<std::string(void)> f) {update_callback = f;}
    void SetColor(const glm::vec4& newcol) {color = newcol;}
    void SetBackgroundColor(const glm::vec4& newcol) {background_color = newcol;}
    void SetAnchor(Anchor a) {anchor = a;}
    void SetSize(float newsize) {size = newsize;}
    void SetScrolling(float character_delay = 0.25f);
    float asp_ratio =8/15.0f;
protected:
    virtual void SetUniforms(Shader* shader, const glm::mat4 &parent_matrix) override;
    std::string ScrollText();

    int line_len = 0;
    int num_lines = 0;
    float size = 13.125;
    std::string content = "";
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 background_color = {0.0f, 0.0f, 0.0f, 0.5f};
    std::function<std::string(void)> update_callback {nullptr};
    // Game* game;
    int win_width = 1920;
    int win_height = 1080;
    float update_delay = 0.0f;
    float update_timer = 0.0f;
    std::string scroll_buffer = "";
    int scroll_index = 0;
    bool scrolling = false;

};

#endif