#pragma once
#include "Nyx/graphics/API/FrameBuffer.h"

namespace Nyx {

	struct RenderPassSpecification
	{
		Ref<FrameBuffer> Framebuffer;
	};

	class RenderPass
	{
	public:
		RenderPass(const RenderPassSpecification& specification);
		const RenderPassSpecification& GetSpecification() const { return m_Specification; }

	private:
		RenderPassSpecification m_Specification;
	};

}