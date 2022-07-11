#pragma once

#include "../Debug.h"
#include "../Utility.h"
#include "../Image/Texture.h"
#include "../Resources.h"


class RenderWindow
{
public:
	RenderWindow	(const int32& width, const int32& height);
	~RenderWindow	();
	
	void clear		();
	void display	();
	
	Texture		texture;
	glm::ivec2	size;
private:
	void	initRender	();
	uint32	_FBO		{}, 
			_MSFBO		{}, 
			_RBO		{}, 
			_quadVAO	{};
};
