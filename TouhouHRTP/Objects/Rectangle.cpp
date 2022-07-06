#include "Rectangle.h"

Rectangle::Rectangle(const glm::ivec2& size, const glm::ivec2& screenSize)
	:
	pos_(glm::vec2(0.0f)),
	size_(size)
{
	this->proj_ = glm::ortho(0.0f, static_cast<float32>(screenSize.x), static_cast<float32>(screenSize.y), 0.0f, -1.0f, 1.0f);
	this->model_ = glm::mat4(1.0f);

	const float32	halfx	= static_cast<float32>(this->size_.x) / 2.0f, 
					halfy	= static_cast<float32>(this->size_.y) / 2.0f;
	float quad[] = {
		// positions
		halfx,  halfy,
	   -halfx, -halfy,
	   -halfx,  halfy,
	   -halfx, -halfy,
		halfx,  halfy,
		halfx, -halfy
	};
	memcpy(this->quadV, quad, sizeof quad);

	glGenBuffers(1, &this->VBO);
	glGenVertexArrays(1, &this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof this->quadV, &this->quadV, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Rectangle::set_pos(const float32& x, const float32& y)
{
	this->pos_ = glm::vec2(x, y);
}

void Rectangle::set_scale(const glm::vec2& scale)
{
	this->scale_ = scale;
}

auto Rectangle::get_pos() -> glm::vec2
{
	return this->pos_;
}

auto Rectangle::get_size() -> glm::vec2
{
	return this->size_;
}

void Rectangle::draw(Shader* shader)
{
	shader->use();
	this->model_ = glm::mat4(1.0f);
	this->model_ = glm::translate(this->model_, glm::vec3(this->pos_, 0.0f));
	this->model_ = glm::scale(this->model_, glm::vec3(this->scale_, 1.0f));
	shader->setMat4("model", this->model_);
	shader->setMat4("projection", this->proj_);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
