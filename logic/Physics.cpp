#include "Physics.h"

#include <iostream>

namespace flo {
	Collider::Collider(std::vector<glm::vec2> vertices) :
	vertices(vertices) {
		utility_vertices.resize(vertices.size());
	}

	float cross2D(glm::vec2 a, glm::vec2 b) {
		return a.x * b.y - b.x * a.y;
	}

	glm::vec2 skew(glm::vec2 v) {
		return glm::vec2(v.y, -v.x);
	}

	float Collider::getMomentOfInertia(glm::vec2 scale) {
		float moi = 0.;
		float divisor = 0.;
		for (int i = 0; i < vertices.size(); ++i) {
			glm::vec2 p0 = vertices[i] * scale;
			glm::vec2 p1 = vertices[(i + 1) % vertices.size()] * scale;

			float cross = cross2D(p1, p0);
			moi += cross * (glm::dot(p0, p0) + glm::dot(p1, p0), glm::dot(p1, p1));
			divisor += cross;
		}
		return moi / (divisor * 6.);
	}

	Collision Collider::collide(Collider& other, glm::vec2 p0) {
		Collision result;
		for (int i = 0; i < utility_vertices.size(); ++i) {
			glm::vec2 d0 = utility_vertices[i] - p0;
			glm::vec2 n0 = skew(d0);
			for (int j = 0; j < other.utility_vertices.size(); ++j) {
				glm::vec2 p1 = other.utility_vertices[j];
				glm::vec2 d1 = other.utility_vertices[(j + 1) % other.utility_vertices.size()] - p1;
				glm::vec2 n1 = skew(d1);
				float a0 = glm::dot(p1 - p0, n0);
				float b0 = glm::dot(p1 - p0 + d1, n0);
				float a1 = glm::dot(p0 - p1, n1);
				float b1 = glm::dot(p0 - p1 + d0, n1);
				float f0 = a0 / (a0 - b0);
				float f1 = a1 / (a1 - b1);
				if (f0 >= 0.0f && f0 < 1.0f && f1 >= 0.0f && f1 < 1.0f) {
					float msqr = glm::dot(n1, n1);
					result.clip_vec = n1 * b1 / msqr;
					result.point = p0 + d0;
					return result;
				}
			}
		}
		return result;
	}

	float cross2(glm::vec2 a, glm::vec2 b) {
		return a.x * b.y - b.x * a.y;
	}

	PhysicsComponent::PhysicsComponent(float moment_of_inertia, float mass, glm::vec2 velocity, float angular_velocity) :
	moment_of_inertia(moment_of_inertia), mass(mass), velocity(velocity), angular_velocity(angular_velocity) {

	}

	void PhysicsComponent::assignComponents(Collider& _collider, TransformComponent& _transform) {
		collider = _collider;
		transform = &_transform;
	}

	void PhysicsComponent::applyForce(glm::vec2 pos, glm::vec2 f) {
		torque += cross2(pos - transform->pos, f);
		force += f;
	}

	void PhysicsComponent::runStep(float dt, flo::TransformComponent& transform) {
		glm::vec2 impulse = force * dt;
		float delta_omega = torque / moment_of_inertia * dt;
		angular_velocity += delta_omega;
		velocity += force * dt;

		transform.pos += velocity * dt;
		transform.angle += angular_velocity * dt;
		transform.angle = glm::mod(transform.angle, 3.141592653589f * 2.f);

		torque = 0;
		force = glm::vec2(0.);
	}

	void PhysicsComponent::handle_collision(PhysicsComponent& other, float dt) {
		glm::vec2 s0 = transform->size * 2.83f;
		glm::vec2 s1 = other.transform->size * 2.83f;
		glm::vec2 p0 = transform->pos - s0 * 0.5f;
		glm::vec2 p1 = other.transform->pos - s1 * 0.5f;
		if (p0.x + s0.x < p1.x || p0.x > p1.x + s1.x || p0.y + s0.y < p1.y || p0.y > p1.y + s1.y) return;
		
		collectVertices();
		other.collectVertices();

		Collision collision = collider.collide(other.collider, transform->pos);
		if (collision.clip_vec == glm::vec2(0.)) collision = other.collider.collide(collider, other.transform->pos);
		if (collision.clip_vec != glm::vec2(0.)) {
			transform->pos -= collision.clip_vec;
			glm::vec2 impulse0 = impulse_vector(collision.point);
			glm::vec2 impulse1 = other.impulse_vector(collision.point);
			glm::vec2 force = (impulse1 + impulse0) / dt;
			applyForce(collision.point, -force);
			other.applyForce(collision.point, force);
		}
	}

	glm::vec2 PhysicsComponent::impulse_vector(glm::vec2 pos) {
		glm::vec2 r = pos - transform->pos;
		float rsqr = glm::dot(r, r);
		return velocity * mass/* + moment_of_inertia * angular_velocity * r / rsqr*/;
	}

	void PhysicsComponent::collectVertices() {
		glm::vec2 rot_vec = glm::vec2(glm::cos(transform->angle), glm::sin(transform->angle));
		for (int i = 0; i < collider.vertices.size(); ++i) {
			glm::vec2 v = collider.vertices[i] * transform->size;
			glm::vec2 r = glm::vec2(v.x * rot_vec.x - v.y * rot_vec.y, v.x * rot_vec.y + v.y * rot_vec.x);
			collider.utility_vertices[i] = r + transform->pos;
		}
	}

	void PhysicsSystem::onRegistered() {
		parent_ecs->registerComponent(flo::uniqueCode<TransformComponent>());
		parent_ecs->registerComponent(flo::uniqueCode<PhysicsComponent>());
		
		physics_components = ComponentBundleArray(TYPE_VECTOR(TYPEHASH(TransformComponent), TYPEHASH(PhysicsComponent)), *parent_ecs);
	}

	void PhysicsSystem::entityAdded(Entity entity) {
		physics_components.onAdded(entity);
	}

	void PhysicsSystem::entityDestroyed(Entity entity) {
		physics_components.onRemoved(entity);
	}

	void PhysicsSystem::update(float dt) {
		for (int i = 0; i < physics_components.size(); ++i) {
			TransformComponent* tc = (TransformComponent*)physics_components.getComponent(0, i);
			PhysicsComponent* pc = (PhysicsComponent*)physics_components.getComponent(1, i);

			for (int j = i + 1; j < physics_components.size(); ++j) {
				PhysicsComponent* pc2 = (PhysicsComponent*)physics_components.getComponent(1, j);
				pc->handle_collision(*pc2, dt);
			}

			pc->runStep(dt, *tc);
		}
	}
}