
#include "CApplication.h"


using namespace ion;
using namespace ion::Scene;
using namespace ion::Graphics;



void CApplication::Run()
{
	InitializeEngine();
	LoadAssets();
	SetupScene();
	AddSceneObjects();

	MainLoop();
}

void CApplication::OnEvent(IEvent & Event)
{
	if (InstanceOf<SKeyboardEvent>(Event))
	{
		SKeyboardEvent KeyboardEvent = As<SKeyboardEvent>(Event);

		if (! KeyboardEvent.Pressed)
		{
			switch (KeyboardEvent.Key)
			{
			case EKey::F:
				RenderPass->SetActiveCamera(FreeCamera);
				break;

			case EKey::T:
				GroundTexture->SetMagFilter(ITexture::EFilter::Nearest);
				break;

			case EKey::Y:
				GroundTexture->SetMagFilter(ITexture::EFilter::Linear);
				break;

			case EKey::RightBracket:
				GroundObject->SetVisible(! GroundObject->IsVisible());
				break;
			}
		}
	}
	else if (InstanceOf<SMouseEvent>(Event))
	{
		SMouseEvent MouseEvent = As<SMouseEvent>(Event);

		switch (MouseEvent.Type)
		{
		case SMouseEvent::EType::Click:
			if (MouseEvent.Button == SMouseEvent::EButton::Left)
			{
				if (! MouseEvent.Pressed)
				{
					ray3f const Ray = FreeCamera->GetPickingRay(MouseEvent.Location, Window->GetSize());
				}
			}

			break;
		}
	}
}

void CApplication::InitializeEngine()
{
	LoadSettings();

	GraphicsAPI->Init(new Graphics::COpenGLImplementation());
	WindowManager->Init(GraphicsAPI);
	TimeManager->Init(WindowManager);

	Window = CreateWindowFromSettings("Texture Mapping");
	GraphicsContext = GraphicsAPI->GetWindowContext(Window);

	SceneManager->Init(GraphicsAPI);

	AssetManager->Init(GraphicsAPI);
	AssetManager->AddAssetPath("Assets/");
	AssetManager->SetShaderPath("Shaders/");
	AssetManager->SetTexturePath("Textures/");

	RenderTarget = GraphicsContext->GetBackBuffer();
	RenderTarget->SetClearColor(color3f(0.9f));

	GUIManager->Init(Window);
	GUIManager->AddFontFromFile("Assets/GUI/OpenSans.ttf", 18.f);
	Window->AddListener(GUIManager);
	GUIManager->AddListener(this);
}

void CApplication::LoadAssets()
{
	CubeMesh = CGeometryCreator::CreateCube();
	SphereMesh = CGeometryCreator::CreateSphere(0.5f, 12, 8);

	ColorShader = AssetManager->LoadShader("Color");
	TextureShader = AssetManager->LoadShader("Texture");
	LineShader = AssetManager->LoadShader("Line");

	GroundTexture = AssetManager->LoadTexture("Ground.png");
	if (GroundTexture)
	{
		GroundTexture->SetMagFilter(ITexture::EFilter::Nearest);
		GroundTexture->SetWrapMode(ITexture::EWrapMode::Clamp);
	}
}

void CApplication::SetupScene()
{
	RenderPass = new CRenderPass(GraphicsContext);
	RenderPass->SetRenderTarget(RenderTarget);
	SceneManager->AddRenderPass(RenderPass);

	FreeCamera = new CPerspectiveCamera(Window->GetAspectRatio());
	FreeCamera->SetPosition(vec3f(0, 3, 3));
	FreeCamera->SetFocalLength(0.4f);
	FreeCamera->SetFarPlane(10000.f);

	CCameraController * Controller = new CGamePadCameraController(FreeCamera);
	Controller->SetTheta(-Constants32::Pi / 2);
	Controller->SetPhi(0);
	Window->AddListener(Controller);
	TimeManager->MakeUpdateTick(0.02)->AddListener(Controller);

	RenderPass->SetActiveCamera(FreeCamera);
}

