#include "text.h"
#include "game.h"
#include "application.h"
#include <cstring>
#include <cstring>

Text::Text(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, Game* game, const std::string& content)
: SceneNode(name, mesh_id, shader_id, texture_id), content(content), game(game) {
    camera_projection = Camera::Projection::ORTHOGRAPHIC;
    alpha_enabled = true;
    SetContent(content);
}

void Text::Update(double dt) {
    if(update_callback) {
        SetContent(update_callback());
    }

    SceneNode::Update(dt);
}


// Please avert your eyes
void Text::SetContent(const std::string& str) {
    if(str == "") {
        content = "";
        line_len = content.size();
        num_lines = 0;
        return;
    }

    int start = 0;
    int curr = 0;
    int longest = 0;
    std::vector<std::string> lines;
    std::string result;

    for(int i = 0; i < str.size(); i++) {
        char ch = str[i];
        // find the longest line, breaking at newline
        if(ch == '\n') {
            int len = i - start;
            start = i+1;
            if(len > longest) {
                longest = len;
            }
        } else if (i == str.size() - 1) { // make sure we account for the last line being the longest
            if(lines.size() == 0) {
                lines.push_back("");
            }
            lines.back().push_back(ch);
            int len = i - start + 1;
            if(len > longest) {
                longest = len;
            }
        }else { // append to our current line
            if(start == i) {
                lines.push_back("");
            }
            lines.back().push_back(ch);
        }
    }

    // combine the lines into a ' ' padded string, all lines having equal length (text block)
    for(auto& l : lines) {
        result += l;
            result.insert(result.end(), longest - l.size(), ' ');
    }

    content = result;
    line_len = longest;
    num_lines = lines.size();
}

void Text::SetUniforms(Shader* shader, const glm::mat4 &view_matrix) {
    int len = content.size();
    shader->SetUniform1i(line_len, "line_len");
    shader->SetUniform1i(num_lines, "num_lines");

    //chars are 8x15 pixels
    float text_width = line_len*8/15.0f*(size);
	float sx = text_width;
	float sy = size*num_lines;

    int winwidth = game->app.GetWinWidth();
    int winheight = game->app.GetWinHeight();

    float asp = 1.0f/((float)winwidth/(float)winheight);
    
    float ndc_width_distance = (text_width/((float)(winwidth)/2));
    float ndc_height_distance = ((size*num_lines)/((float)winheight/2));

    float x = transform.GetPosition().x;
    float y = transform.GetPosition().y;

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