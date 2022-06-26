#pragma once

#include "../headers.h"
#include "stb_truetype.h"

class Font
{
public:
	stbtt_bakedchar cdata_[96]	{};
	uint32			tex_id_		{};
};
