#include "Interactables.h"

namespace fui {
	u64 interactable_active = 0;
	u64 interactable_current = 1;

	PressDetector::PressDetector() {
		id = interactable_current;
		++interactable_current;
	}

	void PressDetector::update(glm::ivec2 mp, glm::ivec2 pos, glm::ivec2 size) {
		bool inside = mp.x >= pos.x && mp.y >= pos.y && mp.x < pos.x + size.x && mp.y < pos.y + size.y;
		
		if (interactable_active && interactable_active != id) inside = false;

		int pre_state = state;

		press_type = flo::InputType::none;
		if (inside) {
			if (flo::getMouseButton(0)) {
				state = 2;
				interactable_active = id;
				press_type = pre_state < 2 ? flo::InputType::hit : flo::InputType::held;
			}
			else {
				if (state == 2) {
					press_type = flo::InputType::released;
				}
				state = 1;
			}
		}
		else {
			if (state != 2) state = 0;
			else if (!flo::getMouseButton(0)) {
				state = 0;
			}
		}
		if (state != 2 && interactable_active == id) interactable_active = 0;
	}

	Button::Button(glm::ivec2 pos, glm::ivec2 size, void(*event)(flo::InputType), std::string str) :
	size(size), event(event) {
		Component::pos = pos;

		text.str = str;
	}

	uint Button::getSize() {
		return sizeof(Button);
	}

	void Button::setStyle(FaceStyle default_style, FaceStyle highlight_style, FaceStyle press_style, fgr::BitmapFont font, fgr::TextFormat format) {
		face.style = default_style;
		Button::default_style = default_style;
		Button::highlight_style = highlight_style;
		Button::press_style = press_style;

		std::string str = text.str;
		face = Face(pos, size, FaceStyle());
		text = Text(pos + glm::ivec2(0, size.y - font.char_size) / 2, glm::vec2(size.x, font.char_size), str);
		text.setStyle(font, format, 2, 10, glm::vec4(1.));
		text.addShadow(glm::ivec2(1), glm::vec4(0., 0., 0., 0.5));
	}

	void Button::setColors(glm::vec4 default_color, glm::vec4 highlight_color, glm::vec4 press_color, glm::vec4 disabled_color) {
		Button::default_color = default_color;
		Button::highlight_color = highlight_color;
		Button::press_color = press_color;
		Button::disabled_color = disabled_color;
	}

	void Button::init(Panel* panel) {
		face.parent_panel = panel;
		face.init(panel);

		default_style.apply(*parent_panel);
		highlight_style.apply(*parent_panel);
		press_style.apply(*parent_panel);

		text.parent_panel = panel;
		text.init(panel);
		text.update(0.);
	}

	void Button::update(float delta_time) {
		if (disabled) {
			detector.press_type = flo::InputType::none;
			detector.state = 0;
			return;
		}

		detector.update(parent_panel->mouse_pos, pos, size);

		if (detector.press_type && event) event(detector.press_type);
	}

	void Button::render() {
		bool state_change = detector.state != pre_state;
		if (state_change) {
			parent_panel->sprites.requestUpdate();
		}

		if (!disabled) {
			if (state_change) {
				switch (detector.state) {
				case 0:
					face.style = default_style;
					text.text_sprite.color = default_color;
					break;
				case 1:
					face.style = highlight_style;
					text.text_sprite.color = highlight_color;
					break;
				case 2:
					face.style = press_style;
					text.text_sprite.color = press_color;
					break;
				}
			}
			if (state_change) face.resetSprites();
			face.render();
		}
		else {
			face.style = press_style;
			face.render();
			text.text_sprite.color = disabled_color;
		}

		text.render();

		pre_state = detector.state;
	}

	void Button::dispose() {
		face.dispose();
		text.dispose();
	}

	TextField::TextField(glm::ivec2 pos, glm::ivec2 size, std::string* str) :
		size(size), str(str) {
		Component::pos = pos;

		face = Face(pos, size, FaceStyle());

		if (!str) {
			unique_string = true;
			TextField::str = new std::string();
		}
	}

	uint TextField::getSize() {
		return sizeof(TextField);
	}

	void TextField::setStyle(FaceStyle default_style, FaceStyle highlight_style, FaceStyle press_style, fgr::BitmapFont font, fgr::TextFormat format) {
		face.style = default_style;
		TextField::default_style = default_style;
		TextField::highlight_style = highlight_style;
		TextField::press_style = press_style;

		text = Text(pos + (glm::ivec2)(default_style.inner0 - default_style.top_left), size - (glm::ivec2)(default_style.bottom_right - default_style.inner1 + default_style.inner0 - default_style.top_left), *str);
		text.font = font;
		tformat = format;
	}

	void TextField::setColors(glm::vec4 default_color, glm::vec4 highlight_color, glm::vec4 press_color) {
		TextField::default_color = default_color;
		TextField::highlight_color = highlight_color;
		TextField::press_color = press_color;
	}

	void TextField::init(Panel* panel) {
		face.parent_panel = panel;

		default_style.apply(*parent_panel);
		highlight_style.apply(*parent_panel);
		press_style.apply(*parent_panel);

		face.init(panel);

		text.parent_panel = panel;
		text.setStyle(text.font, tformat, 2, 10, glm::vec4(1.));
		text.addShadow(glm::ivec2(1), glm::vec4(0., 0., 0., 0.5));
		text.init(panel);
		text.update(0.);

		key_group = flo::uniqueKeyGroup();
	}

