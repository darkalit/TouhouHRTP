#pragma once

//#include <custom/headers.h>
#include "../Utility.h"
#include "../Debug.h"
#include "Shader.h"
#include "../Image/Texture.h"


class RenderWindow
{
public:
	RenderWindow	(const Shader& shader, const int32& width, const int32& height);
	~RenderWindow	();

	//void init		(const Shader& shader, const int32& width, const int32& height);
	void clear		();
	void display	();

	Shader		shader;
	Texture		texture;
	glm::ivec2	size;
private:
	void initRender	();
	uint32	_FBO	{}, 
			_MSFBO	{}, 
			_RBO	{}, 
			_quadVAO{};
};
