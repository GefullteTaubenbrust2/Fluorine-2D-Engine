#include "ECS.h"

namespace flo {
	const u32 invalid_index = -1;
	const int data_chunk_size = 8192;

	Entity to2Power(Entity e) {
		Entity e2 = 1, e3 = 1;
		while (e2 < e) {
			e3 = e2;
			e2 <<= 1;
		}
		return e3;
	}

	ComponentArray::ComponentArray(typehash type) : 
	data_type(type) {
	}

	void flo::ComponentArray::addComponent(Entity entity, u8* d) {
		data.push_back(d);
		map.push_back(entity);
	}

	void flo::ComponentArray::removeComponent(Entity entity) {
		u32 index = findEntity(entity);
		if (index == invalid_index) return;
		map.erase(map.begin() + index);
		data.erase(data.begin() + index);
	}

	u32 flo::ComponentArray::findEntity(Entity entity) {
		if (map.size() < 1) return -1;
		if (map[0] > entity || map[map.size() - 1] < entity) return invalid_index;
		int half_s = to2Power(map.size());
		int offset = 0;
		for (; half_s >= 1; half_s /= 2) {
			const uint in0 = offset;
			const uint in1 = offset + half_s;
			if (in1 < map.size()) {
				if (map[in1] < entity) offset += half_s;
				else if (map[in1] == entity) return offset + half_s;
			}
			if (map[in0] > entity) return invalid_index;
			else if (map[in0] == entity) return offset;
		}
		return invalid_index;
	}

	u8* flo::ComponentArray::getData(Entity entity) {
		u32 index = findEntity(entity);
		if (index == invalid_index) return nullptr;
		return data[index];
	}

	flo::EntityComponentSystem::EntityComponentSystem() {
	}

	void flo::EntityComponentSystem::registerComponent(typehash type) {
		componentArrays.insert(std::make_pair(type, ComponentArray(type)));
	}

	void EntityComponentSystem::registerSystem(System* system) {
		systems.push_back(system);
		system->parent_ecs = this;
		system->onRegistered();
	}

	Entity EntityComponentSystem::registerEntity() {
		++current_entity;
		return current_entity;
	}

	void EntityComponentSystem::removeEntity(Entity entity) {
		for (auto iter = componentArrays.begin(); iter != componentArrays.end(); ++iter) {
			iter->second.removeComponent(entity);
		}
		for (int i = 0; i < systems.size(); ++i) {
			systems[i]->entityDestroyed(entity);
		}
	}

	void flo::EntityComponentSystem::addComponent(typehash type, void* data) {
		ComponentArray* arr = getComponentArray(type);
		if (!arr) return;
		arr->addComponent(current_entity, (u8*)data);
	}

	void EntityComponentSystem::finalizeEntity() {
		for (int i = 0; i < systems.size(); ++i) {
			systems[i]->entityAdded(current_entity);
		}
	}

	ComponentArray* flo::EntityComponentSystem::getComponentArray(typehash type) {
		auto arr = componentArrays.find(type);
		if (arr == componentArrays.end()) return nullptr;
		return &(arr->second);
	}

	u8* flo::EntityComponentSystem::getComponent(typehash component_type, Entity entity) {
		ComponentArray* arr = getComponentArray(component_type);
		if (!arr) return nullptr;
		return arr->getData(entity);
	}

	u8* EntityComponentSystem::getComponentFromAdded(typehash type) {
		ComponentArray* arr = getComponentArray(type);
		if (!arr) return nullptr;
		return arr->getData(current_entity);
	}

	ComponentBundleArray::ComponentBundleArray(const std::vector<typehash>& types, EntityComponentSystem& parent_ecs) :
	types(types), parent_ecs(&parent_ecs) {

	}

	void ComponentBundleArray::onAdded(Entity entity) {
		for (int i = 0; i < types.size(); ++i) {
			u8* comp = parent_ecs->getComponentFromAdded(types[i]);
			if (!comp) break;
			data.push_back(comp);
			if (i == types.size() - 1) map.push_back(entity);
		}
		data.resize(map.size() * types.size());
	}

	void ComponentBundleArray::onRemoved(Entity entity) {
		for (int i = 0; i < map.size(); ++i) {
			if (map[i] == entity) {
				data.erase(data.begin() + i * types.size(), data.begin() + (i + 1) * types.size());
			}
		}
	}

	u8* ComponentBundleArray::getComponent(int type_index, int index) {
		if (type_index >= types.size() || index >= map.size()) return nullptr;
		return data[index * types.size() + type_index];
	}

	uint ComponentBundleArray::size() {
		return map.size();
	}

	glm::mat3 TransformComponent::getMatrix() {
		return flo::scale_rotate_and_translate(size, pos, glm::vec2(glm::cos(angle), glm::sin(angle)));
	}

	glm::mat3 TransformComponent::getSpriteMatrix() {
		return flo::generate_tsrt_matrix(glm::vec2(-0.5), size * 2.f, glm::vec2(glm::cos(angle), glm::sin(angle)), pos);
	}
}
