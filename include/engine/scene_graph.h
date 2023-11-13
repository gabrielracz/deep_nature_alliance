#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <vector>

#include "camera.h"
#include "collision_manager.h"
#include "light.h"
#include "player.h"
#include "resource.h"
#include "scene_node.h"

// Class that manages all the objects in a scene
class SceneGraph {
   public:
    // Scene nodes to render

    // Constructor and destructor
    SceneGraph(void);
    ~SceneGraph();

    // Background color
    void SetBackgroundColor(glm::vec3 color) { background_color_ = color; }
    glm::vec3 GetBackgroundColor(void) const { return background_color_; }

    // Add an already-created node
    void AddNode(SceneNode* node) { node_.push_back(node); }
    void AddLight(Light* light) { lights.push_back(light); }
    void SetPlayer(Player* p);

    // Find a scene node with a specific name
    SceneNode* GetNode(std::string node_name) const;
    CollisionManager& GetColman() { return colman; }
    std::vector<Light*>& GetLights() { return lights; }
    Camera& GetCamera() { return camera; }
    Player* GetPlayer() { return player; }

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
    Camera camera;
    Player* player;

};  // class SceneGraph

#endif  // SCENE_GRAPH_H_
