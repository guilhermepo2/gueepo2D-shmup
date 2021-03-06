// ================================================
// The shoot'em shmup task list
// - [ ] make projectiles glow because that would be cool
// - [ ] add a trail renderer on projectiles? (I have no idea how to do that but that would be cool)
// - [ ] particles would be cool
// ================================================
// Things I dislike about this project
// - I'm not sure if I like the idea of the GameWorld being a singleton
// - A very recurrent problem that I ran up to is that I need a reference for the gameworld and resource manager or even the game layer in a component,
// in case I ever want to create another component inside a component (like shooting! and exploding!).
// Is there a better way to do that than injecting References* into the component or by using singletons?
// - I could just use a bunch of std::function but that also sounds a bit weird.
// ================================================

#include <gueepo2d.h>
#include "ProjectileComponent.h"
#include "ScrollerComponent.h"
#include "ShipComponent.h"
#include "EnemyComponent.h"
#include <imgui.h>

static const int window_width = 640;
static const int window_height = 640;

static float fps;
static bool bShowDebug = false;
static float spawnAnEnemyEvery = 1.0f;
static float spawnTimer = 0.0f;

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

public:
	void Factory_CreateEnemy();
	void Factory_CreateExplosion();
};

void GameLayer::OnAttach() {

	gueepo::Renderer::Initialize();
	m_Camera = std::make_unique<gueepo::OrtographicCamera>(window_width, window_height);
	m_Camera->SetBackgroundColor((223.0f / 255.0f), (246.0f / 255.0f), (245.0f / 255.0f), 1.0f);

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

	gueepo::json backgroundMap("./assets/maps/tilemap.json");
	gueepo::Entity* myTilemap = m_gameWorld->CreateEntity("tilemap");
	myTilemap->AddComponent<gueepo::TransformComponent>(gueepo::math::vec2(-300.0f, 200.0f), 0.0f, gueepo::math::vec2(2.0f, 2.0f));
	gueepo::TilemapComponent& tilemapComponent = myTilemap->AddComponent<gueepo::TilemapComponent>();
	tilemapComponent.LoadFromTiled(m_resourceManager->GetTilemap("tiles-tilemap"), backgroundMap);
	myTilemap->AddComponent<ScrollerComponent>();

	// -----------------------------------------------------------------------------------------------------------------
	gueepo::GameObject* test = m_gameWorld->CreateGameObject(m_resourceManager->GetTexture("ship"), "player");
	test->sprite->RebuildFromTile(m_resourceManager->GetTilemap("ship-tilemap")->GetTile(4));
	test->SetScale(2.0f, 2.0f);
	test->SetPosition(gueepo::math::vec2(0.0f, -280.0f));
	test->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-8.0f, -8.0f), gueepo::math::vec2(8.0f, 8.0f));

	ShipComponent& comp = test->AddComponent<ShipComponent>();
	comp.gameWorld = m_gameWorld;
	comp.projectileTexture = m_resourceManager->GetTexture("tiles");
	comp.projectileMinVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(0).GetRect().bottomLeft;
	comp.projectileMaxVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(0).GetRect().topRight;

	Factory_CreateEnemy();
}

void GameLayer::OnDetach() {
	m_gameWorld->Destroy();
	m_resourceManager->ClearResources();
}

void GameLayer::OnUpdate(float DeltaTime) {
	fps = 1 / DeltaTime;

	spawnTimer += DeltaTime;

	if (spawnTimer > spawnAnEnemyEvery) {
		Factory_CreateEnemy();
		spawnTimer = 0.0f;
	}

	m_gameWorld->Update(DeltaTime);
	m_collisionWorld->Update();
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
	if (bShowDebug) {
		ImGui::Begin("debug");
		ImGui::Text("entities: %d", m_gameWorld->GetNumberOfEntities());
		ImGui::Text("FPS: %.2f", fps);
		ImGui::Text("Draw Calls: %d", gueepo::Renderer::GetDrawCalls());
		ImGui::End();
	}
}

class shootemshmup : public gueepo::Application {
public:
	shootemshmup() : Application("shoot'em shmup ", window_width, window_height) {
		PushLayer(new GameLayer());
	}
	~shootemshmup() {}
};

gueepo::Application* gueepo::CreateApplication() {
	return new shootemshmup();
}


// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------
void GameLayer::Factory_CreateEnemy() {
	gueepo::GameObject* enemyTest = m_gameWorld->CreateGameObject(m_resourceManager->GetTexture("ship"), "enemy");
	enemyTest->sprite->RebuildFromTile(m_resourceManager->GetTilemap("ship-tilemap")->GetTile(5));
	enemyTest->SetScale(2.0f, 2.0f);
	enemyTest->SetLifetime(50.0f);

	float xpos = gueepo::rand::Float() * 150;
	enemyTest->SetPosition(gueepo::math::vec2(xpos, 300.0f));
	enemyTest->GetComponentOfType<gueepo::TransformComponent>()->rotation = 180;
	enemyTest->AddComponent<gueepo::BoxCollider>(gueepo::math::vec2(-10.0f, -10.0f), gueepo::math::vec2(10.0f, 10.0f));

	EnemyComponent& en = enemyTest->AddComponent<EnemyComponent>();
	en.gameWorld = m_gameWorld;
	en.projectileTexture = m_resourceManager->GetTexture("tiles");
	en.projectileMinVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(2).GetRect().bottomLeft;
	en.projectileMaxVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(2).GetRect().topRight;

	en.explosionTexture = m_resourceManager->GetTexture("tiles");
	en.explosionMinVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(4).GetRect().bottomLeft;
	en.explosionMaxVec = m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(4).GetRect().topRight;
}

void GameLayer::Factory_CreateExplosion() {
	gueepo::Texture* tilesheet = m_resourceManager->GetTexture("tiles");
	gueepo::Tilemap* tilemap = m_resourceManager->GetTilemap("tiles-tilemap");

	gueepo::GameObject* explo = m_gameWorld->CreateGameObject(tilesheet, "explosion");
	explo->sprite->RebuildFromTile(tilemap->GetTile(4));
	explo->SetLifetime(0.1f);
}
