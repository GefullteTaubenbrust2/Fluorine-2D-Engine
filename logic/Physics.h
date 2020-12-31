#pragma once
#include "ECS.h"
#include "Matrices.h"

namespace flo {
	struct Collision {
		glm::vec2 clip_vec = glm::vec2(0.);
		glm::vec2 point = glm::vec2(0.);

		Collision() = default;
	};

	struct Collider {
		std::vector<glm::vec2> vertices;
		std::vector<glm::vec2> utility_vertices;

		Collider() = default;

		Collider(std::vector<glm::vec2> vertices);

		float getMomentOfInertia(glm::vec2 scale);

		Collision collide(Collider& other, glm::vec2 center_point);
	};

	struct PhysicsComponent {
		float angular_velocity = 0;
		glm::vec2 velocity = glm::vec2(0);
		float moment_of_inertia = 1, mass = 1;
		glm::vec2 force = glm::vec2(0.);
		float torque = 0.;
		TransformComponent* transform;
		Collider collider;

		PhysicsComponent() = default;

		PhysicsComponent(float moment_of_inertia, float mass, glm::vec2 velocity = glm::vec2(0.), float angular_velocity = 0.);

		void assignComponents(Collider& collider, TransformComponent& transform);

		void applyForce(glm::vec2 pos, glm::vec2 force);

		void runStep(float dt, flo::TransformComponent& transform);

		void handle_collision(PhysicsComponent& other, float dt);

		glm::vec2 impulse_vector(glm::vec2 pos);

		void collectVertices();
	};

	struct PhysicsSystem : public flo::System {
		flo::ComponentBundleArray physics_components;

		PhysicsSystem() = default;

		virtual void onRegistered() override;

		virtual void entityAdded(Entity entity) override;

		virtual void entityDestroyed(Entity entity) override;

		void update(float dt);
	};
}