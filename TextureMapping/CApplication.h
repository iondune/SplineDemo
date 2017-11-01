
#pragma once

#include <ionEngine.h>
#include "CSubdivisionSurface.h"


class CApplication : public ion::Singleton<CApplication>, public ion::CDefaultApplication
{

public:

	void Run();
	void OnEvent(IEvent & Event);

	ion::SingletonPointer<ion::CWindowManager> WindowManager;
	ion::SingletonPointer<ion::CTimeManager> TimeManager;
	ion::SingletonPointer<ion::CSceneManager> SceneManager;
	ion::SingletonPointer<ion::CAssetManager> AssetManager;
	ion::SingletonPointer<ion::CGUIManager> GUIManager;
	ion::SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;

	ion::CWindow * Window = nullptr;
	ion::SharedPointer<ion::Graphics::IGraphicsContext> GraphicsContext;
	ion::Scene::CRenderPass * RenderPass = nullptr;

	ion::SharedPointer<ion::Graphics::IShader> GroundShader;
	ion::SharedPointer<ion::Graphics::IShader> DiffuseShader;
	ion::SharedPointer<ion::Graphics::IShader> ColorShader;
	ion::SharedPointer<ion::Graphics::IShader> SpecularShader;

	ion::Scene::CSimpleMesh * CubeMesh = nullptr;
	ion::Scene::CSimpleMesh * SphereMesh = nullptr;
	
	ion::SharedPointer<ion::Graphics::ITexture> GroundTexture;
	ion::SharedPointer<ion::Graphics::ITexture> TestTexture;

	ion::Scene::CSimpleMeshSceneObject * GroundObject = nullptr;
	ion::Scene::CSimpleMeshSceneObject * SphereObject = nullptr;

	CSubdivisionSurface SubdivisionSurface;
	ion::Graphics::CUniform<int> uTexCoordMode = 0;
	ion::Graphics::CUniform<int> uShowTexCoords = 0;

protected:

	void InitializeEngine();
	void LoadAssets();
	void SetupScene();
	void AddSceneObjects();
	void MainLoop();

	ion::SharedPointer<ion::Graphics::IRenderTarget> RenderTarget = nullptr;
	ion::Scene::CPerspectiveCamera * FreeCamera = nullptr;
	ion::Scene::CPointLight * PointLight = nullptr;

private:

	friend class ion::Singleton<CApplication>;
	CApplication()
	{}

};
