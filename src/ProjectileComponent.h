#pragma once
#include <gueepo2D.h>

// #todo: probably have to create a projectile manager or something that destroys projectiles after some time and when they are out of the camera
class ProjectileComponent : public gueepo::Component {
public:
	gueepo::math::vec2 velocity;
	float lifetime = 5.0f;

	void Initialize() {
		velocity.y = 300.0f;
		// multiplying the velocity by a random factor
		// I want to add/subtract 20% of the velocity according to random stuff
		/*
		bool bShouldAdd = gueepo::rand::Int() % 2 == 0;
		float randomVariation = gueepo::rand::Float() * (velocity.y * 0.1f);

		if (!bShouldAdd) {
			randomVariation *= -1;
		}

		velocity.y += randomVariation;
		*/
		Owner->SetLifetime(lifetime);

		gueepo::BoxCollider* box = Owner->GetComponentOfType<gueepo::BoxCollider>();
		box->OnCollisionEnter = COLLISION_CALLBACK(&ProjectileComponent::ProjectileOnCollisionEnter);
	}

	void Update(float DeltaTime) {
		gueepo::GameObject* gameobjOwner = static_cast<gueepo::GameObject*>(Owner);
		gameobjOwner->Translate(velocity * DeltaTime);
	}

	void ProjectileOnCollisionEnter(gueepo::BoxCollider* other) {
		gueepo::GameWorld::Kill(Owner);
		gueepo::GameWorld::Kill(other->Owner);
	}
};