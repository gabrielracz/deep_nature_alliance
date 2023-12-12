#include "scene_graph.h"

//TODO: Add SceneGraph INIT
SceneGraph::SceneGraph()
    : colman(){
    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}

SceneGraph::~SceneGraph() {
    Reset();
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
    /*for (int i = 0; i < node_.size(); i++) {
        node_[i]->Update(dt);
    }*/

    for (auto it = node_.begin(); it != node_.end();) {
        SceneNode *sn = *it;
        if (sn->deleted) {
            it = node_.erase(it);
        } else{
            sn->Update(dt);
            ++it;
        } 
    }

    int w = camera.GetWinWidth();
    int h = camera.GetWinHeight();
    if(!story_text.empty()){
        story_text.front()->Update(dt, w, h);
    }
    for(auto t : texts) {
        t->Update(dt, w, h);
    }

    if(collision_enabled) {
        colman.CheckCollisions();
    }

    // for (Light* l : lights){
    //     if (l->GetParentTransform() != nullptr){
    //         l->transform.Update(l->GetParentTransform()->CalculateMatrix());
    //     }
    // }

    // UPDATE CAMERA AFTER NODES ALWAYS !!!!!
    camera.Update(dt);
}

void SceneGraph::PushStoryText(Text* text) {
    story_text.push_back(text);
}

void SceneGraph::DismissStoryText() {
    if(!story_text.empty()) {
        story_text.pop_front();
    }
}

void SceneGraph::ClearStoryText() {
    story_text.clear();
}

void SceneGraph::ClearAllNodes() {
    node_.clear();
}

void SceneGraph::ClearText() {
    texts.clear();
}

void SceneGraph::ClearLights() {
    lights.clear();
}

void SceneGraph::ClearEverything() {
    ClearStoryText();
    ClearAllNodes();
    ClearText();
}

void SceneGraph::Reset() {
    colman.Reset();
    ClearEverything();
    if(reset_callback) {
        reset_callback();
    }
}

std::vector<SceneNode*> SceneGraph::GetScreenSpaceNodes() {
    std::vector<SceneNode*> nodes;
    if(!show_hud) {
        return nodes;
    }

    nodes.reserve(texts.size() + 1);
    nodes.insert(nodes.end(), texts.begin(), texts.end());
    if(!story_text.empty()) {
        nodes.push_back(story_text.front());
    }
    return nodes;
}