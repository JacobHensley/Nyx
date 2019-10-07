#pragma once
#include "Nyx.h"
#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/math/Ray.h"

#include "Nyx/scene/Scene.h"
#include "Nyx/scene/SceneObject.h"
#include "Nyx/scene/component/MeshComponent.h"
#include "Nyx/scene/component/TransformComponent.h"

using namespace Nyx;

struct Light
{
	Light(glm::vec3 direction, glm::vec3 radiance)
		: Direction(direction), Radiance(radiance)
	{
	}

	Light()
		: Direction(glm::vec3(1.0f, 1.0f, 1.0f)), Radiance(glm::vec3(1.0f, 1.0f, 1.0f))
	{
	}

	glm::vec3 Direction;
	glm::vec3 Radiance;
};

class GameLayer : public Layer
{
public:
	GameLayer(const String& name);

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void Update() override;
	virtual void Render() override;

	void MousePick();

	virtual void ImGUIRender() override;

	virtual void OnEvent(Event& e) override;
private:
	FrameBuffer* m_FrameBuffer;

	glm::mat4 m_ModelMatrix;

	Scene m_Scene;
	SceneObject object;
	MeshComponent* meshComponent;
	TransformComponent* transformComponent;

	Light* m_Light;
	int m_LightExponent = 2;

	int m_RenderSpaceHeight = 0;
	int m_RenderSpaceWidth = 0;
	glm::vec2 m_RenderSpacePos;

	Ray m_Ray;

	TextureCube* m_TextureCube;
	Camera* m_Camera;
	Shader* m_ModelShader;
	Mesh* m_Model;

	VertexBuffer* m_VertexBuffer;
	Shader* m_SkyboxShader;
	VertexArray* m_VertexArray;
	IndexBuffer* m_IndexBuffer;
};