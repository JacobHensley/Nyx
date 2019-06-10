#pragma once
#include "Nyx.h"

#include "Nyx/graphics/renderer/Model.h"
#include "Nyx/graphics/renderer/camera/Camera.h"
#include "Nyx/graphics/renderer/API/FrameBuffer.h"

using namespace Nyx;

struct Light
{
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

	Camera* m_Camera;
	Shader* m_ModelShader;
	Model* m_Model;
};