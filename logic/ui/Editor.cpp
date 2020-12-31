#include "Editor.h"

#include "../Input.h"
#include "../../graphics/VertexArray.h"

#include "../Matrices.h"

#include "../../graphics/Window.h"

#include "../ScriptingUtils.h"

#include <sstream>

namespace fui {
	struct EditorSystem : public flo::System {
		flo::ComponentBundleArray cba;

		EditorSystem() = default;

		virtual void onRegistered() override {
			parent_ecs->registerComponent(TYPEHASH(flo::TransformComponent));
			cba = flo::ComponentBundleArray(std::vector<typehash>{TYPEHASH(flo::TransformComponent)}, * parent_ecs);
		}

		virtual void entityAdded(flo::Entity entity) override {
			cba.onAdded(entity);
		}

		virtual void entityDestroyed(flo::Entity entity) override {
			cba.onRemoved(entity);
		}

		flo::TransformComponent* update(glm::vec2 mp) {
			for (int i = 0; i < cba.size(); ++i) {
				flo::TransformComponent& tc = *((flo::TransformComponent*)cba.getComponent(0, i));
				glm::vec2 p0 = tc.pos - tc.size;
				glm::vec2 p1 = tc.pos + tc.size;
				if (mp.x > p0.x && mp.x < p1.x && mp.y > p0.y && mp.y < p1.y) {
					return &tc;
				}
			}
			return nullptr;
		}
	};

	namespace editor {
		fgr::VertexArray cursor_va, outline_va;
		u32 key_group;

		int state;
		flo::TransformComponent* tc;

		glm::vec2 cursor_original_pos;
		glm::mat3 matrix = glm::mat3(1.);
		float* angle, angle_original;
		glm::vec2* scale, scale_original;
		glm::vec2* pos, pos_original;

		glm::vec2 script_size = glm::vec2(1), script_pos = glm::vec2(0);
		float script_angle = 0;

		int stage = 0;
		bool revert;

		glm::vec2 upos, uscale;
		float uangle;

		uint key_r, key_s, key_g, key_escape;

		EditorSystem es;

		Panel panel;
		std::string component_code;
		Component* textfield;

		flo::ScriptInterpreter interpreter;
		flo::Script edit_script;
	}

	using namespace editor;

	void build_and_run(flo::InputType type) {
		if (type == flo::InputType::released) {
			interpreter.interpretScript(component_code, edit_script, true);
			edit_script.runScript();
			if (tc) {
				tc->pos = script_pos;
				tc->size = script_size;
				tc->angle = script_angle;
			}
		}
	}

	void resetCode(flo::InputType type) {
		if (type == flo::InputType::released) {
			std::ostringstream oss;
			if (tc) {
				oss << "pos = vec2(" << tc->pos.x << ", " << tc->pos.y << ")\nsize = vec2(" << tc->size.x << ", " << tc->size.y << ")\nangle = " << tc->angle << "\n";
				if (textfield) ((TextField*)textfield)->setText(oss.str());
			}
			else {
				if (textfield) ((TextField*)textfield)->setText(
					"pos = vec2(0, 0)\n"
					"size = vec2(1, 1)\n"
					"angle = 0\n"
				);
			}
		}
	}

