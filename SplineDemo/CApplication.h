
#pragma once

#include <ionEngine.h>
#include "CLineSceneObject.h"


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

	ion::SharedPointer<ion::Graphics::IShader> ColorShader;
	ion::SharedPointer<ion::Graphics::IShader> TextureShader;
	ion::SharedPointer<ion::Graphics::IShader> LineShader;

	ion::Scene::CSimpleMesh * CubeMesh = nullptr;
	ion::Scene::CSimpleMesh * SphereMesh = nullptr;
	ion::CLineSceneObject * LineObject = nullptr;
	
	ion::SharedPointer<ion::Graphics::ITexture> GroundTexture;

	ion::Scene::CSimpleMeshSceneObject * GroundObject = nullptr;
	ion::Scene::CSimpleMeshSceneObject * SphereObject = nullptr;

	ion::Animation::CSpline<ion::vec3f> Spline;
	ion::SharedPointer<ion::Animation::CCatmullRomAdvancedSplineInterpolator<ion::vec3f>> CatmullRom;
	ion::SharedPointer<ion::Animation::CLinearSplineInterpolator<ion::vec3f>> Linear;
	std::vector<ion::Scene::CSimpleMeshSceneObject *> NodeObjects;

	float Timer = 0.f;
	float TimeDirection = 1.f;
	int CurrentNode = 0;

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
