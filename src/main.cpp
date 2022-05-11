#include <gueepo2d.h>

class ShipComponent : public gueepo::Component {
public:
	gueepo::math::vec2 velocity;
	float shipSpeed;
	float accelerationRate;

	void Initialize() {
		shipSpeed = 200.0f;
		accelerationRate = 0.05f;
	}

	void BeginPlay() {}
	
	bool ProcessInput(const gueepo::InputState& CurrentInputState) { 
		
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
	}

	void Destroy() {
	
	}
};

class GameLayer : public gueepo::Layer {
public:
	GameLayer() : Layer("Game Layer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float DeltaTime) override;
	void OnInput(const gueepo::InputState& currentInputState);
	void OnEvent(gueepo::Event& e) override {}
	void OnRender() override;
	void OnImGuiRender() override {}

private:
	std::unique_ptr<gueepo::OrtographicCamera> m_Camera;
	std::unique_ptr<gueepo::GameWorld> m_gameWorld;
	std::unique_ptr<gueepo::ResourceManager> m_resourceManager;
};

void GameLayer::OnAttach() {
	gueepo::Renderer::Initialize();
	m_Camera = std::make_unique<gueepo::OrtographicCamera>(640, 360);
	m_Camera->SetBackgroundColor(0.55f, 0.792f, 0.902f, 1.0f);

	m_gameWorld = std::make_unique<gueepo::GameWorld>();
	m_resourceManager = std::make_unique<gueepo::ResourceManager>();

	m_resourceManager->AddTexture("ship", "./assets/ships_packed.png");
	m_resourceManager->AddTexture("tiles", "./assets/tiles_packed.png");
	m_resourceManager->AddTilemap("tiles-tilemap", "tiles");
	m_resourceManager->GetTilemap("tiles-tilemap")->Slice(32, 32);

	gueepo::GameObject* test = m_gameWorld->CreateGameObject(m_resourceManager->GetTexture("ship"), "shipTest");
	test->sprite->RebuildFromTile(m_resourceManager->GetTilemap("tiles-tilemap")->GetTile(4));
	test->SetScale(1.5f, 1.5f);
	test->SetPosition(gueepo::math::vec2(0.0f, -150.0f));
	test->AddComponent<ShipComponent>();
}

void GameLayer::OnDetach() {
	m_gameWorld->Destroy();
	m_resourceManager->ClearResources();
}

void GameLayer::OnUpdate(float DeltaTime) {
	m_gameWorld->Update(DeltaTime);
}

void GameLayer::OnInput(const gueepo::InputState& currentInputState) {
	m_gameWorld->ProcessInput(currentInputState);
}

void GameLayer::OnRender() {
	gueepo::Renderer::BeginScene(*m_Camera);
	m_gameWorld->Render();
	gueepo::Renderer::EndScene();
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
