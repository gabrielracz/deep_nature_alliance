#include "toggle.h"
#include "colliders/colliders.h"

Toggle::Toggle(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id)
    : SceneNode(name, mesh_id, shader_id, texture_id)
{
    SetNodeType(TTOGGLE);
    visible = false;
}

void Toggle::ToggleOn(const SceneNode& collider)
{
    if (trigger_count_ > max_trigger_count_ && max_trigger_count_ > 0) {
        return;
    }

    toggle_ = true;

    if (!triggered_) {
        triggered_ = true;
    }

    if (trigger_count_ < max_trigger_count_) {
        ++trigger_count_;
    }

    on_callback_();
}

void Toggle::ToggleOff(const SceneNode& exiter) {
    toggle_ = false;

    off_callback_();
}
