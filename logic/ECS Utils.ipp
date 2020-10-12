#pragma once
#include "ECS Utils.h"

namespace flo {
	void BasisSystem::onRegistered() {
		euclid_components.create(parent_ecs->getComponentArray<EuclidComponent>());
	}

	void BasisSystem::onAdded() {
		if (parent_ecs->newestHas<EuclidComponent>()) euclid_components.add(parent_ecs->current_signature);
	}

	void BasisSystem::update(float delta_time) {
		for (int i = 0; i < euclid_components.indices.size(); ++i) {
			EuclidComponent& comp = *euclid_components.getComponent(i);
			comp.position += comp.velocity * delta_time;
			comp.angle += comp.angular_velocity * delta_time;
		}
	}

	void BasisSystem::appendInheritable(InheritableEntity* inheritable) {
		inheritable->entity = parent_ecs->current_signature;
		inheritable->parent_ecs = parent_ecs;
		inheritables.insert(std::make_pair(parent_ecs->current_signature, inheritable));
	}

	void BasisSystem::onDestroyed(flo::Entity entity) {
		inheritables.erase(entity);
		//euclid_components.onRemove(entity, )
	}
}