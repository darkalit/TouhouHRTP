#pragma once
#include "../headers.h"
#include "../Render/Shader.h"

class Rectangle
{
public:
	Rectangle	(const glm::ivec2& size,
				 const glm::ivec2& screenSize = glm::ivec2(0, 0));

	void set_pos		(const float32& x, const float32& y);
	void set_scale		(const glm::vec2& scale);
	auto get_pos		() -> glm::vec2;
	auto get_size		() -> glm::vec2;
	void draw			(Shader* shader);

private:
	glm::mat4	model_	{},
				proj_	{};
	glm::vec2	pos_	{};
	glm::vec2	size_	{};
	glm::vec2	scale_	{1.0f, 1.0f};
	uint32		VAO		{}, 
				VBO		{};
	float32		quadV[12]{};
};

