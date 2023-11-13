#include "scene_graph.h"

SceneGraph::SceneGraph(void)
    : colman() {
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
}