	void init_editor(Generator& gen, Panel& pan) {
		key_group = flo::uniqueKeyGroup();

		cursor_va.init();
		cursor_va.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(0.9, 0.04, 0.5), glm::vec2(0.0), glm::vec4(1., 0.3, 0.3, 1.)),
			fgr::Vertex(glm::vec3(0.9, 0.0, 0.5), glm::vec2(0.0), glm::vec4(1., 0.3, 0.3, 1.)),
			fgr::Vertex(glm::vec3(0.9, -0.04, 0.5), glm::vec2(0.0), glm::vec4(1., 0.3, 0.3, 1.)),
			fgr::Vertex(glm::vec3(1., 0.0, 0.5), glm::vec2(0.0), glm::vec4(1., 0.3, 0.3, 1.)),
			fgr::Vertex(glm::vec3(0.9, 0.04, 0.5), glm::vec2(0.0), glm::vec4(1., 0.3, 0.3, 1.)),
			fgr::Vertex(glm::vec3(0.9, 0.0, 0.5), glm::vec2(0.0), glm::vec4(1., 0.3, 0.3, 1.)),
			fgr::Vertex(glm::vec3(0.0001, 0.0, 0.5), glm::vec2(0.0), glm::vec4(1., 0.3, 0.3, 1.)),
			fgr::Vertex(glm::vec3(0.0, 0.0001, 0.5), glm::vec2(0.0), glm::vec4(0.3, 0.5, 1.0, 1.)),
			fgr::Vertex(glm::vec3(0.0, 0.9, 0.5), glm::vec2(0.0), glm::vec4(0.3, 0.5, 1.0, 1.)),
			fgr::Vertex(glm::vec3(0.04, 0.9, 0.5), glm::vec2(0.0), glm::vec4(0.3, 0.5, 1.0, 1.)),
			fgr::Vertex(glm::vec3(0.0, 1., 0.5), glm::vec2(0.0), glm::vec4(0.3, 0.5, 1.0, 1.)),
			fgr::Vertex(glm::vec3(-0.04, 0.9, 0.5), glm::vec2(0.0), glm::vec4(0.3, 0.5, 1.0, 1.)),
			fgr::Vertex(glm::vec3(0.0, 0.9, 0.5), glm::vec2(0.0), glm::vec4(0.3, 0.5, 1.0, 1.)),
			fgr::Vertex(glm::vec3(0.04, 0.9, 0.5), glm::vec2(0.0), glm::vec4(0.3, 0.5, 1.0, 1.)),
		}.data(), 14);

		outline_va.init();
		outline_va.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(-1., -1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
			fgr::Vertex(glm::vec3(-1., -1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
			fgr::Vertex(glm::vec3(1., -1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
			fgr::Vertex(glm::vec3(1., 1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
			fgr::Vertex(glm::vec3(-1., 1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
			fgr::Vertex(glm::vec3(-1., -1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
			fgr::Vertex(glm::vec3(1., -1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
			fgr::Vertex(glm::vec3(1., -1., 0.5), glm::vec2(0.0), glm::vec4(0.2, 0.8, 1., 1.)),
		}.data(), 8);

		key_r = flo::getKeyEnforced(GLFW_KEY_R);
		key_s = flo::getKeyEnforced(GLFW_KEY_S);
		key_g = flo::getKeyEnforced(GLFW_KEY_G);
		key_escape = flo::getKeyEnforced(GLFW_KEY_ESCAPE);

		panel.dispose();
		panel = pan;

		std::vector<Component*> components;
		gen.startPanel(components, glm::ivec2(0), 160, 8, 4, "Edit Component", true);
		gen.addText("Component Data", fgr::TextFormat::left, 8);
		gen.addTextField(150, &component_code);
		uint in = components.size() - 1;
		gen.addButtonRow(std::vector<Button*>{
			new Button(glm::ivec2(0.), glm::ivec2(0, 20), build_and_run, "Apply"),
			new Button(glm::ivec2(0.), glm::ivec2(0, 20), resetCode, "Reset"),
		});
		gen.terminate();
		textfield = components[in];
		panel.init(components);

		flo::registerUtilFunctions(interpreter);
		interpreter.registerVariable(flo::Variable(&script_size, GENERATE_TYPE(glm::vec2), false), "size");
		interpreter.registerVariable(flo::Variable(&script_pos, GENERATE_TYPE(glm::vec2), false), "pos");
		interpreter.registerVariable(flo::Variable(&script_angle, GENERATE_TYPE(float), false), "angle");
	}

	void editor_selection(flo::EntityComponentSystem& ecs) {
		stage = 1;
		es = EditorSystem();
		ecs.registerSystem(&es);
	}

	void edit_object(glm::vec2* npos, glm::vec2* nscale, float* nangle) {
		pos = npos;
		scale = nscale;
		angle = nangle;

		if (!npos) {
			upos = glm::vec2(0.);
			pos = &upos;
		}
		if (!nscale) {
			uscale = glm::vec2(1.);
			scale = &uscale;
		}
		if (!nangle) {
			uangle = 0.;
			angle = &uangle;
		}

		stage = 2;
		revert = false;

		resetCode(flo::InputType::released);
	}

	void update_editor(glm::mat3& transformations) {
		if (!stage) return;

		glm::vec2 mp = flo::getMousePosition();
		glm::vec2 mouse_pos = glm::inverse(transformations) * glm::vec3(mp.x / fgr::window::width * 2. - 1., -mp.y / fgr::window::height * 2. + 1., 1.);

		switch (stage) {
		case 1:
			tc = es.update(mouse_pos);
			if (tc && flo::getMouseButton(0) & flo::InputType::released) {
				edit_object(&tc->pos, &tc->size, &tc->angle);
				revert = true;
			}
			if (tc) {
				matrix = flo::scale_rotate_and_translate(tc->size, tc->pos, glm::vec2(std::cos(tc->angle), std::sin(tc->angle)));
			}
			if (stage == 1) break;
		case 2:
			glm::vec2 & pos = *editor::pos;
			glm::vec2& scale = *editor::scale;
			float& angle = *editor::angle;

			int next_state = -1;
			if (flo::getMouseButton(0) == flo::InputType::hit && !flo::input_key_group) next_state = 0;
			if (flo::getKey(key_g) == flo::InputType::hit && !flo::input_key_group) next_state = 1;
			if (flo::getKey(key_r) == flo::InputType::hit && !flo::input_key_group) next_state = 2;
			if (flo::getKey(key_s) == flo::InputType::hit && !flo::input_key_group) next_state = 3;
			if (flo::getKey(key_escape) == flo::InputType::hit && !flo::input_key_group) {
				state = 0;
				stage = revert ? 1 : 0;
			}

			if (next_state >= 0 && next_state != state) {
				cursor_original_pos = mouse_pos;
				pos_original = pos;
				scale_original = scale / glm::vec2(glm::length(pos - mouse_pos));
				angle_original = angle - std::atan2(mouse_pos.y - pos.y, mouse_pos.x - pos.x);
				state = next_state;
			}

			switch (state) {
			case 1:
				*editor::pos = pos_original + mouse_pos - cursor_original_pos;
				break;
			case 2:
				*editor::angle = angle_original + std::atan2(mouse_pos.y - pos.y, mouse_pos.x - pos.x);
				break;
			case 3:
				*editor::scale = glm::vec2(glm::length(pos - mouse_pos)) * scale_original;
				break;
			}

			matrix = flo::scale_rotate_and_translate(scale, pos, glm::vec2(std::cos(angle), std::sin(angle)));

			//panel.update(glm::ivec2(fgr::window::width, fgr::window::height), fgr::window::getDeltaTime());
		}
	}

	void render_editor(glm::mat3& transformations) {
		if (!stage) return;

		switch (stage) {
		case 1:
			if (!tc) return;
			fgr::Shader::line.setVec2(0, glm::vec2(fgr::window::width, fgr::window::height));
			fgr::Shader::line.setFloat(1, 4);
			outline_va.setTransformations(transformations * matrix);
			outline_va.draw(fgr::Shader::line, fgr::RendeMode::thick_line);
			break;
		case 2:
			fgr::Shader::line.setVec2(0, glm::vec2(fgr::window::width, fgr::window::height));
			fgr::Shader::line.setFloat(1, 4);
			cursor_va.setTransformations(transformations * matrix);
			cursor_va.draw(fgr::Shader::line, fgr::RendeMode::thick_line);

			panel.render();
			break;
		}
	}
}