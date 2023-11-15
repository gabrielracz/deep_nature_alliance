#include "scene_graph.h"
#include "application.h"

//TODO: Add SceneGraph INIT
SceneGraph::SceneGraph(Application& app)
    : colman(), app(app) {
    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}

SceneGraph::~SceneGraph() {
    for (auto n : node_) {
        delete n;
    }
}

void SceneGraph::SetPlayer(Player* p) {
    player = p;
    colman.SetPlayer(p);
}

void SceneGraph::SetFPPlayer(FP_Player* p) {
    fp_player = p;
}

SceneNode* SceneGraph::GetNode(std::string node_name) const {
    // Find node with the specified name
    for (int i = 0; i < node_.size(); i++) {
        if (node_[i]->GetName() == node_name) {
            return node_[i];
        }
    }
    return NULL;
}

void SceneGraph::Update(double dt) {
    for (int i = 0; i < node_.size(); i++) {
        node_[i]->Update(dt);
    }

    // UPDATE CAMERA AFTER NODES ALWAYS !!!!!
    // TODO: this scene should own the camera
    app.GetCamera().Update();
}
