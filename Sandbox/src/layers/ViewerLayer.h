#pragma once
#include "Nyx.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui/ImGuizmo.h"

#include "Nyx/scene/component/MeshComponent.h"
#include "Nyx/graphics/renderer/Mesh.h"

using namespace Nyx;

class ViewerLayer : public Layer
{
	struct Light
	{
		glm::vec3 radiance;
		glm::vec3 direction;


		Light(glm::vec3 radiance, glm::vec3 direction)
			: radiance(radiance), direction(direction)
		{
		}

		Light()
			: radiance(1.0f, 1.0f, 1.0f), direction(0.0f, 0.0f, 0.0f)
		{
		}
	};

public:
	ViewerLayer(const String& name);
	~ViewerLayer();

	virtual void Update() override;
	virtual void Render() override;

	virtual void ImGUIRender() override;

	virtual void OnEvent(Event& e) override;

	bool OnMouseClick(MouseButtonPressedEvent& e);

private:
	void MousePick();

	String OpenFileExplorer();

private:
	//Renderspace info
	FrameBuffer* m_RenderSpaceFrameBuffer = nullptr;
	glm::vec2 m_RenderSpacePosition = glm::vec2();
	int m_RenderSpaceHeight = 0;
	int m_RenderSpaceWidth = 0;

	//Ray Info
	Ray m_MouseRay = Ray();
	bool m_HoldRay = true;

	Camera* m_Camera = nullptr;

	int m_GizmoMode = 0;
	int m_SceneMode = 0;

	//Texture Cubes
	TextureCube* m_IrradianceTexture = nullptr;
	TextureCube* m_RadianceTexture = nullptr;

	//Skybox
	Shader* m_SkyboxShader = nullptr;
	Mesh* m_SkyboxMesh = nullptr;

	//Grid
	Shader* m_GridShader = nullptr;
	Mesh* m_GridMesh = nullptr;
	glm::mat4 m_GridTransform;
	float m_GridScale = 501.0f;
	float m_GridResolution = 0.02f;

	//PBR shader and textures
	Shader* m_PBRShader = nullptr;
	Texture* m_BRDFLutTexture = nullptr;
	Texture* m_AlbedoMap = nullptr;
	Texture* m_MetalnessMap = nullptr;
	Texture* m_NormalMap = nullptr;
	Texture* m_RoughnessMap = nullptr;

	glm::vec3 m_Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_Metalness = 0.5f;
	float m_Roughness = 0.5f;

	bool m_UsingAlbedoMap = true;
	bool m_UsingMetalnessMap = true;
	bool m_UsingNormalMap = true;
	bool m_UsingRoughnessMap = true;

	bool m_UsingIBL = true;
	bool m_UsingLighting = true;

	float ex = 1.0f;
	float m_LightMultiplier = 1.0f;
	glm::vec3 m_LightColor = { 1.0f, 1.0f, 1.0f };

	//Lights
	Light m_Light;

	FrameBuffer* m_HDRBuffer = nullptr;
	Shader* m_HDRShader = nullptr;
	Mesh* m_HDRQuad = nullptr;

	//Scene
	Scene* m_Scene = nullptr;

	//Cerberus data
	Mesh* m_CerberusMesh = nullptr;
	glm::mat4 m_CerberusTransform;
	
	//Cerberus object
	SceneObject* m_CerberusObject = nullptr;
	MeshComponent* m_CerberusMeshComponent = nullptr;
	TransformComponent* m_CerberusTransformComponent = nullptr;

	//Sphere object
	SceneObject* m_SphereObject = nullptr;
};