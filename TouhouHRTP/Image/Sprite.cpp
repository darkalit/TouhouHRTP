#include "Sprite.h"

Sprite::Sprite(Texture* texture, GLFWwindow* window, const glm::ivec4& rect, const bool& centering)
	:
	texture	(texture),
	size	(glm::ivec2(texture->width, texture->height)),
	pos		(glm::vec2(0.0f)),
	rect	(rect)
{
	int winSizeX, winSizeY;
	glfwGetFramebufferSize(window, &winSizeX, &winSizeY);
	this->proj	= glm::ortho(0.0f, static_cast<float32>(winSizeX), static_cast<float32>(winSizeY), 0.0f, -1.0f, 1.0f);
	this->model = glm::mat4(1.0f);
	loadSprite(centering);
}

Sprite::Sprite(Texture* texture, const glm::ivec4& rect, const glm::ivec2& screenSize, const bool& centering)
	:
	texture	(texture),
	size	(glm::ivec2(texture->width, texture->height)),
	pos		(glm::vec2(0.0f)),
	rect	(rect)
{
	this->proj = glm::ortho(0.0f, static_cast<float32>(screenSize.x), static_cast<float32>(screenSize.y), 0.0f, -1.0f, 1.0f);
	this->model = glm::mat4(1.0f);
	loadSprite(centering);
}

void Sprite::loadSprite(const bool& centering)
{
	if (centering)
	{
		const float32	halfx	= static_cast<float32>(this->rect.z - this->rect.x) / 2.0f, 
						halfy	= static_cast<float32>(this->rect.w - this->rect.y) / 2.0f;
		float32 quad[] = {
			// positions   // texcoords
			halfx,  halfy, static_cast<float32>(this->rect.z) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.y) / static_cast<float32>(this->size.y),
		   -halfx, -halfy, static_cast<float32>(this->rect.x) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.w) / static_cast<float32>(this->size.y),
		   -halfx,  halfy, static_cast<float32>(this->rect.x) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.y) / static_cast<float32>(this->size.y),

		   -halfx, -halfy, static_cast<float32>(this->rect.x) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.w) / static_cast<float32>(this->size.y),
			halfx,  halfy, static_cast<float32>(this->rect.z) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.y) / static_cast<float32>(this->size.y),
			halfx, -halfy, static_cast<float32>(this->rect.z) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.w) / static_cast<float32>(this->size.y)
		};
		memcpy(this->quadV, quad, sizeof quad);
	}		
	else
	{
		const float32	x	= static_cast<float32>(this->rect.z - this->rect.x),
						y	= static_cast<float32>(this->rect.w - this->rect.y);
		float32 quad[] = {
			// positions	// texcoords
			x,		y,		static_cast<float32>(this->rect.z) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.y) / static_cast<float32>(this->size.y),
			0.0f,	0.0f,	static_cast<float32>(this->rect.x) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.w) / static_cast<float32>(this->size.y),
			0.0f,	y,		static_cast<float32>(this->rect.x) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.y) / static_cast<float32>(this->size.y),

			0.0f,	0.0f,	static_cast<float32>(this->rect.x) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.w) / static_cast<float32>(this->size.y),
			x,		y,		static_cast<float32>(this->rect.z) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.y) / static_cast<float32>(this->size.y),
			x,		0.0f,	static_cast<float32>(this->rect.z) / static_cast<float32>(this->size.x), static_cast<float32>(this->rect.w) / static_cast<float32>(this->size.y)
		};
		memcpy(this->quadV, quad, sizeof quad);
	}

	glGenBuffers(1, &this->VBO);
	glGenVertexArrays(1, &this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->quadV), &this->quadV, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Sprite::clear(Shader* shader)
{
	shader->use();
	shader->setMat4("model", this->model);
	shader->setMat4("projection", this->proj);
	model = glm::mat4(1.0f);
}

void Sprite::draw(Shader* shader, const glm::vec2& position, const glm::vec2& scale, const float32& angle)
{
	shader->use();
	this->model = glm::mat4(1.0f);
	this->model = glm::translate(this->model, glm::vec3(position, 0.0f));
	this->model = glm::rotate(this->model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	this->model = glm::scale(this->model, glm::vec3(scale, 1.0f));
	shader->setMat4("model", this->model);
	shader->setMat4("projection", this->proj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture->ID);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

glm::ivec4 Sprite::getEdges()
{
	return this->rect;
}

glm::vec2 Sprite::getSize()
{
	return {	static_cast<float32>(this->rect.z - this->rect.x), 
				static_cast<float32>(this->rect.w - this->rect.y)};
}

Rect Sprite::getBounds()
{
	this->bounds.bottomLeft		= glm::vec2(pos.x - static_cast<float32>(this->rect.z - this->rect.x) / 2.0f,
											pos.y - static_cast<float32>(this->rect.w - this->rect.y) / 2.0f);
	this->bounds.topLeft		= glm::vec2(pos.x - static_cast<float32>(this->rect.z - this->rect.x) / 2.0f,
											pos.y + static_cast<float32>(this->rect.w - this->rect.y) / 2.0f);
	this->bounds.topRight		= glm::vec2(pos.x + static_cast<float32>(this->rect.z - this->rect.x) / 2.0f,
											pos.y + static_cast<float32>(this->rect.w - this->rect.y) / 2.0f);
	this->bounds.bottomRight	= glm::vec2(pos.x + static_cast<float32>(this->rect.z - this->rect.x) / 2.0f,
											pos.y - static_cast<float32>(this->rect.w - this->rect.y) / 2.0f);
	return this->bounds;
}


