
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
				Interpolator->Mode = ion::Animation::CCatmullRomAdvancedSplineInterpolator<vec3f>::Uniform;
				break;

			case EKey::Y:
				Interpolator->Mode = ion::Animation::CCatmullRomAdvancedSplineInterpolator<vec3f>::Chordal;
				break;

			case EKey::U:
				Interpolator->Mode = ion::Animation::CCatmullRomAdvancedSplineInterpolator<vec3f>::Centripetal;
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

	Interpolator = std::make_shared<ion::Animation::CCatmullRomAdvancedSplineInterpolator<vec3f>>();
	Spline.SetDefaultInterpolator(Interpolator);
	Spline.SetLooping(false);

	Spline.AddNode(vec3f(0, 1, 0));
	Spline.AddNode(vec3f(3, 1, 0));
	Spline.AddNode(vec3f(3, 1, 3));
	Spline.AddNode(vec3f(6, 1, 9));
}

void CApplication::MainLoop()
{
	TimeManager->Start();
	while (WindowManager->Run())
	{
		TimeManager->Update();

		Timer += TimeDirection * (float) TimeManager->GetElapsedTime();
		
		float const EndPath = (float) (Spline.GetNodes().size() - 1);
		if (Timer > EndPath)
		{
			Timer = EndPath;
			TimeDirection = -1.f;
		}
		else if (Timer < 0.f)
		{
			Timer = 0.f;
			TimeDirection = 1.f;
		}

		// GUI
		GUIManager->NewFrame();

		LineObject->ResetLines();
		if (NodeObjects.size() < Spline.GetNodes().size())
		{
			NodeObjects.resize(Spline.GetNodes().size(), nullptr);
		}
		for (int i = 0; i < Spline.GetNodes().size(); ++ i)
		{
			auto & Node = Spline.GetNode(i);

			if (! NodeObjects[i])
			{
				NodeObjects[i] = new CSimpleMeshSceneObject();
				NodeObjects[i]->SetMesh(CubeMesh);
				NodeObjects[i]->SetShader(ColorShader);
				NodeObjects[i]->SetScale(0.15f);
				NodeObjects[i]->SetUniform("uColor", CUniform<color3f>(Color::Basic::Magenta));
				RenderPass->AddSceneObject(NodeObjects[i]);
			}

			NodeObjects[i]->SetPosition(Node);

			if (i + 1 < Spline.GetNodes().size())
			{
				LineObject->AddLine(Node, Spline.GetNode(i + 1), Color::Hex(0xCC11FF));
			}
		}
		float const LengthStep = 0.05f;
		for (float f = LengthStep; f < Spline.GetTotalPathLength(); f += LengthStep)
		{
			LineObject->AddLine(
				Spline.GetNodeFromDistance(f - LengthStep),
				Spline.GetNodeFromDistance(f),
				Color::Hex(0xFF0000));
		}
		SphereObject->SetPosition(Spline.GetNodeInterpolated(Timer));


		// Draw
		RenderTarget->ClearColorAndDepth();
		SceneManager->DrawAll();
		ImGui::Render();

		Window->SwapBuffers();
	}
}
