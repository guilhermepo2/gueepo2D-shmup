#pragma once
#include <gueepo2D.h>

class ProjectileComponent : public gueepo::Component {
public:
	gueepo::math::vec2 velocity;
	float lifetime = 5.0f;
	bool friendly;

	void Initialize() {
		velocity.y = 1200.0f;
		Owner->SetLifetime(lifetime);

		gueepo::BoxCollider* box = Owner->GetComponentOfType<gueepo::BoxCollider>();
		box->OnCollisionEnter = COLLISION_CALLBACK(&ProjectileComponent::ProjectileOnCollisionEnter);
	}

	void Update(float DeltaTime) {
		gueepo::GameObject* gameobjOwner = static_cast<gueepo::GameObject*>(Owner);
		gameobjOwner->Translate(velocity * DeltaTime);
	}

	void ProjectileOnCollisionEnter(gueepo::BoxCollider* other) {
		if (friendly) {
			if (other->Owner->Name == "enemy") {
				gueepo::GameWorld::Kill(other->Owner);
				gueepo::GameWorld::Kill(Owner);
			}
		}
		else {
			if (other->Owner->Name == "player") {
				LOG_INFO("player took damage!");
				gueepo::GameWorld::Kill(Owner);
			}
		}
	}
};