	void TextField::update(float delta_time) {
		if (flo::input_key_group == key_group) {
			detector.press_type = flo::InputType::none;
			detector.state = 0;
			int scroll = flo::getScroll().y;
			if (scroll != 0) {
				fgr::BitmapText& t = text.text;
				t.line = glm::max(0, t.line - scroll);
				std::cout << t.line << '\n';
				text.text.update_string(*str);
				if (t.line > text.text.linebreaks.size()) t.line = text.text.linebreaks.size();
			}
		}
		else detector.update(parent_panel->mouse_pos, pos, size);

		if (detector.press_type == flo::InputType::released) {
			flo::setInputString(str, &text_entered);
			flo::input_key_group = key_group;
		}
		if (text_entered) {
			flo::input_key_group = 0;
		}

		if (*str != text.text.str) {
			text.text.update_string(*str);
		}
	}

	void TextField::setText(std::string _str) {
		if (str) *str = _str;
	}

	void TextField::render() {
		bool state_change = detector.state != pre_state;
		if (state_change) {
			parent_panel->sprites.requestUpdate();
		}

		if (state_change) {
			switch (detector.state) {
			case 0:
				face.style = default_style;
				text.text_sprite.color = default_color;
				break;
			case 1:
				face.style = highlight_style;
				text.text_sprite.color = highlight_color;
				break;
			case 2:
				face.style = press_style;
				text.text_sprite.color = press_color;
				break;
			}
			face.resetSprites();
		}
		face.render();

		text.render();

		pre_state = detector.state;
	}

	void TextField::dispose() {
		face.dispose();
		text.dispose();
		if (unique_string) delete str;
	}

	Slider::Slider(glm::ivec2 pos, glm::ivec2 face_size, int width, float* value) :
	face_size(face_size), width(width), value(value) {
		Slider::pos = pos;
		unique_value = !value;
		if (unique_value) Slider::value = new float(0);
		face = Face(pos, face_size, FaceStyle());
	}

	void Slider::setStyle(FaceStyle bar_style, FaceStyle default_style, FaceStyle highlight_style, FaceStyle press_style, int bar_height) {
		bar = Face(pos + glm::ivec2(0, (face_size.y - bar_height) / 2), glm::ivec2(width, bar_height), bar_style);
		face.style = default_style;

		Slider::default_style = default_style;
		Slider::highlight_style = highlight_style;
		Slider::press_style = press_style;
	}

	uint Slider::getSize() {
		return sizeof(Slider);
	}

	void Slider::init(Panel* panel) {
		default_style.apply(*parent_panel);
		highlight_style.apply(*parent_panel);
		press_style.apply(*parent_panel);

		bar.parent_panel = parent_panel;
		bar.init(panel);
		face.parent_panel = panel;
		face.init(panel);
	}

	void Slider::update(float delta_time) {
		float v = glm::clamp(*value, 0.f, 1.f);
		int xp = (width - face_size.x) * v;

		detector.update(parent_panel->mouse_pos, pos + glm::ivec2(xp, 0), face_size);
		int xpre = face.pos.x;
		face.pos.x = xp + pos.x;
		if (face.pos.x != xpre) face_moved = true;
		else face_moved = false;

		if (detector.press_type == flo::InputType::hit) {
			press_start = parent_panel->mouse_pos.x - pos.x - xp;
		}
		if (detector.state == 2) {
			*value = glm::clamp((parent_panel->mouse_pos.x - pos.x - press_start) / (width - face_size.x), 0.f, 1.f);
		}
	}

	void Slider::render() {
		if (detector.state != pre_state) {
			switch (detector.state) {
			case 0:
				face.style = default_style;
				break;
			case 1:
				face.style = highlight_style;
				break;
			case 2:
				face.style = press_style;
				break;
			}
			face.resetSprites();
		}
		else if (face_moved) face.resetSprites();

		pre_state = detector.state;

		bar.render();
		face.render();
	}

	void Slider::dispose() {
		bar.dispose();
		face.dispose();
		if (unique_value) delete value;
	}

	DragComponent::DragComponent(glm::ivec2 pos, glm::ivec2 size) :
	size(size) {
		DragComponent::pos = pos;
	}

	uint DragComponent::getSize() {
		return sizeof(DragComponent);
	}

	void DragComponent::init(Panel* panel) {
		
	}

	void DragComponent::update(float delta_time) {
		detector.update(parent_panel->mouse_pos, pos, size);

		if (detector.state == 2) {
			glm::vec2 mp = parent_panel->mouse_pos + (glm::vec2)parent_panel->pixel_offset;
			if (!dragging) {
				cursor_start = mp;
				panel_start = parent_panel->pixel_offset;
				dragging = true;
			}
			parent_panel->pixel_offset = panel_start + (glm::ivec2)(mp - cursor_start);
			parent_panel->sprites.requestUpdate();
		}
		else dragging = false;
	}

	void DragComponent::render() {
		
	}

	void DragComponent::dispose() {
		
	}
}
