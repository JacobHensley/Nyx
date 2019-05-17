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
	Camera* cam;
	Shader* m_ModelShader;
	Model* m_Model;
	float m_Angle = 0.0f;
};