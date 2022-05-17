#pragma once
#include <gueepo2d.h>

class EnemyComponent : public gueepo::Component {
public:
	gueepo::math::vec2 velocity;
	float speed;
	float shotCooldown;
	float timeToShoot;
	std::shared_ptr<gueepo::GameWorld> gameWorld;

	gueepo::Texture* projectileTexture;
	gueepo::math::vec2 projectileMinVec;
	gueepo::math::vec2 projectileMaxVec;

	void Initialize() {
		speed = 100.0f;
		velocity.x = speed;
		timeToShoot = 1.5f;
		shotCooldown = timeToShoot;
	}

	void Update(float DeltaTime) override {
		// Shooting
		shotCooldown -= DeltaTime;

		if (shotCooldown < 0.0f) {
			LOG_INFO("shooting!");

			gueepo::GameObject* proj = gameWorld->CreateGameObject(projectileTexture, "projectile");
			proj->transform->position = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
			proj->transform->position = proj->transform->position + gueepo::math::vec2(8.0f, -24.0f);
			proj->sprite->RebuildSourceRectangle(projectileMinVec, projectileMaxVec);
			proj->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			ProjectileComponent& pj = proj->AddComponent<ProjectileComponent>();
			pj.velocity.y = -300.0f;
			pj.friendly = false;

			gueepo::GameObject* proj2 = gameWorld->CreateGameObject(projectileTexture, "projectile");
			proj2->transform->position = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
			proj2->transform->position = proj2->transform->position + gueepo::math::vec2(-8.0f, -24.0f);
			proj2->sprite->RebuildSourceRectangle(projectileMinVec, projectileMaxVec);
			proj2->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			ProjectileComponent& pj2 = proj2->AddComponent<ProjectileComponent>();
			pj2.velocity.y = -300.0f;
			pj2.friendly = false;

			shotCooldown = timeToShoot;
		}

		// Moving
		gueepo::GameObject* gameobjOwner = static_cast<gueepo::GameObject*>(Owner);
		gueepo::TransformComponent* transform = gameobjOwner->GetComponentOfType<gueepo::TransformComponent>();

		if (transform->position.x > 250.0f) {
			velocity.x = -speed;
		}
		else if (transform->position.x < -250.0f) {
			velocity.x = speed;
		}

		gameobjOwner->Translate(velocity * DeltaTime);
	}
};