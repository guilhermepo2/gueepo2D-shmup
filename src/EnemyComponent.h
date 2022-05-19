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

	// death explosion stuff
	gueepo::Texture* explosionTexture;
	gueepo::math::vec2 explosionMinVec;
	gueepo::math::vec2 explosionMaxVec;

	void Initialize() {
		speed = 300.0f;
		velocity.x = speed;

		if (gueepo::rand::Int() % 2 == 0) {
			velocity.x *= -1;
		}

		velocity.y = -100.0f;
		timeToShoot = 1.5f;
		shotCooldown = timeToShoot;

		gueepo::BoxCollider* box = Owner->GetComponentOfType<gueepo::BoxCollider>();
		if (box != nullptr) {
			box->OnCollisionEnter = COLLISION_CALLBACK(&EnemyComponent::EnemyOnCollisionEnter);
		}
	}

	void Update(float DeltaTime) override {
		// Shooting
		shotCooldown -= DeltaTime;

		if (shotCooldown < 0.0f) {

			gueepo::GameObject* proj = gameWorld->CreateGameObject(projectileTexture, "projectile");
			proj->transform->position = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
			proj->transform->position = proj->transform->position + gueepo::math::vec2(8.0f, -24.0f);
			proj->sprite->RebuildSourceRectangle(projectileMinVec, projectileMaxVec);
			proj->sprite->spriteTint.rgba[1] = 0.0f;
			proj->sprite->spriteTint.rgba[2] = 0.0f;
			proj->sprite->spriteTint.rgba[3] = 0.5f;
			proj->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			ProjectileComponent& pj = proj->AddComponent<ProjectileComponent>();
			pj.velocity.y = -600.0f;
			pj.friendly = false;

			gueepo::GameObject* proj2 = gameWorld->CreateGameObject(projectileTexture, "projectile");
			proj2->transform->position = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
			proj2->transform->position = proj2->transform->position + gueepo::math::vec2(-8.0f, -24.0f);
			proj2->sprite->RebuildSourceRectangle(projectileMinVec, projectileMaxVec);
			proj2->sprite->spriteTint.rgba[1] = 0.0f;
			proj2->sprite->spriteTint.rgba[2] = 0.0f;
			proj2->sprite->spriteTint.rgba[3] = 0.5f;
			proj2->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			ProjectileComponent& pj2 = proj2->AddComponent<ProjectileComponent>();
			pj2.velocity.y = -600.0f;
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

	void EnemyOnCollisionEnter(gueepo::BoxCollider* other) {
		// checking if we were hit by a friendly projectile
		if (other->GetTag() == "player-projectile") {
			if (explosionTexture != nullptr) {
				gueepo::GameObject* gameobjOwner = static_cast<gueepo::GameObject*>(Owner);
				gueepo::TransformComponent* transform = gameobjOwner->GetComponentOfType<gueepo::TransformComponent>();

				gueepo::GameObject* explo = gameWorld->CreateGameObject(explosionTexture, "explosion");
				explo->transform->position = transform->position;
				explo->transform->position.x = gueepo::math::round(explo->transform->position.x);
				explo->transform->position.y = gueepo::math::round(explo->transform->position.y);
				explo->transform->scale = gueepo::math::vec2(3.0f);
				explo->sprite->RebuildSourceRectangle(explosionMinVec, explosionMaxVec);
				explo->SetLifetime(0.2f);
			}
		}
	}
};