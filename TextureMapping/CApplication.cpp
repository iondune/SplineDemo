
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

			case EKey::Z:

				SubdivisionSurface.MeshObject->SetFeatureEnabled(Graphics::EDrawFeature::Wireframe, true);
				break;

			case EKey::X:

				SubdivisionSurface.MeshObject->SetFeatureEnabled(Graphics::EDrawFeature::Wireframe, false);
				break;

			case EKey::N:

				SubdivisionSurface.NormalMode = 0;
				SubdivisionSurface.ResetMesh();
				break;

			case EKey::M:

				SubdivisionSurface.NormalMode = 1;
				SubdivisionSurface.ResetMesh();
				break;

			case EKey::J:

				SubdivisionSurface.SubDivLevel = Min(++SubdivisionSurface.SubDivLevel, SubdivisionSurface.MaxSubDivide - 1);
				SubdivisionSurface.ResetMesh();
				break;

			case EKey::K:

				SubdivisionSurface.SubDivLevel = Max(--SubdivisionSurface.SubDivLevel, 0);
				SubdivisionSurface.ResetMesh();
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

	GroundShader = AssetManager->LoadShader("Ground");
	DiffuseShader = AssetManager->LoadShader("Diffuse");
	ColorShader = AssetManager->LoadShader("Color");
	SpecularShader = AssetManager->LoadShader("Specular");

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
	FreeCamera->SetPosition(vec3f(0, 1.25f, 3));
	FreeCamera->SetFocalLength(0.4f);
	FreeCamera->SetFarPlane(10000.f);

	CCameraController * Controller = new CGamePadCameraController(FreeCamera);
	Controller->SetTheta(-Constants32::Pi / 2);
	Controller->SetPhi(0);
	Window->AddListener(Controller);
	TimeManager->MakeUpdateTick(0.02)->AddListener(Controller);

	RenderPass->SetActiveCamera(FreeCamera);

	SubdivisionSurface.MakeCube();
}

void CApplication::AddSceneObjects()
{
	GroundObject = new CSimpleMeshSceneObject();
	GroundObject->SetMesh(CubeMesh);
	GroundObject->SetShader(GroundShader);
	GroundObject->SetScale(vec3f(16, 1, 16));
	GroundObject->SetPosition(vec3f(0, 0, 0));
	GroundObject->SetTexture("uTexture", GroundTexture);
	RenderPass->AddSceneObject(GroundObject);

	CSimpleMeshSceneObject * SphereObject = new CSimpleMeshSceneObject();
	SphereObject->SetMesh(SphereMesh);
	SphereObject->SetShader(SpecularShader);
	SphereObject->SetPosition(vec3f(-3, 3, 0));
	SphereObject->SetTexture("uTexture", GroundTexture);
	RenderPass->AddSceneObject(SphereObject);
	
	CDirectionalLight * Light = new CDirectionalLight();
	Light->SetDirection(vec3f(1, -2, -2));
	RenderPass->AddLight(Light);

	PointLight = new CPointLight();
	RenderPass->AddLight(PointLight);
}

void CApplication::MainLoop()
{
	TimeManager->Start();
	while (WindowManager->Run())
	{
		TimeManager->Update();
		
		// GUI
		GUIManager->NewFrame();

		PointLight->SetPosition(FreeCamera->GetPosition());

		// Draw
		RenderTarget->ClearColorAndDepth();
		SceneManager->DrawAll();
		ImGui::Render();

		Window->SwapBuffers();
	}
}
