#pragma once
#include "Nyx.h"
#include "Nyx/graphics/renderer/SceneRenderer.h"
#include "Nyx/Utilities.h"
#include "Nyx/graphics/renderer/PBRMaterial.h"
#include "Nyx/scene/component/MeshComponent.h"
#include "Nyx/scene/component/MaterialComponent.h"
#include "Nyx/scene/component/ScriptComponent.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include "ImGui/ImGuizmo.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace Nyx;

class EditorLayer : public Layer 
{
public:
	EditorLayer(const String& name);
	~EditorLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void Update() override;
	virtual void Render() override;

	virtual void ImGUIRender() override;

	virtual void OnEvent(Event& e) override;

private:
	void RenderViewport();
	void RenderMenuBar();
	void RenderMaterialMenu();
	void RenderObjectMenu();
	void RenderSceneGraphMenu();

	void CreateObject(const std::string& objectName, const std::string& meshPath, const std::string& selectedMaterialName, const std::string& scriptPath = "");
private:
	bool m_SceneGraphMenu = true;
	Ref<SceneObject> m_SelectedObject = nullptr;

private:
	bool m_MaterialMenu = false;

	TextureParameters parameters = (TextureFormat::RGB);
	String m_MaterialName = "";

	Ref<Texture> m_AlbedoMap = nullptr;
	glm::vec3 m_AlbedoValue = glm::vec3();
	bool m_UseAlbedoMap = true;

	Ref<Texture> m_MetalnessMap = nullptr;
	float m_MetalnessValue = 0.0f;
	bool m_UseMetalnessMap = true;

	Ref<Texture> m_RoughnessMap = nullptr;
	float m_RoughnessValue = 0.0f;
	bool m_UseRoughnessMap = true;

	Ref<Texture> m_NormalMap = nullptr;
	bool m_UseNormalMap = true;

	std::unordered_map<String, Ref<PBRMaterial>> m_Materials;

private:
	bool m_ObjectMenu = false;

	String m_ObjectName = "";
	String m_MeshPath = "";
	String m_ScriptPath = "";
	int m_SelectedMaterial = -1;
	String m_SelectedMaterialName = "";
	bool m_SelectedComponents[4] = { true, true, true, false };

private:
	Ref<TextureCube> m_Skybox = nullptr;
	Ref<LightEnvironment> m_LightEnv = nullptr;
	Ref<Camera> m_Camera = nullptr;
	Ref<Scene> m_Scene = nullptr;

	glm::vec2 m_RenderSpaceSize = glm::vec2();
	glm::vec2 m_LastRenderSpaceSize = glm::vec2();

private:
	Ref<PBRMaterial> m_DefaultMaterial = nullptr;
	Ref<DirectionalLight> m_Light;
	Ref<Texture> m_BRDFLutTexture = nullptr;
	Ref<TextureCube> m_IrradianceTexture = nullptr;
	Ref<TextureCube> m_RadianceTexture = nullptr;

	bool m_UsingIBL = true;
	bool m_UsingLighting = true;
};