#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"

namespace Nyx {

	SceneRenderer* SceneRenderer::s_Instance = new SceneRenderer();
	Scene* SceneRenderer::m_ActiveScene = nullptr;
	Material* SceneRenderer::m_ActiveSceneMaterial = nullptr;

	SceneRenderer::SceneRenderer()
	{
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::Begin(Scene* scene, Material* sceneMaterial)
	{
		SceneRenderer::m_ActiveScene = scene;
		SceneRenderer::m_ActiveSceneMaterial = sceneMaterial;

		Renderer::Begin(scene);
	}

	void SceneRenderer::Flush()
	{
		Renderer::Flush();
	}

	void SceneRenderer::End()
	{
		Renderer::End();
	}

	void SceneRenderer::SubmitMesh(Mesh* mesh, glm::mat4 transform, bool depthTesting, Material* objectMaterialOverride)
	{
		
		if (m_ActiveSceneMaterial != nullptr)
			Renderer::SubmitMesh(mesh, transform, m_ActiveSceneMaterial, depthTesting);
		else if (objectMaterialOverride != nullptr)
			Renderer::SubmitMesh(mesh, transform, objectMaterialOverride, depthTesting);
	//	else if (mesh->HasMaterial())
	//		Renderer::SubmitMesh(mesh, transform, mesh->GetMaterial(), depthTesting);
	}

}