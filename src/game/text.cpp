#include "text.h"
#include "game.h"
#include "application.h"

void Text::SetUniforms(Shader* shd, Camera *camera, const glm::mat4 &parent_matrix) {
    int len = content.size();
    shd->SetUniform1i(len, "text_len");

    float size = 13.125;
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
    }

    glm::mat4 transformation_matrix = translation * glm::scale(glm::vec3(sx, sy, 1.0f));

    shd->SetUniform4m(transformation_matrix, "world_mat");

    color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    shd->SetUniform4f(color, "text_color");
    background_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    shd->SetUniform4f(background_color, "background_color");

	// Set the text data
	assert(512 > len);
	int data[512] = {0};
	for (int i = 0; i < len; i++){
		data[i] = content[i];
	}

    shd->SetUniform1iv(data, len, "text_content");
    camera->SetProjectionUniforms(shd, Camera::Projection::ORTHOGRAPHIC);
    alpha_enabled = true;
}