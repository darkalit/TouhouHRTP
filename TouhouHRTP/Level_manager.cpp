#include "Level_manager.h"
#include "Level_manager.h"

std::unordered_map<std::string, Level_manager::Object_types> Level_manager::transcriber_
{
	{"BACKGROUND",	Level_manager::Object_types::BACKGROUND},
	{"TILES",			Level_manager::Object_types::TILES},
	{"BUMPERS",		Level_manager::Object_types::BUMPERS},
	{"RAILS",			Level_manager::Object_types::RAILS},
	{"TELEPORTERS",	Level_manager::Object_types::TELEPORTERS},
	{"TURRETS",		Level_manager::Object_types::TURRETS}
};

void Level_manager::load(Level_objects& arg, const char* file)
{
	std::ifstream lvl_file;
	std::string lvl_str;
	bool read_m	{false};
	Object_types mode{};
	// Levels structured in a grid 20x8
	// 1 tile size (width=height, cause it all squares) defined in Level_objects.tile_size
	clear(arg);
	lvl_file.open(file, std::ios::in);
	while (std::getline(lvl_file, lvl_str))
	{
		if (const auto it = lvl_str.find("//"); it != std::string::npos)
			lvl_str.erase(it);
		lvl_str.erase(
			std::remove_if(lvl_str.begin(), lvl_str.end(), 
				isspace), lvl_str.end());
		lvl_str.erase(std::remove_if(lvl_str.begin(), lvl_str.end(), 
			[&read_m](uint8 x) -> bool { return read_m = (x == '[' || x == ']'); }), lvl_str.end());
		if (lvl_str.empty())
			continue;
		if (read_m)
		{
			mode = transcriber_.at(lvl_str);
			continue;	
		}

		std::vector<uint8> tile_params;
		get_tile_params(lvl_str, tile_params);

		switch (mode)
		{
		case Object_types::BACKGROUND:
			if (arg.bg != nullptr)
				delete arg.bg;
			tile_params[0] %= 7;
			arg.bg_num = tile_params[0];
			arg.bg = new Sprite(Resources::get_texture("bg" + std::to_string(tile_params[0])),
				glm::ivec4(0, 0, 
					Resources::get_texture("bg" + std::to_string(tile_params[0]))->width,
					Resources::get_texture("bg" + std::to_string(tile_params[0]))->height), 
				glm::ivec2(arg.scr_width, arg.scr_height), false);
			break;

		case Object_types::TILES:
			arg.tiles.push_back( new Tile(arg.scr_width, arg.scr_height, tile_params[2], glm::clamp<uint8>(tile_params[3] + 1, 1, 3)));
			arg.tiles.back()->set_pos(
				arg.tile_size * (static_cast<float32>(tile_params[0]) + 0.5f), 
				static_cast<float32>(arg.scr_height) - arg.tile_size * (static_cast<float32>(tile_params[1]) + 2.5f));
			break;

		case Object_types::BUMPERS:
			arg.bumpers.push_back( new Bumper(arg.scr_width, arg.scr_height));
			arg.bumpers.back()->set_pos(
				arg.tile_size * (static_cast<float32>(tile_params[0]) + 0.5f), 
				static_cast<float32>(arg.scr_height) - arg.tile_size * (static_cast<float32>(tile_params[1]) + 2.5f));
			break;

		case Object_types::RAILS:
			arg.rails.push_back( new Rail(arg.scr_width, arg.scr_height));
			arg.rails.back()->set_rot(tile_params[3]);
			arg.rails.back()->set_pos(
				arg.tile_size * (static_cast<float32>(tile_params[0]) + 0.5f) 
				+ static_cast<float32>(arg.rails.back()->get_rot()) * 0.5f
				* static_cast<float32>((tile_params[2] == 0) - (tile_params[2] == 2))
				* (arg.rails.back()->get_size().x - arg.tile_size),
				static_cast<float32>(arg.scr_height) -
				arg.tile_size * (static_cast<float32>(tile_params[1]) + 2.5f)
				+ static_cast<float32>(!arg.rails.back()->get_rot()) * 0.5f
				* static_cast<float32>((tile_params[2] == 2) - (tile_params[2] == 0))
				* (arg.tile_size - arg.rails.back()->get_size().y)
			);
			break;

		case Object_types::TELEPORTERS:
			arg.ball_tps.push_back( new Ball_tp(arg.scr_width, arg.scr_height));
			arg.ball_tps.back()->set_pos(
				arg.tile_size * (static_cast<float32>(tile_params[0]) + 0.5f), 
				static_cast<float32>(arg.scr_height) - arg.tile_size * (static_cast<float32>(tile_params[1]) + 2.5f));
			break;

		case Object_types::TURRETS:
			arg.turrets.push_back( new Turret(arg.scr_width, arg.scr_height, tile_params[2]));
			arg.turrets.back()->set_pos(
				arg.tile_size * (static_cast<float32>(tile_params[0]) + 0.5f),
				static_cast<float32>(arg.scr_height) - arg.tile_size * (static_cast<float32>(tile_params[1]) + 2.5f));
			break;
		}
		tile_params.clear();
	}
	lvl_file.close();
}

void Level_manager::draw(Level_objects& arg)
{
	arg.bg->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));

	for (auto tile : arg.tiles)
		tile->draw();

	for (auto bumper : arg.bumpers)
		bumper->draw();

	for (auto rail : arg.rails)
		rail->draw();

	for (auto ball_tp : arg.ball_tps)
		ball_tp->draw();

	for (auto turret : arg.turrets)
		turret->draw();
}

void Level_manager::clear(Level_objects& arg)
{
	for (auto tile : arg.tiles)
		delete tile;
	arg.tiles.clear();

	for (auto bumper : arg.bumpers)
		delete bumper;
	arg.bumpers.clear();

	for (auto rail : arg.rails)
		delete rail;
	arg.rails.clear();

	for (auto ball_tp : arg.ball_tps)
		delete ball_tp;
	arg.ball_tps.clear();

	for (auto turret : arg.turrets)
		delete turret;
	arg.turrets.clear();
}

void Level_manager::get_tile_params(const std::string& object_str, std::vector<uint8>& tile_params)
{
	std::istringstream lvl_stream(object_str);
	std::string tile_param_str;
	while (std::getline(lvl_stream, tile_param_str,','))
		tile_params.push_back(static_cast<uint8>(std::stoi(tile_param_str)));
}

