#pragma once
#include <gueepo2d.h>
#include "ProjectileComponent.h"

class ShipComponent : public gueepo::Component {
public:
	gueepo::math::vec2 velocity;
	float shipSpeed;
	float accelerationRate;
	std::shared_ptr<gueepo::GameWorld> gameWorld;
	gueepo::Texture* projectileTexture;
	gueepo::math::vec2 projectileMinVec;
	gueepo::math::vec2 projectileMaxVec;
	gueepo::math::vec2 projectilePositionOffset;

	float shootingCooldown = 0.0f;
	float cooldownCount = 0.0f;

	void Initialize() {
		shipSpeed = 200.0f;
		accelerationRate = 0.05f;

		projectilePositionOffset.x = 8.0f;
		projectilePositionOffset.y = 24.0f;

		shootingCooldown = 0.1f;
	}

	void BeginPlay() {}

	bool ProcessInput(const gueepo::InputState& CurrentInputState) {

		// Handling Space (Shooting) first, and NOT returning
		// Because we want the ability of being able to shoot and move at the same time :)
		if (CurrentInputState.Keyboard.IsKeyDown(gueepo::KEYCODE_SPACE) && cooldownCount <= 0.0f) {
			// #todo: how to shoot?
			// have to instantiate a new gameobject;
			// how do I instantiate a new gameobject from an entity?
			// The current solution is having the gameworld as a public attribute here, I don't like it.
			// Should the game world be a global? like GameObject.Instantiate(position, rotation, scale)?


			gueepo::GameObject* proj = gameWorld->CreateGameObject(projectileTexture, "projectile");
			proj->transform->position = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
			proj->transform->position = proj->transform->position + projectilePositionOffset;
			proj->sprite->RebuildSourceRectangle(projectileMinVec, projectileMaxVec);
			gueepo::BoxCollider& box = proj->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			ProjectileComponent& pj = proj->AddComponent<ProjectileComponent>();
			pj.friendly = true;

			gueepo::GameObject* proj2 = gameWorld->CreateGameObject(projectileTexture, "projectile");
			proj2->transform->position = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
			proj2->transform->position.x -= projectilePositionOffset.x;
			proj2->transform->position.y += projectilePositionOffset.y;
			proj2->sprite->RebuildSourceRectangle(projectileMinVec, projectileMaxVec);
			proj2->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			ProjectileComponent& pj2 = proj2->AddComponent<ProjectileComponent>();
			pj2.friendly = true;


			cooldownCount = shootingCooldown;
		}

		if (CurrentInputState.Keyboard.IsKeyDown(gueepo::KEYCODE_D)) {
			velocity.x += shipSpeed * accelerationRate;
			if (velocity.x > shipSpeed) velocity.x = shipSpeed;
			return true;
		}
		else if (CurrentInputState.Keyboard.IsKeyDown(gueepo::KEYCODE_A)) {
			velocity.x -= shipSpeed * accelerationRate;
			if (velocity.x < -shipSpeed) velocity.x = -shipSpeed;
			return true;
		}


		return false;
	}

	void Update(float DeltaTime) {
		gueepo::GameObject* gameobjOwner = static_cast<gueepo::GameObject*>(Owner);
		gameobjOwner->Translate(velocity * DeltaTime);
		velocity = velocity * 0.97f;

		cooldownCount -= DeltaTime;
	}

	void Destroy() {}
};