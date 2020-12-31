#include "Panel.h"
#include "../../graphics/Texture.h"
#include "../Input.h"
#include "../../graphics/Window.h"

namespace fui {

	Panel::Panel(fgr::TextureStorage& texture, flo::ScaleMode scalemode, glm::ivec2 pixel_offset, glm::vec2 screen_pos) :
	scalemode(scalemode),
	pixel_offset(pixel_offset), 
	screen_pos(screen_pos),
	sprites(),
	texture(&texture),
	text_renderer() {

	}

	void Panel::init(std::vector<Component*>& comps) {
		sprites.init();
		text_renderer.init(*texture);

		sprites.texture_array = texture->id;

		components = comps;

		for (int i = 0; i < components.size(); ++i) {
			Component* comp = components[i];
			comp->parent_panel = this;
			comp->init(this);
		}
	}

	void Panel::update(float delta_time, glm::ivec2 screen_size) {
		glm::vec2 window_scale = flo::applyWindowScale(screen_size.x, screen_size.y, scalemode);
		pixel_size = flo::getPixelSize(screen_size.x, screen_size.y, scalemode);
		glm::ivec2 offset = pixel_offset + (glm::ivec2)(glm::vec2(fgr::window::width, fgr::window::height) * glm::vec2(0.5, -0.5) * screen_pos / (float)pixel_size);
		transform_matrix =
			flo::scale_and_translate(window_scale * glm::vec2(1., -1.), flo::centerWindowOrigin(screen_size.x, screen_size.y)) *
			flo::scale_and_translate(glm::vec2(1), offset);

		mouse_pos = flo::getMousePosition() / (float)pixel_size - (glm::vec2)(offset + screen_size / 2 / pixel_size);

		for (int i = components.size() - 1; i >= 0; --i) {
			components[i]->update(delta_time);
		}

		if (!initial_update) initial_update = 1;
	}

	void Panel::render() {
		sprites.resetBatch();
		sprites.setTransformations(transform_matrix);

		if (initial_update < 2) {
			sprites.requestUpdate();
			if (initial_update == 1) initial_update++;
		}

		for (int i = 0; i < components.size(); ++i) {
			components[i]->render();
		}

		sprites.drawBatch(fgr::Shader::sprites_instanced);
	}

	void Panel::dispose() {
		for (int i = 0; i < components.size(); ++i) {
			components[i]->dispose();
			delete components[i];
		}
	}

	Component::~Component() {}

	/*Manager::Manager(flo::ScaleMode scalemode) : 
	scalemode(scalemode) {

	}

	void Manager::init() {
		sprites.init();
		text_renderer.init();

		sprites.texture_array = text_renderer.rendered_buffer.id;
	}

	void Manager::update(glm::ivec2 screen_size) {
	}

	void Manager::start_render() {
		current_hash = 0;

		sprites.resetBatch();
	}

	void Manager::render(u32 hash) {
		if (current_hash >= hashes.size()) hashes.resize(hashes.size() + 8);
		if (hashes[current_hash] != hash) {
			sprites.requestUpdate();
			hashes[current_hash] = hash;
		}
		++current_hash;
	}

	void Manager::terminate_render() {
		sprites.setTransformations(transform_matrix);
		sprites.drawBatch(fgr::Shader::sprites_instanced);
	}*/
}