#pragma once
#include "Nyx.h"

#include "Nyx/graphics/renderer/Model.h"
#include "Nyx/graphics/renderer/camera/Camera.h"

using namespace Nyx;

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
	Shader* m_Shader;
	Texture* m_Texture;
	IndexBuffer* m_IndexBuffer;
	VertexArray* m_VertexArray;
	VertexBuffer* m_VertexBuffer;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	Camera* cam;
	Shader* m_ModelShader;
	Model* m_Model;
};