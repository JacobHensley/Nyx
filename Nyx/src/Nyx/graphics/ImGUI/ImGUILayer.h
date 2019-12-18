#pragma once
#include "NXpch.h"
#include "Nyx/Graphics/Layers/Layer.h"

namespace Nyx {

	class ImGUILayer : public Layer
	{
	public:
		ImGUILayer(const String& name);
		~ImGUILayer();

	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void ImGUIRender() override;

		void Begin();
		void End();
	};

}