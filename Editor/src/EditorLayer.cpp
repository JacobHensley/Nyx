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
		m_Scene = SceneSerializer::Load("assets/scenes/GlassTest.nyx");

		m_ViewportPanel = ViewportPanel();
		m_HierarchyPanel = SceneHierarchyPanel(m_Scene);
		m_ObjectPanel = ObjectPanel(m_Scene);
	}

	void EditorLayer::Update()
	{
		m_Scene->Update();
		m_EditorCamera->Update();
	}

	void EditorLayer::Render()
	{
		m_Scene->Render(m_EditorCamera);
	}

	void EditorLayer::ImGUIRender()
	{
		m_ViewportPanel.Render(m_EditorCamera, m_SelectedObject);
		m_HierarchyPanel.Render(m_SelectedObject);
		m_ObjectPanel.Render(m_SelectedObject);

		SceneRenderer::OnImGuiRender();

		ImGui::Begin("Scene Settings");
		ImGui::SliderFloat3("Point Light", glm::value_ptr(m_Scene->GetLightEnvironment()->GetPointLight().Position), -1, 1);
		ImGui::SliderFloat3("Dir Light", glm::value_ptr(m_Scene->GetLightEnvironment()->GetDirectionalLight().Direction), -1, 1);
		ImGui::End();
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

					for (int i = subMesh.TriangleOffset; i < (subMesh.TriangleOffset + subMesh.TriangleCount); i++)
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