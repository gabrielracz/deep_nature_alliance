#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <vector>
#include <deque>

// #include "application.h"
#include "camera.h"
#include "collision_manager.h"
#include "light.h"
#include "player.h"
#include "resource.h"
#include "scene_node.h"
#include "fp_player.h"
#include "text.h"

class Application;

// Class that manages all the objects in a scene
class SceneGraph {
   public:
    // Scene nodes to render

    // Constructor and destructor
    SceneGraph(Application& app);
    ~SceneGraph();

    // Background color
    void SetBackgroundColor(glm::vec3 color) { background_color_ = color; }
    glm::vec3 GetBackgroundColor(void) const { return background_color_; }

    // Add an already-created node
    void AddNode(SceneNode* node) { node_.push_back(node); }
    void AddText(Text* t) {texts.push_back(t);}
    void AddCollider(SceneNode* node) { colman.AddNode(node); }
    void AddLight(Light* light) { lights.push_back(light); }
    void SetPlayer(Player* p);
    void SetSkybox(SceneNode* s) {skybox = s;};

    void PushStoryText(Text* text);
    void DismissStoryText();

    // Find a scene node with a specific name
    SceneNode* GetNode(std::string node_name) const;
    CollisionManager& GetColman() { return colman; }
    std::vector<Light*>& GetLights() { return lights; }
    Camera& GetCamera() { return camera; }
    Player* GetPlayer() { return player; }
    SceneNode* GetSkybox() { return skybox;}
    std::vector<SceneNode*> GetScreenSpaceNodes();

    // Get node const iterator
    std::vector<SceneNode*>::const_iterator begin() const { return node_.begin(); }
    std::vector<SceneNode*>::const_iterator end() const { return node_.end(); }

    // Update entire scene
    void Update(double dt);

   private:
    // Background color
    glm::vec3 background_color_;
    std::vector<SceneNode*> node_;
    CollisionManager colman;
    std::vector<Light*> lights;
    std::deque<Text*> story_text;
    std::vector<Text*> texts;
    Camera camera;
    Player* player;
    SceneNode* skybox = nullptr;
    Application& app;

};  // class SceneGraph

#endif  // SCENE_GRAPH_H_
