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
public:
	ViewerLayer(const String& name);
	~ViewerLayer();

	virtual void Update() override;
	virtual void Render() override;

	virtual void ImGUIRender() override;

	virtual void OnEvent(Event& e) override;

	bool OnMouseClick(MouseButtonPressedEvent& e);

private:
	void InitSkyboxQuad();
	void InitGridQuad();
	void MousePick();

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

	ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;

	//Texture Cubes
	TextureCube* m_IrradianceTexture = nullptr;
	TextureCube* m_RadianceTexture = nullptr;

	//Skybox
	Shader* m_SkyboxShader = nullptr;
	VertexBuffer* m_SkyboxVertexBuffer = nullptr;
	VertexArray* m_SkyboxVertexArray = nullptr;
	IndexBuffer* m_SkyboxIndexBuffer = nullptr;

	//Grid
	Shader* m_GridShader = nullptr;
	VertexBuffer* m_GridVertexBuffer = nullptr;
	VertexArray* m_GridVertexArray = nullptr;
	IndexBuffer* m_GridIndexBuffer = nullptr;
	glm::mat4 m_GridTransform;

	//PBR shader and textures
	Shader* m_PBRShader = nullptr;
	Texture* m_BRDFLutTexture = nullptr;
	Texture* m_AlbedoMap = nullptr;
	Texture* m_MetalnessMap = nullptr;
	Texture* m_NormalMap = nullptr;
	Texture* m_RoughnessMap = nullptr;

	//Cerberus data
	Mesh* m_CerberusMesh = nullptr;
	glm::mat4 m_CerberusTransform;

	//Scene and Components
	Scene* m_Scene = nullptr;
	SceneObject* m_CerberusObject = nullptr;
	MeshComponent* m_CerberusMeshComponent = nullptr;
	TransformComponent* m_CerberusTransformComponent = nullptr;
};