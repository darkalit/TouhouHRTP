#pragma once

#include "../headers.h"

class Texture
{
public:
	uint32	ID{};
	int32	width, 
			height;
	int32	internalFormat, 
			imageFormat;
	int32	wrapS, 
			wrapT;
	int32	filterMin, 
			filterMax;

	Texture();
	Texture(const std::string& filepath, const bool& binary = true);
	void generate		(const int32& width, const int32& height, const unsigned char* data);
	void texFromImage	(const std::string& filepath);
	void texFromBin		(const std::string& filepath);
	void bind			() const;
};
