#pragma once
#include <gueepo2d.h>

class ScrollerComponent : public gueepo::Component {
public:
	gueepo::math::vec2 velocity;

	gueepo::math::vec2 internalPosition;

	void Initialize() {
		velocity.y = -10.0f;
		internalPosition = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
	}

	void Update(float DeltaTime) {
		gueepo::TransformComponent* transform = Owner->GetComponentOfType<gueepo::TransformComponent>();
		internalPosition.x += velocity.x * DeltaTime;
		internalPosition.y += velocity.y * DeltaTime;


		transform->position.x = gueepo::math::round(internalPosition.x);
		transform->position.y = gueepo::math::round(internalPosition.y);
	}
};