#pragma once

#include "../headers.h"
#include "stb_truetype.h"

class Font
{
public:
	Font(const char* file_path, const float32& size);
	
	stbtt_fontinfo	fontinfo_	{};
	stbtt_bakedchar cdata_[96]	{};
	uint32			tex_id_		{};
};
