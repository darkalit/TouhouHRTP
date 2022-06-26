#pragma once

#include "../headers.h"
#include "stb_truetype.h"

class Font
{
public:
	Font(const char* file_path);

	stbtt_bakedchar cdata_[96]	{};
	uint32			tex_id_		{};
};
