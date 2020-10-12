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

	namespace editor {
		extern u32 key_enter, key_escape, key_left, key_right, key_ctrl, key_backspace, key_delete, key_v, key_c, key_down, key_up;
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

	InputType getKey(const unsigned int key) {
		int result = 0;
		if (key < 0 || key >= 350) return (InputType)result;
		return (InputType)keys[key];
	}

	void setInputString(std::string* str, bool* exit_state) {
		editor::entered_string = str;
		editor::entered_clone = *str;
		editor::exit_state = exit_state;
		if (exit_state) *exit_state = false;
		editor::cursor_pos = str->size();
		editor::update_parent_string();
	}

	InputType getMouseButton(const unsigned int mouse_button) {
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
		setEditorKey(to_monitor, size, &editor::key_backspace, GLFW_KEY_BACKSPACE);
		setEditorKey(to_monitor, size, &editor::key_ctrl, GLFW_KEY_LEFT_CONTROL);
		setEditorKey(to_monitor, size, &editor::key_down, GLFW_KEY_DOWN);
		setEditorKey(to_monitor, size, &editor::key_enter, GLFW_KEY_ENTER);
		setEditorKey(to_monitor, size, &editor::key_escape, GLFW_KEY_ESCAPE);
		setEditorKey(to_monitor, size, &editor::key_left, GLFW_KEY_LEFT);
		setEditorKey(to_monitor, size, &editor::key_right, GLFW_KEY_RIGHT);
		setEditorKey(to_monitor, size, &editor::key_up, GLFW_KEY_UP);
		setEditorKey(to_monitor, size, &editor::key_delete, GLFW_KEY_DELETE);
		setEditorKey(to_monitor, size, &editor::key_v, GLFW_KEY_V);
		setEditorKey(to_monitor, size, &editor::key_c, GLFW_KEY_C);
		setEditorKey(to_monitor, size, &editor::key_up, GLFW_KEY_UP);
		setEditorKey(to_monitor, size, &editor::key_down, GLFW_KEY_DOWN);
	}
}