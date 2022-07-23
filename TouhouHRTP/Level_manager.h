#pragma once

#include "Objects/Tile.h"
#include "Objects/Bumper.h"
#include "Objects/Ball_tp.h"
#include "Objects/Rail.h"
#include "Objects/Turret.h"

class Level_manager
{
public:
	struct Level_objects
	{
		Sprite*					bg			{};
		std::vector<Tile*>		tiles		{};
		std::vector<Bumper*>	bumpers		{};
		std::vector<Rail*>		rails		{};
		std::vector<Ball_tp*>	ball_tps	{};
		std::vector<Turret*>	turrets		{};
		float32					tile_size	{};
		uint32					scr_width	{};
		uint32					scr_height	{};
		uint8					bg_num		{};
	};

	static void load	(Level_objects& arg, const char* file);
	static void draw	(Level_objects& arg);
	static void clear	(Level_objects& arg);

private:
	enum class Object_types
	{
		BACKGROUND,
		TILES,
		BUMPERS,
		RAILS,
		TELEPORTERS,
		TURRETS
	};

	static std::unordered_map<std::string, Object_types> transcriber_;
	static uint32 tile_size_;

	static void get_tile_params(const std::string& object_str, std::vector<uint8>& tile_params);
};

