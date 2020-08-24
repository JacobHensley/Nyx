#pragma once
#include "Nyx.h"

#include "Nyx/graphics/renderer/SceneRenderer.h"
#include "Nyx/scene/component/MeshComponent.h"

#include "Nyx/Utilities.h"

#include "ImGui/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glm/gtx/matrix_decompose.hpp"

using namespace Nyx;

class EditorLayer : public Layer
{
public:
	EditorLayer();

public:
	void Init();

	void Update();
	void Render();
	void ImGUIRender();

private:
	void CreateObject(const String& name, const String& meshPath, glm::mat4& transform);

private:
	void RenderSceneWindow();
	void RenderPropertiesWindow(Ref<SceneObject> object);
	void RenderSceneSettingsWindow();
	void RenderViewport();

	void UpdateGizmoMode();

private:
	glm::vec2 m_ViewportSize;
	glm::vec2 m_ViewportPosition;
	glm::vec2 m_LastViewportSize;

	ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;

	Ref<SceneObject> m_SelectedObject;

	Ref<Scene> m_Scene;
	Ref<Camera> m_Camera;

	Ref<EnvironmentMap> m_Skybox;
	Ref<LightEnvironment> m_LightEnvironment;
	Ref<DirectionalLight> m_SceneLight;
};