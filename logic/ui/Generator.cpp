#include "Generator.h"

namespace fui {
	void Generator::initDefaultStyles() {
		font.loadFromFile("res/ascii futuristic.png", 8);
		texture = fgr::TextureStorage(1024, 1024);
		texture.addImage("res/panel.png", false);
		texture.createBuffer(GL_REPEAT, GL_NEAREST);

		//panel_style = fui::FaceStyle(glm::vec2(0, 0), glm::vec2(8, 8), glm::vec2(16, 16), glm::vec2(24, 24), 0);
		fgr::SubTexture tex;
		tex.hash = 1;
		tex.layer = 0;
		panel_style = fui::FaceStyle(glm::vec2(0, 0), glm::vec2(8, 8), glm::vec2(16, 16), glm::vec2(24, 24), tex);
		top_style = fui::FaceStyle(glm::vec2(24, 0), glm::vec2(32, 8), glm::vec2(40, 16), glm::vec2(48, 18), tex);
		button_default_style = fui::FaceStyle(glm::vec2(5, 29), glm::vec2(8, 32), glm::vec2(16, 40), glm::vec2(19, 43), tex);
		button_highlight_style = fui::FaceStyle(glm::vec2(29, 29), glm::vec2(32, 32), glm::vec2(40, 40), glm::vec2(43, 43), tex);
		button_press_style = fui::FaceStyle(glm::vec2(53, 29), glm::vec2(56, 32), glm::vec2(64, 40), glm::vec2(67, 43), tex);
		textfield_default_style = fui::FaceStyle(glm::vec2(6, 54), glm::vec2(12, 60), glm::vec2(12, 60), glm::vec2(18, 66), tex);
		textfield_highlight_style = fui::FaceStyle(glm::vec2(30, 54), glm::vec2(32, 56), glm::vec2(40, 64), glm::vec2(42, 66), tex);
		textfield_press_style = fui::FaceStyle(glm::vec2(54, 54), glm::vec2(56, 56), glm::vec2(64, 64), glm::vec2(66, 66), tex);
		bar_style = fui::FaceStyle(glm::vec2(81, 10), glm::vec2(83, 12), glm::vec2(85, 13), glm::vec2(88, 15), tex);

		text_color_default = glm::vec4(1.);
		text_color_highlight = glm::vec4(0.8, 0.9, 1., 1.);
		text_color_press = glm::vec4(0.5, 0.5, 0.5, 1.);
		text_color_error = glm::vec4(1., 0.5, 0.5, 1.);
		shadow_color = glm::vec4(0., 0., 0., 0.5);

		slider_bar_size = 6;
	}

	void Generator::startPanel(std::vector<Component*>& components, glm::ivec2 pos, int width, int border_distance, int component_distance, std::string title, bool drag) {
		component_array = &components;
		add_pos = pos + glm::ivec2(border_distance, 20);
		origin_pos = pos;
		Generator::width = width;
		Generator::border_distance = border_distance;
		Generator::component_distance = component_distance;
	
		start_index = components.size();

		components.push_back(new Face(pos, glm::ivec2(width, 20), panel_style));
		components.push_back(new Face(pos, glm::ivec2(width, 16), top_style));
		Text* text = new Text(pos + glm::ivec2(0, 4), glm::ivec2(width, 8), title);
		text->addShadow(glm::ivec2(1), shadow_color);
		text->setStyle(font, fgr::TextFormat::center, 2, 0, text_color_default);
		components.push_back(text);
		if (drag) components.push_back(new DragComponent(pos, glm::ivec2(width, 16)));
	}

	void Generator::addTextField(int height, std::string* str) {
		TextField* tf = new TextField(add_pos, glm::ivec2(width - border_distance * 2, height), str);
		tf->setColors(text_color_default, text_color_highlight, text_color_press);
		tf->setStyle(textfield_default_style, textfield_highlight_style, textfield_press_style, font, fgr::TextFormat::left);
		component_array->push_back(tf);
		add_pos.y += height + component_distance;
	}

	void Generator::addButton(int height, std::string str, void(*event)(flo::InputType type)) {
		Button* button = new Button(add_pos, glm::ivec2(width - border_distance * 2, height), event, str);
		button->setColors(text_color_default, text_color_highlight, text_color_press, text_color_error);
		button->setStyle(button_default_style, button_highlight_style, button_press_style, font, fgr::TextFormat::center);
		component_array->push_back(button);
		add_pos.y += height + component_distance;
	}

	void Generator::addSlider(glm::ivec2 face_size, float* value) {
		Slider* slider = new Slider(add_pos, face_size, width - border_distance * 2, value);
		slider->setStyle(bar_style, button_default_style, button_highlight_style, textfield_highlight_style, slider_bar_size);
		component_array->push_back(slider);
		add_pos.y += face_size.y + component_distance;
	}

	void Generator::addText(std::string str, fgr::TextFormat format, int height) {
		add_pos.y += component_distance;
		Text* text = new Text(add_pos, glm::ivec2(width - border_distance * 2, height), str);
		text->addShadow(glm::ivec2(1.), shadow_color);
		text->setStyle(font, format, 2, 10, text_color_default);
		component_array->push_back(text);
		add_pos.y += height + component_distance;
	}

	void Generator::addButtonRow(std::vector<Button*> buttons) {
		int height = buttons[0]->size.y;
		glm::ivec2 ap = add_pos;
		int w = width - border_distance * 2;
		glm::ivec2 size = glm::ivec2((w - component_distance * (buttons.size() - 1)) / buttons.size(), height);
		for (int i = 0; i < buttons.size(); ++i) {
			ap = add_pos + glm::ivec2(((w - size.x) * i) / (buttons.size() - 1), 0);

			buttons[i]->pos = ap;
			buttons[i]->size = size;
			buttons[i]->setColors(text_color_default, text_color_highlight, text_color_press, text_color_error);
			buttons[i]->setStyle(button_default_style, button_highlight_style, button_press_style, font, fgr::TextFormat::center);

			component_array->push_back(buttons[i]);
		}

		add_pos.y += height + component_distance;
	}

	void Generator::terminate() {
		Face& face = *((Face*)(*component_array)[start_index]);
		face.size = glm::ivec2(width, add_pos.y - origin_pos.y + border_distance - component_distance);
	}
}