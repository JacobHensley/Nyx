#pragma once
#include "Nyx.h"

#include "Nyx/graphics/renderer/SceneRenderer.h"
#include "Nyx/scene/component/MeshComponent.h"
#include "Nyx/scene/component/MaterialComponent.h"
#include "Nyx/graphics/renderer/PBRMaterial.h"
#include "Nyx/graphics/renderer/DebugRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"
#include "Nyx/Utilities.h"

#include <imgui/imgui_internal.h>
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "ImGui/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

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

	void OnEvent(Event& e);

private:
	void CreateObject(const String& name, const String& meshPath, glm::mat4& transform);

private:
	void RenderSceneWindow();
	void RenderPropertiesWindow(Ref<SceneObject> object);
	void RenderSceneSettingsWindow();
	void RenderMainMenu();
	void RenderViewport();

	void UpdateGizmoMode();
	void MousePick();
	bool SelectObject();

private:
	glm::vec2 m_ViewportSize;
	glm::vec2 m_ViewportPosition;
	glm::vec2 m_LastViewportSize;
	bool m_MouseOverViewport = false;

	ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
	Ray m_MouseRay;

	Ref<SceneObject> m_SelectedObject;

	Ref<Scene> m_Scene;
	Ref<Camera> m_EditorCamera;

	Ref<EnvironmentMap> m_Skybox;
	Ref<LightEnvironment> m_LightEnvironment;
	Ref<DirectionalLight> m_SceneLight;

	AssetHandle defaultMesh;
};