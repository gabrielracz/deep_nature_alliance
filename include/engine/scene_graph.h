#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <vector>
#include <list>
#include <deque>

// #include "application.h"
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
    SceneGraph(Game* game): colman(game) {}
    ~SceneGraph();

    // Background color
    void SetBackgroundColor(glm::vec3 color) { background_color_ = color; }
    glm::vec3 GetBackgroundColor(void) const { return background_color_; }

    // Add an already-created node
    void AddNode(SceneNode* node) { node_.push_back(node); }
    void AddText(Text* t) {texts.push_back(t);}
    void AddCollider(SceneNode* node) { colman.AddNode(node); }
    void AddLight(Light* light) { lights.push_back(light); }
    void AddTerrain(Terrain* terr) {terrain = terr; node_.push_back(terr);}
    void SetPlayer(Player* p);
    void SetSkybox(SceneNode* s) {skybox = s;};
    void ToggleHUD(){ show_hud = !show_hud;}
    void SetCollision(bool toggle) { collision_enabled = toggle; }

    void PushStoryText(Text* text);
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
    SceneNode* GetNode(std::string node_name) const;
    CollisionManager& GetColman() { return colman; }
    std::vector<Light*>& GetLights() { return lights; }
    Camera& GetCamera() { return camera; }
    Player* GetPlayer() { return player; }
    SceneNode* GetSkybox() { return skybox;}
    std::vector<SceneNode*> GetScreenSpaceNodes();
    Terrain* GetTerrain() {return terrain;}

    // Get node const iterator
    std::list<SceneNode*>::const_iterator begin() const { return node_.begin(); }
    std::list<SceneNode*>::const_iterator end() const { return node_.end(); }

    // Update entire scene
    void Update(double dt);

   private:
    // Background color
    glm::vec3 background_color_;
    std::list<SceneNode*> node_;
    CollisionManager colman;
    std::vector<Light*> lights;
    std::deque<Text*> story_text;
    std::vector<Text*> texts;
    bool show_hud = true;
    Camera camera;
    Player* player;
    SceneNode* skybox = nullptr;
    Terrain* terrain;
    bool collision_enabled = true;

    std::function<void()> reset_callback;

};  // class SceneGraph

#endif  // SCENE_GRAPH_H_
