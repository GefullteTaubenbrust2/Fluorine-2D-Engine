#pragma once
#include "Input.h"
#include "Clipboard.h"

namespace flo {
	namespace editor {
		std::string* entered_string = nullptr;
		std::string entered_clone;
		int cursor_pos;
		bool* exit_state = nullptr;
		int back_required = 0, left_required = 0, right_required = 0, delete_required = 0, up_required, down_required;
		Stopclock time;

		u32 key_enter, key_escape, key_left, key_right, key_up, key_down, key_ctrl, key_backspace, key_delete, key_v, key_c;

		void init_textinput() {
			key_enter = getKeyEnforced(GLFW_KEY_ENTER);
			key_escape = getKeyEnforced(GLFW_KEY_ESCAPE);
			key_left = getKeyEnforced(GLFW_KEY_LEFT);
			key_right = getKeyEnforced(GLFW_KEY_RIGHT);
			key_up = getKeyEnforced(GLFW_KEY_UP);
			key_down = getKeyEnforced(GLFW_KEY_DOWN);
			key_ctrl = getKeyEnforced(GLFW_KEY_LEFT_CONTROL);
			key_backspace = getKeyEnforced(GLFW_KEY_BACKSPACE);
			key_delete = getKeyEnforced(GLFW_KEY_DELETE);
			key_v = getKeyEnforced(GLFW_KEY_V);
			key_c = getKeyEnforced(GLFW_KEY_C);
		}

		void update_parent_string() {
			*entered_string = entered_clone;
			entered_string->insert(entered_string->begin() + cursor_pos, '|');
		}

		bool whitespace(char c) {
			return c == ' ' || c == '\n';
		}

		void backspace() {
			if (getKey(key_ctrl)) {
				int last = cursor_pos - 1;
				for (; last >= 0; --last) {
					if (whitespace(entered_clone[last])) {
						break;
					}
				}
				if (last < 0) last = 0;
				entered_clone.erase(last, cursor_pos - last);
				cursor_pos = last;
			}
			else {
				if (cursor_pos > entered_clone.size()) cursor_pos = entered_clone.size();
				if (cursor_pos < 0) cursor_pos = 0;
				if (cursor_pos > 0) {
					entered_clone.erase(entered_clone.begin() + cursor_pos - 1);
					--cursor_pos;
				}
			}
			update_parent_string();
		}

		void fdelete() {
			if (getKey(key_ctrl)) {
				int last = cursor_pos;
				for (; last < entered_clone.size(); ++last) {
					if (whitespace(entered_clone[last])) {
						++last;
						break;
					}
				}
				if (last >= entered_clone.size()) last = entered_clone.size();
				entered_clone.erase(cursor_pos, last - cursor_pos);
			}
			else {
				if (cursor_pos > entered_clone.size()) cursor_pos = entered_clone.size();
				if (cursor_pos < 0) cursor_pos = 0;
				if (cursor_pos < entered_clone.size()) {
					entered_clone.erase(entered_clone.begin() + cursor_pos);
				}
			}
			update_parent_string();
		}

		void move_left() {
			if (cursor_pos > 0) --cursor_pos;
			if (getKey(key_ctrl)) {
				int last = cursor_pos;
				for (; last >= 0; --last) {
					if (whitespace(entered_clone[last])) {
						break;
					}
				}
				if (last < 0) last = 0;
				cursor_pos = last;
			}
			update_parent_string();
		}

		void move_right() {
			if (cursor_pos < entered_clone.size()) ++cursor_pos;
			if (getKey(key_ctrl)) {
				int last = cursor_pos;
				for (; last < entered_clone.size(); ++last) {
					if (whitespace(entered_clone[last])) {
						break;
					}
				}
				if (last >= entered_clone.size()) last = entered_clone.size();
				cursor_pos = last;
			}
			update_parent_string();
		}

		void move_up() {
			int last = cursor_pos - 1;
			for (; last >= 0; --last) {
				if (entered_clone[last] == '\n') {
					break;
				}
			}
			if (last < 0) last = 0;
			cursor_pos = last;
			update_parent_string();
		}

		void move_down() {
			int last = cursor_pos + 1;
			for (; last < entered_clone.size(); ++last) {
				if (entered_clone[last] == '\n') {
					break;
				}
			}
			if (last >= entered_clone.size()) last = entered_clone.size();
			cursor_pos = last;
			update_parent_string();
		}

		void exit() {
			if (exit_state) *exit_state = true;
			*entered_string = entered_clone;
			entered_string = nullptr;
		}

		bool isActive() {
			return entered_string;
		}

		void character_callback(GLFWwindow* window, uint codepoint) {
			if (codepoint < 128 && entered_string) {
				entered_clone.insert(entered_clone.begin() + cursor_pos, (uchar)codepoint);
				++cursor_pos;
				update_parent_string();
			}
		}

		void update() {
			int t = time.stop().asSeconds() * 32;
			if (getKey(key_left) & InputType::hit) {
				move_left();
				left_required = time.stop().asSeconds() * 32 + 16;
			}
			else if (getKey(key_left)) {
				if (t > left_required) {
					left_required = t;
					move_left();
				}
			}
			if (getKey(key_right) & InputType::hit) {
				move_right();
				update_parent_string();
				right_required = time.stop().asSeconds() * 32 + 16;
			}
			else if (getKey(key_right)) {
				if (t > right_required) {
					right_required = t;
					move_right();
					update_parent_string();
				}
			}
			if (getKey(key_enter) & InputType::hit) {
				entered_clone.insert(entered_clone.begin() + cursor_pos, '\n');
				++cursor_pos;
				update_parent_string();
			}
			else if (getKey(key_escape) & InputType::hit) {
				exit();
				return;
			}
			if (getKey(key_backspace) & InputType::hit) {
				backspace();
				back_required = time.stop().asSeconds() * 32 + 16;
			}
			else if (getKey(key_backspace)) {
				if (t > back_required) {
					back_required = t;
					backspace();
				}
			}
			if (getKey(key_delete) & InputType::hit) {
				fdelete();
				back_required = time.stop().asSeconds() * 32 + 16;
			}
			else if (getKey(key_delete)) {
				if (t > back_required) {
					back_required = t;
					fdelete();
				}
			}
			if (getKey(key_up) & InputType::hit) {
				move_up();
				up_required = time.stop().asSeconds() * 32 + 16;
			}
			else if (getKey(key_up)) {
				if (t > up_required) {
					up_required = t;
					move_up();
				}
			}
			if (getKey(key_down) & InputType::hit) {
				move_down();
				down_required = time.stop().asSeconds() * 32 + 16;
			}
			else if (getKey(key_down)) {
				if (t > down_required) {
					down_required = t;
					move_down();
				}
			}
			if (getKey(key_v) & InputType::hit && getKey(key_ctrl)) {
				std::string str = flo::getFromClipboard();
				entered_clone.insert(cursor_pos, str.data());
				cursor_pos += str.size();
				update_parent_string();
			}
			if (getKey(key_c) & InputType::hit && getKey(key_ctrl)) {
				flo::copyToClipboard(entered_clone);
			}
		}
	}
}