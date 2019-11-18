#include "NXpch.h"
#include "Material.h"
#include "glad/glad.h"

namespace Nyx {
	Material::Material(Shader* shader)
		:	m_Shader(shader)
	{
	}

	Material::~Material()
	{
	}

	void Material::Bind()
	{
		m_Shader->Bind();
	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

}