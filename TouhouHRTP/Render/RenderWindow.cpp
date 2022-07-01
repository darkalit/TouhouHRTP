#include "RenderWindow.h"

RenderWindow::RenderWindow(const Shader& shader, const int32& width, const int32& height)
	:
	shader(shader),
	size(glm::ivec2(width, height))
{
	// initialize renderbuffer/framebuffer object
	glGenFramebuffers	(1, &this->_MSFBO);
	glGenFramebuffers	(1, &this->_FBO);
	glGenRenderbuffers	(1, &this->_RBO);

	// initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, this->_MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->_RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); // allocate storage for render buffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->_RBO); // attach MS render buffer object to framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

	// also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
	glBindFramebuffer(GL_FRAMEBUFFER, this->_FBO);
	//glGenTextures(1, &this->texture.ID);
	this->texture.filterMax = GL_NEAREST;
	this->texture.filterMin = GL_NEAREST;
	this->texture.generate(width, height, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture.ID, 0); // attach texture to framebuffer as its color attachment
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->initRender();
}

RenderWindow::~RenderWindow()
{
	glDeleteFramebuffers(1, &this->_MSFBO);
	glDeleteFramebuffers(1, &this->_FBO);
	glDeleteRenderbuffers(1, &this->_RBO);
}

void RenderWindow::clear()
{
	int32 error = glGetError();
	if (error != GL_NO_ERROR)
		std::cout << GLErrorToString(error) << " | 0x" << std::hex << error << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, this->_MSFBO);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderWindow::display()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->_MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _FBO);
	glBlitFramebuffer(0, 0, this->size.x, this->size.y, 0, 0, this->size.x, this->size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);

	this->shader.use();
	glActiveTexture(GL_TEXTURE0);
	this->texture.bind();
	
	// render Quad
	glBindVertexArray(this->_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderWindow::initRender()
{
	uint32 quadVBO;
	float32 quadV[] = {
		// positions  // texcoords
		1.0f,  1.0f,  1.0f, 0.0f,
	   -1.0f, -1.0f,  0.0f, 1.0f,
	   -1.0f,  1.0f,  0.0f, 0.0f,

	   -1.0f, -1.0f,  0.0f, 1.0f,
		1.0f,  1.0f,  1.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 1.0f
	};

	glGenBuffers(1, &quadVBO);
	glGenVertexArrays(1, &this->_quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadV), &quadV, GL_STATIC_DRAW);

	glBindVertexArray(this->_quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
