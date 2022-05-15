// ================================================
// The shoot'em shmup task list
// - [ ] make projectiles glow because that would be cool
// - [ ] add a trail renderer on projectiles? (I have no idea how to do that but that would be cool)
// - [ ] particles would be cool
// ================================================

#include <gueepo2d.h>
#include <imgui.h>

static float fps;
static bool bShowDebug = false;

// #todo: probably have to create a projectile manager or something that destroys projectiles after some time and when they are out of the camera
class ProjectileComponent : public gueepo::Component {
public:
	gueepo::math::vec2 velocity;
	float lifetime = 5.0f;

	void Initialize() {
		velocity.y = 300.0f;
		// multiplying the velocity by a random factor
		// I want to add/subtract 20% of the velocity according to random stuff
		bool bShouldAdd = gueepo::rand::Int() % 2 == 0;
		float randomVariation = gueepo::rand::Float() * (velocity.y * 0.1f);

		if (!bShouldAdd) {
			randomVariation *= -1;
		}

		velocity.y += randomVariation;
		Owner->SetLifetime(lifetime);
	}

	void Update(float DeltaTime) {
		gueepo::GameObject* gameobjOwner = static_cast<gueepo::GameObject*>(Owner);
		gameobjOwner->Translate(velocity * DeltaTime);
	}
};

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
			proj->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			proj->AddComponent<ProjectileComponent>();

			gueepo::GameObject* proj2 = gameWorld->CreateGameObject(projectileTexture, "projectile");
			proj2->transform->position = Owner->GetComponentOfType<gueepo::TransformComponent>()->position;
			proj2->transform->position.x -= projectilePositionOffset.x;
			proj2->transform->position.y += projectilePositionOffset.y;
			proj2->sprite->RebuildSourceRectangle(projectileMinVec, projectileMaxVec);
			proj2->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-4.0f, -8.0f), gueepo::math::vec2(4.0f, 8.0f));
			proj2->AddComponent<ProjectileComponent>();


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

// ================================================================================================
// ================================================================================================
// ================================================================================================
// ================================================================================================
class GameLayer : public gueepo::Layer {
public:
	GameLayer() : Layer("Game Layer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float DeltaTime) override;
	void OnInput(const gueepo::InputState& currentInputState);
	void OnEvent(gueepo::Event& e) override {}
	void OnRender() override;
	void OnImGuiRender() override;

private:
	std::unique_ptr<gueepo::OrtographicCamera> m_Camera;
	std::shared_ptr<gueepo::GameWorld> m_gameWorld;
	std::unique_ptr<gueepo::ResourceManager> m_resourceManager;
	std::unique_ptr<gueepo::CollisionWorld> m_collisionWorld;
};

void GameLayer::OnAttach() {
	gueepo::Renderer::Initialize();
	m_Camera = std::make_unique<gueepo::OrtographicCamera>(640, 360);
	m_Camera->SetBackgroundColor(0.55f, 0.792f, 0.902f, 1.0f);

	m_gameWorld = std::make_shared<gueepo::GameWorld>();
	m_resourceManager = std::make_unique<gueepo::ResourceManager>();
	m_collisionWorld = std::make_unique<gueepo::CollisionWorld>();
	m_collisionWorld->Initialize();

	m_resourceManager->AddTexture("ship", "./assets/ships_packed.png");
	m_resourceManager->AddTilemap("ship-tilemap", "ship");
	m_resourceManager->GetTilemap("ship-tilemap")->Slice(32, 32);

	m_resourceManager->AddTexture("tiles", "./assets/tiles_packed.png");
	m_resourceManager->AddTilemap("tiles-tilemap", "tiles");
	m_resourceManager->GetTilemap("tiles-tilemap")->Slice(16, 16);

	gueepo::GameObject* test = m_gameWorld->CreateGameObject(m_resourceManager->GetTexture("ship"), "shipTest");
	test->sprite->RebuildFromTile(m_resourceManager->GetTilemap("ship-tilemap")->GetTile(4));
	test->SetScale(1.5f, 1.5f);
	test->SetPosition(gueepo::math::vec2(0.0f, -150.0f));
	test->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-8.0f, -8.0f), gueepo::math::vec2(8.0f, 8.0f));

	ShipComponent& comp = test->AddComponent<ShipComponent>();
	comp.gameWorld = m_gameWorld;
	comp.projectileTexture = m_resourceManager->GetTexture("tiles");
	comp.projectileMinVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(9).GetRect().bottomLeft;
	comp.projectileMaxVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(9).GetRect().topRight;

	gueepo::GameObject* enemyTest = m_gameWorld->CreateGameObject(m_resourceManager->GetTexture("ship"), "enemy test");
	enemyTest->sprite->RebuildFromTile(m_resourceManager->GetTilemap("ship-tilemap")->GetTile(10));
	enemyTest->SetScale(2.0f, 2.0f);
	enemyTest->SetPosition(gueepo::math::vec2(0.0f, 100.0f));
	enemyTest->GetComponentOfType<gueepo::TransformComponent>()->rotation = 180;
	enemyTest->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-10.0f, -10.0f), gueepo::math::vec2(10.0f, 10.0f));
}

void GameLayer::OnDetach() {
	m_gameWorld->Destroy();
	m_resourceManager->ClearResources();
}

void GameLayer::OnUpdate(float DeltaTime) {
	fps = 1 / DeltaTime;
	m_gameWorld->Update(DeltaTime);
}

void GameLayer::OnInput(const gueepo::InputState& currentInputState) {
	m_gameWorld->ProcessInput(currentInputState);

	if (currentInputState.Keyboard.WasKeyPressedThisFrame(gueepo::Keycode::KEYCODE_1)) {
		bShowDebug = !bShowDebug;
	}
}

void GameLayer::OnRender() {
	gueepo::Renderer::BeginScene(*m_Camera);
	m_gameWorld->Render();

	if (bShowDebug) {
		m_collisionWorld->Debug_Render();
	}
	 
	gueepo::Renderer::EndScene();
}

void GameLayer::OnImGuiRender()
{
	ImGui::Begin("debug");
	ImGui::Text("entities: %d", m_gameWorld->GetNumberOfEntities());
	ImGui::Text("FPS: %.2f", fps);
	ImGui::Text("Draw Calls: %d", gueepo::Renderer::GetDrawCalls());
	ImGui::End();
}

class shootemshmup : public gueepo::Application {
public:
	shootemshmup() : Application("shoot'em shmup ", 640, 360) {
		PushLayer(new GameLayer());
	}
	~shootemshmup() {}
};

gueepo::Application* gueepo::CreateApplication() {
	return new shootemshmup();
}
