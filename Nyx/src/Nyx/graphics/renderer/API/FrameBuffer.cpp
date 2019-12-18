#include "NXpch.h"
#include "FrameBuffer.h"
#include "glad/glad.h"

namespace Nyx {

	FrameBuffer::FrameBuffer(int width, int height, TextureParameters parameters /*= TextureParameters()*/)
		: m_Parameters(parameters)
	{ 
		m_Texture = new Texture(width, height, m_Parameters);

		glGenFramebuffers(1, &m_FrameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

		glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture->GetTextureID(), 0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_FrameBufferID);
		glDeleteRenderbuffers(1, &m_RenderBufferID);

		m_Texture->~Texture();
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

		glViewport(0, 0, m_Texture->GetWidth(), m_Texture->GetHeight());
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Clear()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Resize(int width, int height)
	{
		glDeleteFramebuffers(1, &m_FrameBufferID);
		glDeleteRenderbuffers(1, &m_RenderBufferID);

		m_Texture->~Texture();

		m_Texture = new Texture(width, height, m_Parameters);

		glGenFramebuffers(1, &m_FrameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

		glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture->GetTextureID(), 0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void FrameBuffer::SetViewPortSize(int x, int y, int width, int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
		glViewport(x, y, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}