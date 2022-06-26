#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() :
	width			(0),			height		(0), 
	internalFormat	(GL_RGB),		imageFormat	(GL_RGB), 
	wrapS			(GL_REPEAT),	wrapT		(GL_REPEAT), 
	filterMin		(GL_LINEAR),	filterMax	(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}

void Texture::generate(const int32& width, const int32& height, const unsigned char* data)
{
	this->width = width;
	this->height = height;


	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, this->width, this->height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texFromImage(const std::string& filepath)
{
	int32 nrChannels;

	unsigned char* data = stbi_load(filepath.c_str(), &this->width, &this->height, &nrChannels, 0);
	if (data)
	{
		GLenum format = 0;
		switch (nrChannels)
		{
		case 1:
			format = GL_RED;	break;
		case 2:
			format = GL_RG;		break;
		case 3:
			format = GL_RGB;	break;
		case 4:
			format = GL_RGBA;	break;
		default:				break;
		}

		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int32>(format), this->width, this->height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
		std::cout << "Texture failed to load at path:" << filepath << std::endl;
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}
