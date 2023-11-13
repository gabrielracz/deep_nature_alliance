#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_node.h"
#include "resource.h"
#include "camera.h"
#include "light.h"
#include "collision_manager.h"

// Class that manages all the objects in a scene
class SceneGraph {

    public:
        // Scene nodes to render

        // Constructor and destructor
        SceneGraph(void);
        ~SceneGraph();

        // Background color
        void SetBackgroundColor(glm::vec3 color);
        glm::vec3 GetBackgroundColor(void) const;
        
        // Add an already-created node
        void AddNode(SceneNode *node);
        void AddLight(Light* light) { lights.push_back(light); }

        // Find a scene node with a specific name
        SceneNode *GetNode(std::string node_name) const;
        CollisionManager& GetColman() { return colman; }
        std::vector<Light*>& GetLights() {return lights;}
        Camera& GetCamera() {return camera;}

        // Get node const iterator
        std::vector<SceneNode *>::const_iterator begin() const;
        std::vector<SceneNode *>::const_iterator end() const;


        // Draw the entire scene
        void Draw(Camera* camera, std::vector<Light*>& lights);

        // Update entire scene
        void Update(double dt);

    private:
        // Background color
        glm::vec3 background_color_;
        std::vector<SceneNode *> node_;
        CollisionManager colman;
        std::vector<Light*> lights;
        Camera camera;


}; // class SceneGraph

#endif // SCENE_GRAPH_H_
