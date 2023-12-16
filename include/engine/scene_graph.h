#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <vector>
#include <list>
#include <deque>
#include <memory>

#include "camera.h"
#include "collision_manager.h"
#include "light.h"
#include "player.h"
#include "resource.h"
#include "scene_node.h"
#include "fp_player.h"
#include "terrain.h"
#include "text.h"

// Class that manages all the objects in a scene
class SceneGraph {
public:
    // Scene nodes to render

    // Constructor and destructor
    SceneGraph(Game* game) : colman(game) {}
    ~SceneGraph();

    // Background color
    void SetBackgroundColor(glm::vec3 color) { background_color_ = color; }
    glm::vec3 GetBackgroundColor(void) const { return background_color_; }

    // Add an already-created node
    void AddNode(std::shared_ptr<SceneNode> node) { node_.push_back(node); }
    void AddText(std::shared_ptr<Text> t) { texts.push_back(t); }
    void AddCollider(std::shared_ptr<SceneNode> node) { colman.AddNode(node); }
    void AddLight(std::shared_ptr<Light> light) { lights.push_back(light); }
    void AddTerrain(std::shared_ptr<Terrain> terr) { terrain = terr; node_.push_back(terr); }
    void SetPlayer(std::shared_ptr<Player> p);
    void SetSkybox(std::shared_ptr<SceneNode> s) { skybox = s; };
    void ToggleHUD() { show_hud = !show_hud; }
    void SetCollision(bool toggle) { collision_enabled = toggle; }
    int StoryTextAmount() { return story_text.size(); }

    void PushStoryText(std::shared_ptr<Text> text);
    void DismissStoryText();

    void ClearStoryText();
    void ClearAllNodes();
    void ClearText();
    void ClearLights();
    void ClearEverything();

    void SetResetCallback(std::function<void()> f) { reset_callback = f; }
    std::function<void()> GetResetCallback() { return reset_callback; }
    void Reset();

    // Find a scene node with a specific name
    std::shared_ptr<SceneNode> GetNode(std::string node_name) const;
    CollisionManager& GetColman() { return colman; }
    std::vector<std::shared_ptr<Light>>& GetLights() { return lights; }
    Camera& GetCamera() { return camera; }
    std::shared_ptr<Player> GetPlayer() { return player; }
    std::shared_ptr<SceneNode> GetSkybox() { return skybox; }
    std::vector<std::shared_ptr<SceneNode>> GetScreenSpaceNodes();
    std::shared_ptr<Terrain> GetTerrain() { return terrain; }

    // Get node const iterator
    std::vector<std::shared_ptr<SceneNode>>::const_iterator begin() const { return node_.begin(); }
    std::vector<std::shared_ptr<SceneNode>>::const_iterator end() const { return node_.end(); }

    // Update entire scene
    void Update(double dt);

private:
    // Background color
    glm::vec3 background_color_;
    std::vector<std::shared_ptr<SceneNode>> node_;
    CollisionManager colman;
    std::vector<std::shared_ptr<Light>> lights;
    std::deque<std::shared_ptr<Text>> story_text;
    std::vector<std::shared_ptr<Text>> texts;
    bool show_hud = true;
    Camera camera;
    std::shared_ptr<Player> player;
    std::shared_ptr<SceneNode> skybox = nullptr;
    std::shared_ptr<Terrain> terrain;
    bool collision_enabled = true;

    std::function<void()> reset_callback;
};

#endif // SCENE_GRAPH_H_
