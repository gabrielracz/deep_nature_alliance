#include "menu_controller.h"

Menu_Player::Menu_Player(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id): 
    Player(name, mesh_id, shader_id, texture_id), buttons(){

}

void Menu_Player::Update(double dt){

}

void Menu_Player::Control(Controls c, float dt, float damping){
    double x, y;
    glfwGetCursorPos(win->ptr, &x, &y);
    if (c == Player::Controls::LEFTCLICK){
        for (auto button: buttons){
            button->click(glm::vec2(x, y), win);
        }
    }
}

void Menu_Player::MouseControls(Mouse& mouse) {
    //need to do something to display cursor here maybe just have a child scenenode with another quad and texture which i move with mouse.prev
    //or use system cursor and disable capturing
    //just set quad stuff here
}
