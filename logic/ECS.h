#pragma once
#include <vector>
#include <map>

#include "Types.h"

#include "Matrices.h"

namespace flo {
	///<summary>
	/// A hash for entities.
	///</summary>
	typedef u64 Entity;

	extern const u32 invalid_index;

	///<summary>
	/// A struct for handling entities and components.
	///</summary>
	struct EntityComponentSystem;

	///<summary>
	/// A basic transform component.
	///</summary>
	struct TransformComponent {
		glm::vec2 pos = glm::vec2(0.), size = glm::vec2(0.);
		float angle = 0;

		TransformComponent() = default;
		
		///<summary>
		/// Get the matrix defined by the component.
		///</summary>
		glm::mat3 getMatrix();

		///<summary>
		/// Generate the matrix such that it accurately applies for a sprite.
		///</summary>
		glm::mat3 getSpriteMatrix();
	};

	///<summary>
	/// An abstract base struct for systems. Such systems handle behavior of entities.
	///</summary>
	struct System {
		EntityComponentSystem* parent_ecs;

		///<summary>
		/// A callback for when the System is registered.
		///</summary>
		virtual void onRegistered() = 0;

		///<summary>
		/// A callback for when an entity is added to the parent.
		///</summary>
		virtual void entityAdded(Entity entity) = 0;

		///<summary>
		/// A callback for when an entity is freed from the parent.
		///</summary>
		virtual void entityDestroyed(Entity entity) = 0;
	};

	///<summary>
	/// A container for components.
	///</summary>
	struct ComponentArray {
		///<summary>
		/// The type of the components.
		///</summary>
		typehash data_type;

		///<summary>
		/// References to all components as abstract pointers.
		///</summary>
		std::vector<u8*> data;

		///<summary>
		/// All hashes to which the components belong, ordered from smallest to greatest.
		///</summary>
		std::vector<Entity> map;

		///<summary>
		/// Create a ComponentArray. This is the only availibe constructor.
		///</summary>
		///<param name="type">The type of component that will be stored. This can also be a base class.</param>
		ComponentArray(typehash type);

		///<summary>
		/// Add a component. You do not need to call this manually.
		///</summary>
		///<param name="entity">The entity to which the component belongs.</param>
		///<param name="data">A pointer to the component.</param>
		void addComponent(Entity entity, u8* data);

		///<summary>
		/// Remove a component. You do not need to call this manually.
		///</summary>
		///<param name="entity">The entity to which the component belongs.</param>
		void removeComponent(Entity entity);
		
		///<summary>
		/// Find the index of a component in the array.
		///</summary>
		///<param name="entity">The entity to which the component belongs.</param>
		///<returns>The index of the component in the array if it is present, invalid_index otherwhise.</returns>
		u32 findEntity(Entity entity);

		///<summary>
		/// Get a component from the array.
		///</summary>
		///<param name="entity">The entity to which the component belongs.</param>
		///<returns>The component if found, nullptr otherwhise.</returns>
		u8* getData(Entity entity);
	};

	struct EntityComponentSystem {
		///<summary>
		/// A map containing all components of the ECS. WARNING: READ-ONLY!
		///</summary>
		std::map<typehash, ComponentArray> componentArrays;

		///<summary>
		/// All registered systems. WARNING: READ-ONLY!
		///</summary>
		std::vector<System*> systems;

		///<summary>
		/// The hash of the entity that is currently being created. WARNING: READ-ONLY!
		///</summary>
		Entity current_entity = 0;

		///<summary>
		/// Create an ECS.
		///</summary>
		EntityComponentSystem();

		///<summary>
		/// Register a type of component. This creates all storages neccessary.
		///</summary>
		///<param name="type">The type of component.</param>
		void registerComponent(typehash type);

		///<summary>
		/// Register a system. This system will now receive callbacks.
		///</summary>
		///<param name="system">A pointer to the system in question.</param>
		void registerSystem(System* system);

		///<summary>
		/// Register a new entity.
		///</summary>
		///<returns>The hash</returns>
		Entity registerEntity();

		///<summary>
		/// Destroy an entity.
		///</summary>
		///<param name="entity">The hash of the entity in question.</param>
		void removeEntity(Entity entity);

		///<summary>
		/// Add a component to the entity that is currently being created. Note that only one instance of a component type can be added.
		///</summary>
		///<param name="type">The type of component.</param>
		///<param name="data">A pointer to the component.</param>
		void addComponent(typehash type, void* data);

		///<summary>
		/// Call the entityAdded() callback for all systems with the newest entity.
		///</summary>
		void finalizeEntity();

		///<summary>
		/// Get a ComponentArray.
		///</summary>
		///<param name="type">The type of the array.</param>
		///<returns>A pointer to the array if found, nullptr otherwhise.</returns>
		ComponentArray* getComponentArray(typehash type);

		///<summary>
		/// Get a component.
		///</summary>
		///<param name="component_type">The type of component.</param>
		///<param name="entity">The entity to which the component belongs.</param>
		///<returns>A pointer to the component if found, nullptr otherwhise.</returns>
		u8* getComponent(typehash component_type, Entity entity);

		///<summary>
		/// Get a component from the newest entity.
		///</summary>
		///<param name="type">The type of component.</param>
		///<returns>A pointer to the component if found, nullptr otherwhise.</returns>
		u8* getComponentFromAdded(typehash type);
	};

	///<summary>
	/// An array that contains components of a set type. Only when all components are present in an entity can they appear in this array.
	///</summary>
	struct ComponentBundleArray {
		std::vector<typehash> types;
		std::vector<u8*> data;
		std::vector<Entity> map;
		EntityComponentSystem* parent_ecs = nullptr;

		ComponentBundleArray() = default;

		///<summary>
		/// Create a ComponentBundleArray
		///</summary>
		///<param name="types">The types of components to bundle.</param>
		///<param name="parent_ecs">The ecs to search.</param>
		ComponentBundleArray(const std::vector<typehash>& types, EntityComponentSystem& parent_ecs);

		///<summary>
		/// Check an entity for components. This must be called manually.
		///</summary>
		///<param name="entity">The entity to query.</param>
		void onAdded(Entity entity);

		///<summary>
		/// Remove an entity from the array. This must be called manually.
		///</summary>
		void onRemoved(Entity entity);

		///<summary>
		/// Get a specific component.
		///</summary>
		///<param name="type_indedx">The index of the type of component in the type array.</param>
		///<param name="index">The index of the bundle. Negative values will throw an exception.</param>
		///<returns>The component if found, nullptr otherwhise.</returns>
		u8* getComponent(int type_index, int index);

		///<summary>
		/// The size of the array.
		///</summary>
		///<returns>The size of the array.</returns>
		uint size();
	};
}