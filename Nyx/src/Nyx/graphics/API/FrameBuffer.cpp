#include "NXpch.h"
#include "FrameBuffer.h"
#include "glad/glad.h"

namespace Nyx {

	FrameBuffer::FrameBuffer(int width, int height, TextureParameters parameters /*= TextureParameters()*/)
		: m_Parameters(parameters)
	{ 
		Resize(width, height);
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
		if (m_FrameBufferID)
		{
			glDeleteFramebuffers(1, &m_FrameBufferID);
			glDeleteRenderbuffers(1, &m_RenderBufferID);
		}

		m_Texture = CreateRef<Texture>(width, height, m_Parameters);

		glGenFramebuffers(1, &m_FrameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

		glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

		if (m_HasDepthBuffer)
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
		}

		if (m_HasColorBuffer)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture->GetTextureID(), 0);
		}


		glClearColor(1.1f, 0.1f, 1.1f, 1.0f);

		SetViewPortSize(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void FrameBuffer::SetViewPortSize(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

}