void CApplication::AddSceneObjects()
{
	GroundObject = new CSimpleMeshSceneObject();
	GroundObject->SetMesh(CubeMesh);
	GroundObject->SetShader(TextureShader);
	GroundObject->SetScale(vec3f(16, 1, 16));
	GroundObject->SetPosition(vec3f(0, 0, 0));
	GroundObject->SetTexture("uTexture", GroundTexture);
	RenderPass->AddSceneObject(GroundObject);

	SphereObject = new CSimpleMeshSceneObject();
	SphereObject->SetMesh(SphereMesh);
	SphereObject->SetShader(ColorShader);
	SphereObject->SetPosition(vec3f(0, 3, 0));
	SphereObject->SetScale(0.35f);
	SphereObject->SetUniform("uColor", CUniform<color3f>(Color::Basic::Red));
	RenderPass->AddSceneObject(SphereObject);

	LineObject = new CLineSceneObject();
	LineObject->SetShader(LineShader);
	RenderPass->AddSceneObject(LineObject);
	
	CDirectionalLight * Light = new CDirectionalLight();
	Light->SetDirection(vec3f(1, -2, -2));
	RenderPass->AddLight(Light);

	PointLight = new CPointLight();
	PointLight->SetPosition(vec3f(3, 6, 3));
	RenderPass->AddLight(PointLight);

	CSimpleMeshSceneObject * NodeObject = new CSimpleMeshSceneObject();
	NodeObject->SetMesh(CubeMesh);
	NodeObject->SetShader(ColorShader);
	NodeObject->SetPosition(vec3f(0, 1, 0));
	NodeObject->SetScale(0.15f);
	NodeObject->SetUniform("uColor", CUniform<color3f>(Color::Basic::Magenta));
	RenderPass->AddSceneObject(NodeObject);

	NodeObject = new CSimpleMeshSceneObject();
	NodeObject->SetMesh(CubeMesh);
	NodeObject->SetShader(ColorShader);
	NodeObject->SetPosition(vec3f(3, 1, 0));
	NodeObject->SetScale(0.15f);
	NodeObject->SetUniform("uColor", CUniform<color3f>(Color::Basic::Magenta));
	RenderPass->AddSceneObject(NodeObject);

	NodeObject = new CSimpleMeshSceneObject();
	NodeObject->SetMesh(CubeMesh);
	NodeObject->SetShader(ColorShader);
	NodeObject->SetPosition(vec3f(3, 1, 3));
	NodeObject->SetScale(0.15f);
	NodeObject->SetUniform("uColor", CUniform<color3f>(Color::Basic::Magenta));
	RenderPass->AddSceneObject(NodeObject);

	NodeObject = new CSimpleMeshSceneObject();
	NodeObject->SetMesh(CubeMesh);
	NodeObject->SetShader(ColorShader);
	NodeObject->SetPosition(vec3f(6, 1, 9));
	NodeObject->SetScale(0.15f);
	NodeObject->SetUniform("uColor", CUniform<color3f>(Color::Basic::Magenta));
	RenderPass->AddSceneObject(NodeObject);

	LineObject->AddLine(vec3f(0, 1, 0), vec3f(3, 1, 0), Color::Hex(0xCC11FF));
	LineObject->AddLine(vec3f(3, 1, 0), vec3f(3, 1, 3), Color::Hex(0xCC11FF));
	LineObject->AddLine(vec3f(3, 1, 3), vec3f(6, 1, 9), Color::Hex(0xCC11FF));
}

void CApplication::MainLoop()
{
	TimeManager->Start();
	while (WindowManager->Run())
	{
		TimeManager->Update();
		
		// GUI
		GUIManager->NewFrame();

		// Draw
		RenderTarget->ClearColorAndDepth();
		SceneManager->DrawAll();
		ImGui::Render();

		Window->SwapBuffers();
	}
}
