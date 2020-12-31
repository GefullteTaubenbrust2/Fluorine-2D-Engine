#pragma once
#include "Panel.h"
#include "Face.h"
#include "Interactables.h"
#include "Text.h"

namespace fui {
	struct Generator {
		std::vector<Component*>* component_array;
		FaceStyle
			panel_style,
			top_style,
			button_default_style,
			button_highlight_style,
			button_press_style,
			textfield_default_style,
			textfield_highlight_style,
			textfield_press_style,
			bar_style;
		int border_distance, component_distance, width;
		int slider_bar_size;
		uint start_index = 0;
		glm::ivec2 origin_pos, add_pos;
		fgr::BitmapFont font;
		glm::vec4 text_color_default, text_color_highlight, text_color_press, text_color_error, shadow_color;
		fgr::TextureStorage texture;

		void initDefaultStyles();

		void startPanel(std::vector<Component*>& components, glm::ivec2 pos, int width, int border_distance, int component_distance, std::string title, bool drag);

		void addTextField(int height, std::string* str);

		void addButton(int height, std::string str, void(*event)(flo::InputType type));

		void addSlider(glm::ivec2 face_size, float* value);

		void addText(std::string str, fgr::TextFormat format, int height);

		void addButtonRow(std::vector<Button*> buttons);

		void terminate();
	};
}