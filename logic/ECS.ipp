#pragma once
#include "ECS.h"

namespace flo {
	Entity to2Power(Entity e) {
		Entity e2 = 1, e3 = 1;
		while (e2 < e) {
			e3 = e2;
			e2 <<= 1;
		}
		return e3;
	}

	template<typename T>
	constexpr unsigned int uniqueCode() {
		return std::type_index(typeid(T)).hash_code();
	}

	template<typename ComponentType>
	int ComponentArray<ComponentType>::find_index(Entity entity) {
		if (map.size() < 1) return -1;
		if (map[0] > entity || map[map.size() - 1] < entity) return -1;
		int half_s = to2Power(map.size());
		int offset = 0;
		for (; half_s >= 1; half_s /= 2) {
			const unsigned int in0 = offset;
			const unsigned int in1 = offset + half_s;
			if (in1 < map.size()) {
				if (map[in1] < entity) offset += half_s;
				else if (map[in1] == entity) return offset + half_s;
			}
			if (map[in0] > entity) return -1;
			else if (map[in0] == entity) return offset;
		}
		return -1;
	}

	template<typename ComponentType>
	void ComponentArray<ComponentType>::addComponent(ComponentType component, Entity entity) {
		components.push_back(component);
		map.push_back(entity);
		for (int i = 0; i < sub_arrays.size(); ++i) {
			sub_arrays[i]->onAdded();
		}
		++update_id;
	}

	template<typename ComponentType>
	void ComponentArray<ComponentType>::removeComponent(Entity entity) {
		const int index = find_index(entity);
		if (index < 0) return;
		components.erase(components.begin() + index);
		map.erase(map.begin() + index);
		for (int i = 0; i < sub_arrays.size(); ++i) {
			sub_arrays[i]->onRemove(entity, index);
		}
		++update_id;
	}

	template<typename ComponentType>
	ComponentType* ComponentArray<ComponentType>::findComponent(Entity entity) {
		const int index = find_index(entity);
		if (index < 0) return nullptr;
		return components.data() + index;
	}

	template<typename ComponentType>
	void ComponentArray<ComponentType>::registerSubArray(SubComponentArray<ComponentType>& sub_array) {
		sub_arrays.push_back(&sub_array);
	}

	EntityComponentSystem::EntityComponentSystem() {

	}

	Entity EntityComponentSystem::registerNewEntity() {
		return ++current_signature;
	}

	void EntityComponentSystem::destroyEntity(Entity entity) {
		for (auto iter = component_storage.begin(); iter != component_storage.end(); ++iter) {
			iter->second->removeComponent(entity);
		}
		for (int i = 0; i < systems.size(); ++i) {
			systems[i]->onDestroyed(entity);
		}
	}

	void EntityComponentSystem::registerSystem(System* system) {
		systems.push_back(system);
		system->parent_ecs = this;
		system->onRegistered();
	}

	template<typename ComponentType>
	void EntityComponentSystem::addComponentToNewest(ComponentType component) {
		const int size = sizeof(ComponentType);
		const int eof = size + current_components_eof;
		if (eof > added_component_data.size()) {
			added_component_data.resize(eof);
		}
		/*std::copy((i8*)&component, ((i8*)&component) + size, added_component_data.data() + current_components_eof);
		current_components.push_back((i8*)added_component_data.data() + current_components_eof);
		current_components_eof = eof;*/
		current_components_types.push_back(uniqueCode<ComponentType>());

		ComponentArray<ComponentType>* ca = getComponentArray<ComponentType>();
		if (ca) {
			ca->addComponent(component, current_signature);
		}
	}

	template<typename ComponentType>
	void EntityComponentSystem::registerComponent() {
		unsigned int code = uniqueCode<ComponentType>();
		if (!component_storage.count(code)) {
			component_storage.insert(std::make_pair(code, new ComponentArray<ComponentType>(*this)));
		}
	}

	void EntityComponentSystem::submitEntity() {
		for (int i = 0; i < systems.size(); ++i) {
			systems[i]->onAdded();
		}
		current_components_types.clear();
		current_components_eof = 0;
	}

	template<typename ComponentType>
	ComponentArray<ComponentType>::ComponentArray(EntityComponentSystem& parent_ecs) : parent_ecs(&parent_ecs) {

	}

	template<typename ComponentType>
	ComponentArray<ComponentType>* EntityComponentSystem::getComponentArray() {
		const int signature = uniqueCode<ComponentType>();
		return (ComponentArray<ComponentType>*)component_storage[signature];
	}

	template<typename ComponentType>
	ComponentType* EntityComponentSystem::getComponent(Entity entity) {
		ComponentArray<ComponentType>* ca = getComponentArray<ComponentType>();
		if (ca) {
			return ca->findComponent(entity);
		}
		return nullptr;
	}

	bool EntityComponentSystem::newestHasComponents(std::vector<unsigned int>& signatures) {
		unsigned int index = 0;
		int iterated = 0;
		for (int i = 0; i < signatures.size(); ++i) {
			bool found = false;
			unsigned int signature = signatures[i];
			for (; iterated < current_components_types.size(); ++iterated) {
				if (current_components_types[index] == signature) {
					found = true;
					break;
				}
				++index;
				if (index >= current_components_types.size()) index = 0;
			}
			if (!found) return false;
			iterated = 0;
		}
		return true;
	}

	template<typename ComponentType>
	bool EntityComponentSystem::newestHas() {
		unsigned int signature = uniqueCode<ComponentType>();
		for (int i = 0; i < current_components_types.size(); ++i) {
			if (current_components_types[i] == signature) return true;
		}
		return false;
	}

	template<typename ComponentType>
	void SubComponentArray<ComponentType>::create(ComponentArray<ComponentType>* array) {
		parent_ica = array;
		array->registerSubArray(*this);
	}

	template<typename ComponentType>
	void SubComponentArray<ComponentType>::add(Entity entity) {
		int index = parent_ica->find_index(entity);
		if (index < 0) return;
		indices.push_back(glm::ivec2(index, entity));
	}

	template<typename ComponentType>
	void SubComponentArray<ComponentType>::onAdded() {
		array = parent_ica->components.data();
	}

	template<typename ComponentType>
	void SubComponentArray<ComponentType>::onRemove(Entity entity, unsigned int index) {
		if (index < 0) return;
		for (int i = 0; i < indices.size(); ++i) {
			Entity id = indices[i].y;
			unsigned int ind = indices[i].x;
			if (ind > index) {
				--indices[i].x;
			}
			if (id == entity) {
				indices.erase(indices.begin() + i);
				--i;
			}
		}
	}

	template<typename ComponentType>
	ComponentType* SubComponentArray<ComponentType>::getComponent(unsigned int index) {
		unsigned int ind = indices[index].x;
		return array + ind;
	}
}