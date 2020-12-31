#include "Input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Time.h"

#include "TextWriting.h"

namespace flo {
	int monitored_keys[350];
	int monitored_count;

	i16 keys[350];
	i8 mouse_buttons[3];
	glm::dvec2 mouse_position;
	glm::vec2 scroll;
	glm::vec2 scroll_result;

	u32 input_key_group = 0, key_group_current = 0;

	u32 uniqueKeyGroup() {
		++key_group_current;
		return key_group_current;
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		scroll += glm::vec2(xoffset, yoffset);
	}

	void update_input(GLFWwindow* window) {
		for (int i = 0; i < monitored_count; ++i) {
			int result = 0;
			int key = monitored_keys[i];
			if (i >= 350) break;
			if (glfwGetKey(window, key) == GLFW_PRESS) {
				result |= InputType::held;
				if (!keys[i]) result = InputType::hit;
			}
			else {
				if (keys[i] & InputType::held) result |= InputType::released;
			}
			keys[i] = result;
		}

		for (int i = 0; i < 3; ++i) {
			int result = 0;
			if (glfwGetMouseButton(window, i) == GLFW_PRESS) {
				result |= InputType::held;
				if (!mouse_buttons[i]) result = InputType::hit;
			}
			else {
				if (mouse_buttons[i] & InputType::held) result |= InputType::released;
			}
			mouse_buttons[i] = result;
		}

		scroll_result = scroll;
		scroll = glm::vec2(0.0, 0.0);

		if (editor::isActive()) {
			editor::update();
		}

		glfwGetCursorPos(window, &mouse_position.x, &mouse_position.y);
	}

	InputType getKey(const uint key) {
		int result = 0;
		if (key < 0 || key >= 350) return (InputType)result;
		return (InputType)keys[key];
	}

	void setInputString(std::string* str, bool* exit_state) {
		if (str == editor::entered_string) return;
		editor::entered_string = str;
		editor::entered_clone = *str;
		editor::exit_state = exit_state;
		if (exit_state) *exit_state = false;
		editor::cursor_pos = str->size();
		editor::update_parent_string();
	}

	InputType getMouseButton(const uint mouse_button) {
		int result = 0;
		if (mouse_button < 3) {
			result = mouse_buttons[mouse_button];
		}
		return (InputType)result;
	}

	const glm::vec2 getMousePosition() {
		return glm::vec2(mouse_position.x, mouse_position.y);
	}

	const glm::vec2 getScroll() {
		return scroll_result;
	}

	void setEditorKey(const i16* to_monitor, const int size, u32* var, u32 target) {
		for (int i = 0; i < size; ++i) {
			if (to_monitor[i] == target) {
				*var = i;
				return;
			}
		}
		monitored_keys[monitored_count] = target;
		*var = monitored_count++;
	}

	void setInputKeys(const i16* to_monitor, const int size) {
		monitored_count = size;
		for (int i = 0; i < size; ++i) {
			monitored_keys[i] = to_monitor[i];
		}
	}

	uint getKeyEnforced(i16 key) {
		for (int i = 0; i < monitored_count; ++i) {
			if (monitored_keys[i] == key) return i;
		}
		monitored_keys[monitored_count] = key;
		++monitored_count;
		return monitored_count - 1;
	}
}