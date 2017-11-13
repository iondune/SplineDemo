
#pragma once

#include <ionScene.h>


namespace ion
{

	class CLineSceneObject : public ion::Scene::ISceneObject
	{

	public:

		CLineSceneObject();
		~CLineSceneObject();

		virtual void Load(ion::Scene::CRenderPass * RenderPass);
		virtual void Draw(ion::Scene::CRenderPass * RenderPass);

		virtual void SetShader(ion::SharedPointer<ion::Graphics::IShader> Shader);

		void ResetLines();
		void AddLine(vec3f const & A, vec3f const & B, color3f const & Color);

	protected:

		vector<uint> Indices;
		vector<float> Vertices;
		bool DataNeedsUpload = true;

		size_t IndexCounter = 0;

		ion::SharedPointer<ion::Graphics::IPipelineState> PipelineState;
		ion::SharedPointer<ion::Graphics::IShader> Shader;

		ion::SharedPointer<ion::Graphics::IIndexBuffer> IndexBuffer;
		ion::SharedPointer<ion::Graphics::IVertexBuffer> VertexBuffer;

	};

}
