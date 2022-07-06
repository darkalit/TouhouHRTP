#pragma once

#include "../headers.h"
#include "Font.h"
#include "../Render/Resources.h"

class Text
{
public:
	Text(Font* font, const uint32& screen_width, const uint32& screen_height);

	void set_color(float32 r, float32 g, float32 b);
	void set_pos(const float32& x, const float32& y);
	void set_text(const std::string& text);

	void draw();

private:
	Font*		font_		{};
	float32		win_height_	{};
	std::string text_		{};
	glm::mat4	model_		{}, 
				proj_		{};
	glm::ivec2	size_		{};
	glm::vec2	pos_		{0.0f, 0.0f};
	uint32		vao_		{}, 
				vbo_		{};
	//float32		quadV[24]	{};
};
