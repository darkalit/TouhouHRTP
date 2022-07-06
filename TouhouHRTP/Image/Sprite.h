#pragma once

#include "../Render/RenderWindow.h"
#include "../Render/Shader.h"

struct Rect
{
	glm::vec2 bottomLeft;
	glm::vec2 topLeft;
	glm::vec2 topRight;
	glm::vec2 bottomRight;
};

class Sprite
{
public:
	Sprite			(Texture* texture, GLFWwindow* window, 
					 const glm::ivec4& rect, const bool& centering = true);
	Sprite			(Texture* texture, const glm::ivec4& rect, 
					 const glm::ivec2& screenSize = glm::ivec2(0, 0), 
					 const bool& centering = true);
	void loadSprite	(const bool& centering = true);
	void clear		(Shader* shader);
	void draw		(Shader* shader,
					 const glm::vec2& position, 
					 const glm::vec2& scale = glm::vec2(1.0f, 1.0f), 
					 const float32& angle = 0.0f);

	auto getEdges	() -> glm::ivec4;
	auto getSize	() -> glm::vec2;
	auto getBounds	() -> Rect;

private:
	glm::mat4	model	{},
				proj	{};
	Texture*	texture	{};
	glm::ivec2	size	{};
	glm::vec2	pos		{};
	glm::ivec4	rect	{};
	Rect		bounds	{};
	uint32		VAO		{}, 
				VBO		{};
	float32		quadV[24]{};
};
