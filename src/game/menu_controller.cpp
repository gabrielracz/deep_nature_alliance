#include "menu_controller.h"

Menu_Player::Menu_Player(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id, RenderBundle mouseCursor, Window *w): 
    Player(name, mesh_id, shader_id, texture_id), buttons(), win(w){
        cursor = new SceneNode("mychilddawg", mouseCursor.mesh_id, mouseCursor.shader_id, mouseCursor.texture_id);
        cursor->transform.SetScale(glm::vec3(0.075));
        // cursor->transform.SetPosition(glm::vec3(0))
        AddChild(cursor);

}

void Menu_Player::Update(double dt){
    glm::vec2 mpos = getMousePos();

    float screenX = mpos.x / win->width;
    float screenY = mpos.y / win->height;
    
    const float imageOffsetForMoreAccurateClickingIDontThinkThisVariableIsDescriptiveEnough = 0.068;

    cursor->transform.SetPosition(glm::vec3(screenX * 2.0f - 1.0f - 1.0f + imageOffsetForMoreAccurateClickingIDontThinkThisVariableIsDescriptiveEnough, 1.0f - screenY * 2.0f - imageOffsetForMoreAccurateClickingIDontThinkThisVariableIsDescriptiveEnough, 0.0));

    float radius = 1.0f;
    float angularSpeed = 0.2f;

    // Calculate the new position in a circular motion
    float newX = radius * cos(angularSpeed * dt);
    float newZ = radius * sin(angularSpeed * dt);

    // Set the new position
    transform.SetPosition(glm::vec3(newX, transform.GetPosition().y, newZ));

    SceneNode::Update(dt);
}

void Menu_Player::Control(Controls c, float dt, float damping){
    glm::vec2 mpos = getMousePos();
    bool debugPlacement = false;
    for (auto button : buttons){
        button->click(mpos, win, debugPlacement);
    }
}

void Menu_Player::MouseControls(Mouse& mouse) {
    //just handle in update and control
}

glm::vec2 Menu_Player::getMousePos() {
    double x, y;
    glfwGetCursorPos(win->ptr, &x, &y);
    return {x,y};
}
