#include "EditorLayer.h"
#include <glm/gtc/type_ptr.hpp>

namespace Nyx {

	EditorLayer::EditorLayer()
		: Layer("Editor")
	{
		Init();
	}

	void EditorLayer::Init()
	{
		m_EditorCamera = CreateRef<Camera>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 100.0f));
		m_Scene = CreateRef<Scene>();

		m_ViewportPanel = ViewportPanel();
		m_HierarchyPanel = SceneHierarchyPanel();
		m_ObjectPanel = ObjectPanel(m_Scene);
	}

	void EditorLayer::Update()
	{
		m_Scene->Update();
		m_EditorCamera->Update();

		UpdateShortcuts();
	}

	void EditorLayer::Render()
	{
		m_Scene->Render(m_EditorCamera);
	}

	void EditorLayer::ImGUIRender()
	{
		m_ViewportPanel.Render(m_EditorCamera, m_SelectedObject);
		m_HierarchyPanel.Render(m_Scene, m_SelectedObject);
		m_ObjectPanel.Render(m_SelectedObject);

		SceneRenderer::OnImGuiRender();

		RenderShaderReload();
		RenderToolBar();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (e.GetEventType() == KeyPressed)
		{
			if (((KeyPressedEvent&)e).GetKeycode() == NX_KEY_G)
				 m_ViewportPanel.SetGizmoMode(ImGuizmo::TRANSLATE);
			if (((KeyPressedEvent&)e).GetKeycode() == NX_KEY_R)
				 m_ViewportPanel.SetGizmoMode(ImGuizmo::ROTATE);
			if (((KeyPressedEvent&)e).GetKeycode() == NX_KEY_S)
				 m_ViewportPanel.SetGizmoMode(ImGuizmo::SCALE);
		}
		
		if (e.GetEventType() == MouseButtonReleased && m_ViewportPanel.IsHovered() && !m_EditorCamera->IsMoving() && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
		{
			Ray mouseRay = CastCameraRay();
			m_SelectedObject = PickObject(mouseRay);
		}
		
		m_EditorCamera->OnEvent(e);
	}

	void EditorLayer::UpdateShortcuts()
	{
		if (Input::IsKeyPressed(NX_KEY_LEFT_CONTROL))
		{
			if (Input::IsKeyPressed(NX_KEY_S))
			{
				if (m_Scene->GetPath() != "")
					SaveScene(m_Scene->GetPath());
				else
					SaveScene();
			}	
			else if (Input::IsKeyPressed(NX_KEY_O))
				LoadScene();
			else if (Input::IsKeyPressed(NX_KEY_N))
				m_Scene = CreateRef<Scene>();
		}
	}

	// ----- Move everything past here to somewhere else -----
	void EditorLayer::RenderShaderReload()
	{
		ImGui::Begin("Shader Reload");

		if (ImGui::Button("Reload PBR Shader"))
			SceneRenderer::GetPBRShader()->Reload();;
		if (ImGui::Button("Reload Glass Shader"))
			SceneRenderer::GetGlassShader()->Reload();
		if (ImGui::Button("Reload Composite Shader"))
			SceneRenderer::GetCompositeShader()->Reload();
		if (ImGui::Button("Reload Skybox Shader"))
			SceneRenderer::GetSkyboxShader()->Reload();

		ImGui::End();
	}

	void EditorLayer::RenderToolBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "CTRL+S")) 
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					LoadScene();
				}

				if (ImGui::MenuItem("New", "CTRL+N"))
				{
					m_Scene = CreateRef<Scene>();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void EditorLayer::SaveScene(const std::string& path)
	{
		if (path != "")
		{
			SceneSerializer::Save(m_Scene, path);
			return;
		}

		std::string file = SaveFileExplorer("nyx\0*.nyx\0");

		if (file != "")
		{
			SceneSerializer::Save(m_Scene, file);
			m_Scene->SetPath(file);
		}
	}

	void EditorLayer::LoadScene()
	{
		std::string file = OpenFileExplorer("nyx\0*.nyx\0");
		if (file != "")
		{
			m_Scene = SceneSerializer::Load(file);
			m_Scene->SetPath(file);
		}
	}

	Ray EditorLayer::CastCameraRay()
	{
		glm::vec2 NDC = m_ViewportPanel.GetMouseNDC();

		glm::vec4 mouseClipPos = { NDC.x, NDC.y, -1.0f, 1.0f };

		glm::mat4 inverseProjection = glm::inverse(m_EditorCamera->GetProjectionMatrix());
		glm::mat3 inverseView = glm::inverse(glm::mat3(m_EditorCamera->GetViewMatrix()));

		glm::vec3 cameraPosition = glm::inverse(m_EditorCamera->GetViewMatrix())[3];

		glm::vec4 ray = inverseProjection * mouseClipPos;
		glm::vec3 rayDirection = inverseView * glm::vec3(ray);

		return Ray(cameraPosition, rayDirection);
	}

	SceneObject EditorLayer::PickObject(Ray& ray)
	{
		SceneObject result = {};
		bool foundObject = false;

		m_Scene->GetRegistry().each([&](auto objectID)
		{
			SceneObject object = { objectID, m_Scene.get() };
			if (!object || !object.HasComponent<MeshComponent>() || foundObject)
				return;

			Ref<Mesh> mesh = object.GetComponent<MeshComponent>().GetMesh();
			glm::mat4& transform = object.GetComponent<TransformComponent>().GetTransform();

			for (const auto& subMesh : mesh->GetSubMeshs())
			{
				if (foundObject)
					break;

				glm::mat4 worldTransform = transform * subMesh.Transform;

				AABB boundingBox = subMesh.BoundingBox;
				boundingBox.Min = worldTransform * glm::vec4(boundingBox.Min, 1.0f);
				boundingBox.Max = worldTransform * glm::vec4(boundingBox.Max, 1.0f);

				float distance;
				bool intersects = ray.IntersectsAABB(boundingBox, distance);

				if (intersects)
				{
					Ray localRay = ray;
					localRay.Origin = glm::inverse(worldTransform) * glm::vec4(localRay.Origin, 1.0f);
					localRay.Direction = glm::inverse(glm::mat3(worldTransform)) * localRay.Direction;

					for (uint32_t i = subMesh.TriangleOffset; i < (subMesh.TriangleOffset + subMesh.TriangleCount); i++)
					{
						Triangle triangle = mesh->GetTriangles()[i];
						bool triangleIntersection = localRay.IntersectsTriangle(triangle);

						if (triangleIntersection)
						{
							result = object;
							foundObject = true;
							break;
						}
					}
				}
			}
		});

		return result;
	}

}