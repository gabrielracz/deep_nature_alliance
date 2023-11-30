#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_MENU_CONTROLLER_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_MENU_CONTROLLER_H_

#include <vector>
#include <functional>

#include "player.h"
#include "defines.h"

class Button {
    public:
        glm::vec2 topLeftCoordPercentage;
        glm::vec2 spanPercentage;
        std::function<void()> action;
        bool click(const glm::vec2& mousePos, Window* win) {
            // Calculate the actual coordinates and size of the button in pixels
            int buttonX = static_cast<int>(topLeftCoordPercentage.x * win->width);
            int buttonY = static_cast<int>(topLeftCoordPercentage.y * win->height);
            int buttonWidth = static_cast<int>(spanPercentage.x * win->width);
            int buttonHeight = static_cast<int>(spanPercentage.y * win->height);

            if (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth &&
                mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
                if (action) {
                    action();
                }
                return true;
            }
            return false;
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