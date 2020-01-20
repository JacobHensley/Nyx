#pragma once
#include "Nyx.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "Nyx/scene/component/MeshComponent.h"
#include "Nyx/scene/component/MaterialComponent.h"
#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/PRBMaterial.h"

using namespace Nyx;

	struct Light
	{
		glm::vec3 radiance;
		glm::vec3 direction;

		Light(glm::vec3 radiance, glm::vec3 direction)
			: radiance(radiance), direction(direction)
		{
		}

		Light()
			: radiance(glm::vec3()), direction(glm::vec3())
		{
		}
	};

class ViewerLayer : public Layer
{
public:
	ViewerLayer(const String& name);
	~ViewerLayer();

public:
	virtual void Update() override;
	virtual void Render() override;

	virtual void ImGUIRender() override;

	virtual void OnEvent(Event& e) override;

private:
	void MousePick();
	bool OnMouseClick(MouseButtonPressedEvent& e);

private:
	//PBR Values
	Shader* m_PBRShader = nullptr;

	Texture* m_BRDFLutTexture = nullptr;
	Texture* m_AlbedoMap = nullptr;
	Texture* m_MetalnessMap = nullptr;
	Texture* m_NormalMap = nullptr;
	Texture* m_RoughnessMap = nullptr;
	TextureCube* m_IrradianceTexture = nullptr;
	TextureCube* m_RadianceTexture = nullptr;

	glm::vec3 m_Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_Metalness = 0.5f;
	float m_Roughness = 0.5f;

	//PRB Editor options
	bool m_UsingAlbedoMap = true;
	bool m_UsingMetalnessMap = true;
	bool m_UsingNormalMap = true;
	bool m_UsingRoughnessMap = true;

	bool m_UsingIBL = true;
	bool m_UsingLighting = true;

	//Scene
	Camera* m_Camera = nullptr;
	Scene* m_Scene = nullptr;
	
	//Scene Object
	SceneObject* m_SceneObject = nullptr;
	MeshComponent* m_ObjectMeshComponent = nullptr;
	TransformComponent* m_ObjectTransformComponent = nullptr;
	MaterialComponent* m_ObjectMaterialComponent = nullptr;

	//Scene Object Data
	Mesh* m_ObjectMesh = nullptr;
	glm::mat4 m_ObjectTransform = glm::mat4(1.0f);
	PBRMaterial* m_ObjectMaterial = nullptr;

	//Grid Shader and data
	Shader* m_GridShader = nullptr;
	Mesh* m_GridMesh = nullptr;
	glm::mat4 m_GridTransform = glm::mat4(1.0f);
	
	float m_GridScale = 501.0f;
	float m_GridResolution = 0.02f;

	//Skybox
	Shader* m_SkyboxShader = nullptr;

	//HDR pass
	Shader* m_HDRShader = nullptr;
	FrameBuffer* m_HDRBuffer = nullptr;
	float m_Exposure = 1.0f;

	//Fullscreen quad (For skybox and HDR)
	Mesh* m_FullscreenQuad = nullptr;

	//Renderspace info
	glm::vec2 m_RenderSpaceSize = glm::vec2();
	glm::vec2 m_RenderSpacePosition = glm::vec2();
	FrameBuffer* m_RenderSpaceBuffer = nullptr;

	//Light data
	Light m_Light;
	glm::vec3 m_LightColor = { 1.0f, 1.0f, 1.0f };
	float m_LightMultiplier = 1.0f;

	//Misc editor info
	Ray m_MouseRay = Ray();
	int m_GizmoMode = 0;
};