#include "text.h"
#include "game.h"
#include "application.h"

Text::Text(const std::string name, Mesh* Mesh, Shader* shader, Game* game, const std::string& content)
: SceneNode(name, Mesh, shader), content(content), game(game) {
    camera_projection = Camera::Projection::ORTHOGRAPHIC;
    alpha_enabled = true;
}

void Text::Update(double dt) {
    if(update_callback) {
        content = update_callback();
    }

    SceneNode::Update(dt);
}

void Text::SetUniforms(Camera& camera, const glm::mat4 &parent_matrix) {
    int len = content.size();
    shader->SetUniform1i(len, "text_len");

    //chars are 8x15 pixels
    float text_width = len*8/15.0f*(size);
	float sx = text_width;
	float sy = size;

    int winwidth = game->app.GetWinWidth();
    int winheight = game->app.GetWinHeight();

    float asp = 1.0f/((float)winwidth/(float)winheight);
    
    float ndc_width_distance = (text_width/((float)(winwidth)/2));
    float ndc_height_distance = (size/((float)winheight/2));

    float& x = transform.position.x;
    float& y = transform.position.y;

    // change the anchor point of the text quad
    glm::mat4 translation;
    switch(anchor) {
        case Anchor::TOPLEFT:
            translation = glm::translate(glm::vec3(x + ndc_width_distance, y - ndc_height_distance, 0.0f));
            break;
        case Anchor::BOTTOMLEFT:
            translation = glm::translate(glm::vec3(x + ndc_width_distance, y + ndc_height_distance, 0.0f));
            break;
        case Anchor::TOPRIGHT:
            translation = glm::translate(glm::vec3(x - ndc_width_distance, y - ndc_height_distance, 0.0f));
            break;
        case Anchor::BOTTOMRIGHT:
            translation = glm::translate(glm::vec3(x - ndc_width_distance, y + ndc_height_distance, 0.0f));
            break;
        case Anchor::CENTER:
            translation = glm::translate(glm::vec3(x, y, 0.0f));
            break;
        case Anchor::TOPCENTER:
            translation = glm::translate(glm::vec3(x, y - ndc_height_distance, 0.0f));
            break;
        case Anchor::BOTTOMCENTER:
            translation = glm::translate(glm::vec3(x, y + ndc_height_distance, 0.0f));
            break;
    }

    glm::mat4 transformation_matrix = translation * glm::scale(glm::vec3(sx, sy, 1.0f));

    shader->SetUniform4m(transformation_matrix, "world_mat");

    shader->SetUniform4f(color, "text_color");
    shader->SetUniform4f(background_color, "background_color");

	// Set the text data
	assert(512 > len);
	int data[512] = {0};
	for (int i = 0; i < len; i++){
		data[i] = content[i];
	}

    shader->SetUniform1iv(data, len, "text_content");
    // camera->SetProjectionUniforms(shader, Camera::Projection::ORTHOGRAPHIC);
    alpha_enabled = true;
}