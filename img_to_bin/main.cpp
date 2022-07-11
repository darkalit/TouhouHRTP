#include <iostream>
#include <fstream>

#define STBI_NO_PSD
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void print_usage()
{
	std::cout <<	"Syntax: img_to_bin.exe [-f | -h] [PATH...]\n"
					"Options:\n"
					"\t-f\tconvert certain file\n"
					"\t-h\tdisplay this help\n"
					"PATH - path to file\n"
					"Possible extensions: PNG, JPEG, TGA, BMP" << std::endl;
}

void write_bin(const std::string& file)
{
	std::cout << file << std::endl;
	int32_t width, height, nrChannels;
	unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		std::size_t found1 = file.find_last_of('.');
		std::size_t found2 = file.find_last_of('\\');
		if (found2 == std::string::npos)
			found2 = file.find_last_of('/');

		std::ofstream ofile(file.substr(found2 != std::string::npos ? found2 + 1 : 0, file.size() - found1 + 1) + ".dat", std::ios::binary);
		ofile.write(reinterpret_cast<char*>(&width), sizeof(width));
		ofile.write(reinterpret_cast<char*>(&height), sizeof(height));
		ofile.write(reinterpret_cast<char*>(&nrChannels), sizeof(nrChannels));
		ofile.write(reinterpret_cast<char*>(data), width * height * nrChannels);
		ofile.close();
	}
	else
		std::cout << "bad/no file - " << file << std::endl;

	stbi_image_free(data);
}

int main(int argc, char* argv[])
{
	if (argv[1] == nullptr)
	{
		print_usage();

		return 0;
	}
	if (!strcmp(argv[1], "-f"))
	{
		if (argv[2] == nullptr)
			print_usage();
		else
			write_bin(argv[2]);

		return 0;
	}

	if (!strcmp(argv[1], "-h"))
	{
		print_usage();

		return 0;
	}		

	return 0;
}