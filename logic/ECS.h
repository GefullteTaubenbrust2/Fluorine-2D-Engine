#pragma once
#include <vector>
#include <typeindex>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../logic/Types.h"

namespace flo {
	typedef u64 Entity;

	template<typename T>
	constexpr unsigned int uniqueCode();

	Entity to2Power(Entity e);

	struct EntityComponentSystem;

	struct iComponentArray {
		long long update_id = 0;

		virtual void removeComponent(Entity entity) = 0;
	};

	template<typename ComponentType>
	struct SubComponentArray;

	template<typename ComponentType>
	struct ComponentArray : public iComponentArray {
		std::vector<ComponentType> components;
		std::vector<Entity> map;
		std::vector<SubComponentArray<ComponentType>*> sub_arrays;
		EntityComponentSystem* parent_ecs = nullptr;

		ComponentArray(EntityComponentSystem& parent_ecs);

		int find_index(Entity entity);

		void addComponent(ComponentType component, Entity entity);

		void removeComponent(Entity entity);

		void registerSubArray(SubComponentArray<ComponentType>& sub_array);

		ComponentType* findComponent(Entity entity);
	};

	template<typename ComponentType>
	struct SubComponentArray {
		ComponentArray<ComponentType>* parent_ica = nullptr;
		ComponentType* array = nullptr;
		std::vector<glm::u64vec2> indices;
		i64 update_id = 0;

		SubComponentArray() = default;

		void create(ComponentArray<ComponentType>* array);

		void add(Entity entity);

		void onAdded();

		void onRemove(Entity entity, unsigned int index);

		ComponentType* getComponent(unsigned int index);
	};

	struct System {
		EntityComponentSystem* parent_ecs = nullptr;

		System() = default;

		virtual void onRegistered() = 0;

		virtual void onAdded() = 0;

		virtual void onDestroyed(Entity entity) = 0;
	};

	struct EntityComponentSystem {
		Entity current_signature = -1;
		std::map<unsigned int, iComponentArray*> component_storage;
		std::vector<System*> systems;
		std::vector<i8> added_component_data;
		int current_components_eof = 0;
		std::vector<unsigned int> current_components_types;

		EntityComponentSystem();

		Entity registerNewEntity();

		void destroyEntity(Entity entity);

		void registerSystem(System* system);

		bool newestHasComponents(std::vector<unsigned int>& signatures);

		template<typename ComponentType>
		bool newestHas();

		template<typename ComponentType>
		void addComponentToNewest(ComponentType component);

		template<typename ComponentType>
		void registerComponent();

		void submitEntity();

		template<typename ComponentType>
		ComponentArray<ComponentType>* getComponentArray();

		template<typename ComponentType>
		ComponentType* getComponent(Entity entity);
	};
}

#include "ECS.ipp"