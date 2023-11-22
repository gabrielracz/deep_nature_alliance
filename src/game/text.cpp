#include "text.h"
#include "game.h"
#include "application.h"
#include "scene_node.h"
#include <cstring>
#include <cstring>

Text::Text(std::string content, Anchor anchor, const glm::vec4 col, const glm::vec4 back_col)
: SceneNode("Obj_StoryText", "M_Quad", "S_Text", "T_Charmap"), anchor(anchor), color(col), background_color(back_col), content(content) {
    camera_projection = Camera::Projection::ORTHOGRAPHIC;
    alpha_enabled = true;
    transform.SetPosition({1.0, 0.0, 0.0});
    SetContent(content);
}

Text::Text(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, const std::string& content)
: SceneNode(name, mesh_id, shader_id, texture_id), content(content){
    camera_projection = Camera::Projection::ORTHOGRAPHIC;
    alpha_enabled = true;
    SetContent(content);
}

void Text::Update(double dt, int w, int h) {
    // these really should be globally accessible
    win_width = w;
    win_height = h;
    std::cout << name << std::endl;
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

    float asp = 1.0f/((float)win_width/(float)win_height);
    
    float ndc_width_distance = (text_width/((float)(win_width)/2));
    float ndc_height_distance = ((size*num_lines)/((float)win_height/2));

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
        case Anchor::LEFTCENTER:
            translation = glm::translate(glm::vec3(x + ndc_width_distance, y, 0.0f));
            break;
        case Anchor::RIGHTCENTER:
            translation = glm::translate(glm::vec3(x - ndc_width_distance, y, 0.0f));
            break;
    }

    glm::mat4 transformation_matrix = translation * glm::scale(glm::vec3(sx, sy, 1.0f));

    shader->SetUniform4m(transformation_matrix, "world_mat");

    shader->SetUniform4f(color, "text_color");
    shader->SetUniform4f(background_color, "background_color");

	// Set the text data
    std::vector<int> data;
	for (int i = 0; i < len; i++){
		data.push_back(content[i]);
	}

    shader->SetUniform1iv(data.data(), len, "text_content");
    // camera->SetProjectionUniforms(shader, Camera::Projection::ORTHOGRAPHIC);
    alpha_enabled = true;
}