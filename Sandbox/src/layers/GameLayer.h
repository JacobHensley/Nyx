#pragma once
#include "Nyx.h"

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
};