#pragma once
#include "Nyx/Core/Layer.h"
#include <String>

namespace Nyx {

	class ImGUILayer : public Layer
	{
	public:
		ImGUILayer(const std::string& name);
		~ImGUILayer();

	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void ImGUIRender() override;

		void Begin();
		void End();
	};

}