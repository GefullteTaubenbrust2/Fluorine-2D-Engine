#pragma once
#include "ECS.h"

namespace flo {
	struct EuclidComponent {
		glm::vec2 position = glm::vec2(0.0);
		glm::vec2 velocity = glm::vec2(0.0);
		glm::vec2 scale = glm::vec2(1.0);
		float angle = 0.;
		float angular_velocity = 0.;

		EuclidComponent() = default;

		EuclidComponent(glm::vec2 scale, glm::vec2 position, glm::vec2 velocity, float angle, float angular_velocity) :
			position(position), velocity(velocity), angle(angle), angular_velocity(angular_velocity), scale(scale) {

		}

		glm::mat3 getMatrix() {
			return flo::scale_and_translate_and_rotate(scale, position, glm::vec2(std::cos(angle), std::sin(angle)));
		}
	};

	struct InheritableEntity {
		Entity entity = 0;
		EntityComponentSystem* parent_ecs;

		InheritableEntity() = default;
	};

	struct BasisSystem : public System {
		SubComponentArray<EuclidComponent> euclid_components;
		std::map<Entity, InheritableEntity*> inheritables;

		BasisSystem() = default;

		virtual void onRegistered() override;

		virtual void onAdded() override;

		void update(float delta_time);

		void appendInheritable(InheritableEntity* inheritable);

		virtual void onDestroyed(flo::Entity entity) override;
	};
}

#include "ECS Utils.ipp"