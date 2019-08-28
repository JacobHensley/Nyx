#pragma once
#include "Nyx.h"

#include "Nyx/graphics/renderer/Model.h"
#include "Nyx/graphics/renderer/camera/Camera.h"
#include "Nyx/graphics/renderer/API/FrameBuffer.h"
#include "Nyx/graphics/renderer/API/TextureCube.h"

using namespace Nyx;

struct Light
{
	Light(glm::vec3 direction, glm::vec3 radiance)
		: Direction(direction), Radiance(radiance)
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

	virtual void ImGUIRender() override;

	virtual void OnEvent(Event& e) override;
private:
	FrameBuffer* m_FrameBuffer;

	glm::mat4 m_ModelMatrix;

	Light* m_Light;
	int m_LightExponent = 2;

	TextureCube* m_TextureCube;
	Camera* m_Camera;
	Shader* m_ModelShader;
	Model* m_Model;

	VertexBuffer* m_VertexBuffer;
	Shader* m_SkyboxShader;
	VertexArray* m_VertexArray;
	IndexBuffer* m_IndexBuffer;
};