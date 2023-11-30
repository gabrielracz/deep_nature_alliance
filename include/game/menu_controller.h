#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_MENU_CONTROLLER_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_MENU_CONTROLLER_H_

#include <vector>
#include <functional>

#include "player.h"
#include "defines.h"

class Button{
    public:
        float xCoord;
        float zCoord;
        float xSpan;
        float zSpan;
        std::function<void()> action;
        bool click(glm::vec2 mousePos, Window *win){

        }
};

class Menu_Player : public Player {

    public:
        Menu_Player(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id): 
            Player(name, mesh_id, shader_id, texture_id), buttons() {}

        virtual void Update(double dt) override;
        virtual void Control(Controls c, float dt, float damping = 1.0) override;
        virtual void MouseControls(Mouse& mouse) override;

    private:

        std::vector<Button> buttons;
        //needed for aspect ratio and shit 
        Window *win;

        

};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_MENU_CONTROLLER_H_