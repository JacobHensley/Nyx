#pragma once
#include "Nyx.h"

#include "Nyx/graphics/renderer/Model.h"

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
	IndexBuffer* m_IndexBuffer;
	VertexArray* m_VertexArray;
	VertexBuffer* m_VertexBuffer;
	float transX = 0;
	float transY = 0;
	float transZ = 0;

	Shader* m_ModelShader;
	Model* m_Model;
};