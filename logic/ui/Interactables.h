#pragma once
#include "Face.h"
#include "Text.h"
#include "../Input.h"

namespace fui {
	struct PressDetector {
		flo::InputType press_type = flo::InputType::none;
		int state = 0;
		u64 id;
		
		PressDetector();

		void update(glm::ivec2 mp, glm::ivec2 pos, glm::ivec2 size);
	};

	struct Button : public Component {
		glm::ivec2 size;
		Text text;
		Face face;
		PressDetector detector;
		void(*event)(flo::InputType);
		fgr::TextFormat tformat;
		int pre_state = -1;
		FaceStyle default_style, highlight_style, press_style;
		glm::vec4 default_color = glm::vec4(1.), highlight_color = glm::vec4(1.), press_color = glm::vec4(0.5, 0.5, 0.5, 1.), disabled_color = glm::vec4(1., 0.5, 0.5, 1.);
		bool disabled = false;

		Button() = default;

		Button(glm::ivec2 pos, glm::ivec2 size, void(*event)(flo::InputType), std::string str);

		void setStyle(FaceStyle default_style, FaceStyle highlight_style, FaceStyle press_style, fgr::BitmapFont font, fgr::TextFormat format);

		void setColors(glm::vec4 default_color, glm::vec4 highlight_color, glm::vec4 press_color, glm::vec4 disabled_color);

		virtual uint getSize() override;

		virtual void init(Panel* panel) override;

		virtual void update(float delta_time) override;

		virtual void render() override;

		virtual void dispose() override;
	};

	struct TextField : public Component {
		glm::ivec2 size;
		Text text;
		Face face;
		PressDetector detector;
		fgr::TextFormat tformat;
		int pre_state = -1;

		FaceStyle default_style, highlight_style, press_style;
		glm::vec4 default_color = glm::vec4(1.), highlight_color = glm::vec4(1.), press_color = glm::vec4(0.5, 0.5, 0.5, 1.);

		std::string* str;
		bool unique_string = false;
		bool text_entered = false;
		u32 key_group;

		TextField() = default;

		TextField(glm::ivec2 pos, glm::ivec2 size, std::string* str);

		void setStyle(FaceStyle default_style, FaceStyle highlight_style, FaceStyle press_style, fgr::BitmapFont font, fgr::TextFormat format);

		void setColors(glm::vec4 default_color, glm::vec4 highlight_color, glm::vec4 press_color);

		virtual uint getSize() override;

		virtual void init(Panel* panel) override;

		virtual void update(float delta_time) override;

		void setText(std::string str);

		virtual void render() override;

		virtual void dispose() override;
	};

	struct Slider : public Component {
		Face bar, face;
		FaceStyle default_style, highlight_style, press_style;
		int width;
		glm::ivec2 face_size;

		PressDetector detector;
		int pre_state = 0;
		int press_start = 0;
		float* value = nullptr;
		bool face_moved = true;
		bool unique_value = false;

		Slider() = default;

		Slider(glm::ivec2 pos, glm::ivec2 face_size, int width, float* value);
		
		void setStyle(FaceStyle bar_style, FaceStyle default_style, FaceStyle highlight_style, FaceStyle press_style, int bar_height);

		virtual uint getSize() override;

		virtual void init(Panel* panel) override;

		virtual void update(float delta_time) override;

		virtual void render() override;

		virtual void dispose() override;
	};

	struct DragComponent : public Component {
		PressDetector detector;
		glm::ivec2 size;

		glm::vec2 cursor_start;
		glm::ivec2 panel_start;
		bool dragging = false;

		DragComponent(glm::ivec2 pos, glm::ivec2 size);

		virtual uint getSize() override;

		virtual void init(Panel* panel) override;

		virtual void update(float delta_time) override;

		virtual void render() override;

		virtual void dispose() override;
	};
}