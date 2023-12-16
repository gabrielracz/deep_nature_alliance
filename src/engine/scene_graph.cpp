#include "scene_graph.h"

SceneGraph::~SceneGraph() {
    Reset();
    // No need to manually delete shared_ptr objects
    node_.clear();
}

void SceneGraph::SetPlayer(std::shared_ptr<Player> p) {
    player = p;
    colman.SetPlayer(p);
}

std::shared_ptr<SceneNode> SceneGraph::GetNode(std::string node_name) const {
    for (const auto& node : node_) {
        if (node->GetName() == node_name) {
            return node;
        }
    }
    return nullptr;
}

void SceneGraph::Update(double dt) {

    if (collision_enabled) {
        colman.CheckCollisions();
    }

    // remove deleted nodes
    node_.erase(
        std::remove_if(node_.begin(), node_.end(),
            [](std::shared_ptr<SceneNode>&  sn){ return sn->deleted; } 
        ),
        node_.end()
    );

    for(auto it = node_.begin(); it != node_.end();) {
        std::shared_ptr<SceneNode> sn = *it;
        if(sn == nullptr) { // just in case
            continue;
        }
        sn->Update(dt);
        it++;
    }

    int w = camera.GetWinWidth();
    int h = camera.GetWinHeight();
    if (!story_text.empty()) {
        story_text.front()->Update(dt, w, h);
    }

    for (auto& t : texts) {
        t->Update(dt, w, h);
    }

    colman.WhyCouldntTheyJustBeInvisible();

    // UPDATE CAMERA AFTER NODES ALWAYS !!!!!
    camera.Update(dt);
}

void SceneGraph::PushStoryText(std::shared_ptr<Text> text) {
    story_text.push_back(text);
}

void SceneGraph::DismissStoryText() {
    if (!story_text.empty()) {
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
    ClearLights();
    player = nullptr;
}

void SceneGraph::Reset() {
    colman.Reset();
    ClearEverything();
    if (reset_callback) {
        reset_callback();
    }
}

std::vector<std::shared_ptr<SceneNode>> SceneGraph::GetScreenSpaceNodes() {
    std::vector<std::shared_ptr<SceneNode>> nodes;
    if (!show_hud) {
        return nodes;
    }

    nodes.reserve(texts.size() + 1);
    nodes.insert(nodes.end(), texts.begin(), texts.end());
    if (!story_text.empty()) {
        nodes.push_back(story_text.front());
    }
    return nodes;
}
