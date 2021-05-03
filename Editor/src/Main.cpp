#include "Nyx/Core/Application.h"
#include "EditorLayer.h"
#include <iostream>
#include <memory>

#include <map>           // tree vector -> tree operator<()
#include <unordered_map> // hash map O(1)

#include <set> // material

#include <iostream>

#include <glm/glm.hpp>

struct TestMaterial
{
	std::string Name;
	uint64_t SortKey;
	bool Opaque = true;

	TestMaterial(std::string name, uint64_t sortKey)
		: Name(name), SortKey(sortKey) {}

	bool operator<(const TestMaterial& other) const
	{
		return SortKey < other.SortKey;
	}
};

struct TestSubmesh
{
	std::string Name;
	glm::vec3 Position{ 0.0f };
};

int main()
{
	Ref<TestMaterial> glass = CreateRef<TestMaterial>("Glass", 100);
	Ref<TestMaterial> pbr = CreateRef<TestMaterial>("PBR", 0);

	glm::vec3 cameraPosition{ 0.0f };

	std::map<Ref<TestMaterial>, std::vector<TestSubmesh>, std::owner_less<std::shared_ptr<TestMaterial>>> draws;

	draws[pbr].push_back(TestSubmesh{ "PBR mesh", {-5.0f, 5.0f, 5.0f} });
	draws[glass].push_back(TestSubmesh{ "Glass mesh", {-5.0f, 5.0f, 4.8f} });
	draws[pbr].push_back(TestSubmesh{ "PBR mesh", {0.0f, 2.0f, 2.0f} });
	draws[glass].push_back(TestSubmesh{ "Glass mesh", {5.5f, 5.0f, 5.0f} });
	draws[pbr].push_back(TestSubmesh{ "PBR mesh", {0.0f, 10.0f, 0.0f} });

	for (auto& [material, meshList] : draws)
	{
		//std::cout << material.Name << std::endl;
		Ref<TestMaterial> m = material;
		std::sort(meshList.begin(), meshList.end(), [m, cameraPosition](auto& a, auto& b)
		{ 
			float camMeshA = glm::length(cameraPosition - a.Position);
			float camMeshB = glm::length(cameraPosition - b.Position);
//			if (m->Opaque)
//				return camMeshA > camMeshB;
			return camMeshA < camMeshB;
		});
		for (auto& submesh : meshList)
		{
			float distanceFromCamera = glm::length(cameraPosition - submesh.Position);
			std::cout << submesh.Name << " - " << distanceFromCamera << std::endl;
		}

	}


	Ref<Application> application = CreateRef<Application>();
	Ref<EditorLayer> editor = CreateRef<EditorLayer>();

	application->PushLayer(editor);
	application->Run();

	return 0;
}