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

// Class that manages all the objects in a scene
class SceneGraph {

    private:
        // Background color
        glm::vec3 background_color_;

    public:
        // Scene nodes to render
        std::vector<SceneNode *> node_;

        // Constructor and destructor
        SceneGraph(void);
        ~SceneGraph();

        // Background color
        void SetBackgroundColor(glm::vec3 color);
        glm::vec3 GetBackgroundColor(void) const;
        
        // Create a scene node from the specified resources
        SceneNode *CreateNode(std::string node_name, Resource *geometry, Resource *material);
        // Add an already-created node
        void AddNode(SceneNode *node);
        // Find a scene node with a specific name
        SceneNode *GetNode(std::string node_name) const;
        // Get node const iterator
        std::vector<SceneNode *>::const_iterator begin() const;
        std::vector<SceneNode *>::const_iterator end() const;



        // Draw the entire scene
        void Draw(Camera* camera);

        // Update entire scene
        void Update(double dt);

}; // class SceneGraph

#endif // SCENE_GRAPH_H_
