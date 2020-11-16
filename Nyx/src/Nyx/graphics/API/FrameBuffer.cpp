#include "NXpch.h"
#include "FrameBuffer.h"
#include "glad/glad.h"

#include "Nyx/Application.h"

namespace Nyx {

	FrameBuffer::FrameBuffer(int width, int height, TextureParameters parameters /*= TextureParameters()*/)
		: m_Parameters(parameters)
	{ 
		Resize(width, height);
	}

	FrameBuffer::FrameBuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		if (spec.Width == 0)
		{
			auto width = Application::GetApp().GetWindow()->GetWidth();
			auto height = Application::GetApp().GetWindow()->GetHeight();
			Resize(width * spec.Scale, height * spec.Scale);
		}
		else
		{
			Resize(spec.Width, spec.Height);
		}

	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_FrameBufferID);
		glDeleteRenderbuffers(1, &m_RenderBufferID);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
		glViewport(0, 0, m_Texture->GetWidth(), m_Texture->GetHeight());
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void FrameBuffer::Attach(BufferAtachment attachment)
	{
		if (attachment == BufferAtachment::COLOR && !m_HasColorBuffer)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture->GetTextureID(), 0);
			m_HasColorBuffer = true;
		}
			
		else if (attachment == BufferAtachment::DEPTH && !m_HasDepthBuffer)
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Texture->GetWidth(), m_Texture->GetHeight());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
			m_HasDepthBuffer = true;
		}
	}

	void FrameBuffer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::Resize(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		if (m_FrameBufferID)
		{
			glDeleteFramebuffers(1, &m_FrameBufferID);

			// TODO: delete attachments

			m_ColorAttachments.clear();
			// m_DepthAttachment = 0;
		}

		glGenFramebuffers(1, &m_FrameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

		bool multisample = m_Specification.Samples > 1;
		if (multisample)
		{
			// TODO
		}
		else
		{
			for (size_t i = 0; i < m_Specification.ColorAttachments.size(); i++)
			{
				auto& textureSpec = m_Specification.ColorAttachments[i];

				GLuint textureID = m_ColorAttachments.emplace_back();
				glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
				glBindTexture(GL_TEXTURE_2D, textureID);

				if (textureSpec.format == TextureFormat::RGBA16F)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
				}
				else if (textureSpec.format == TextureFormat::RGBA)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}

				// TODO: take this from format

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureID, 0);
			}

			for (size_t i = 0; i < m_Specification.DepthAttachments.size(); i++)
			{
				auto& textureSpec = m_Specification.DepthAttachments[i];

				GLuint textureID = m_ColorAttachments.emplace_back();
				glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
				glBindTexture(GL_TEXTURE_2D, textureID);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
			}

			NX_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

	}

	void FrameBuffer::SetViewPortSize(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

}