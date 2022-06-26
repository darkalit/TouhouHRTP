#include "Font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_truetype.h"

Font::Font(const char* file_path)
{
	auto* ttf_buffer = new uint8[1 << 20];
	auto* temp_bitmap = new uint8[512 * 512];
	FILE* buffer;
	fopen_s(&buffer, file_path, "rb");

	if (buffer)
	{
		fread_s(ttf_buffer, 1<<20, 1, 1<<20, buffer);
		fclose(buffer);
	}
	else
		std::cout << "ERROR::TRUETYPE: Failed to open font file" << std::endl;
		
	stbtt_BakeFontBitmap(ttf_buffer, 0, 72.0, temp_bitmap, 
		512, 512, 32, 96, this->cdata_);
	
	// Testing bitmap creation
	// stbi_write_png("image.png", 512, 512, 1, temp_bitmap, 512 * 1);

	glGenTextures(1, &this->tex_id_);
	glBindTexture(GL_TEXTURE_2D, this->tex_id_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512,512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	delete[] ttf_buffer;
	delete[] temp_bitmap;
}
