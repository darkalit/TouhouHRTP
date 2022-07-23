#pragma once

#include "Image/Text.h"

class Menu
{
public:
	enum States
	{
		MAIN_MENU,
		GAMEPLAY,
		PAUSE,
		GAME_OVER,
		LVL_SWITCH,
		ENDING,
		OPTIONS
	};

	Menu(const uint32& screen_width, const uint32& screen_height, const float32& k);
	~Menu();

	uint8		option			{};

	void draw_main_menu();
	void draw_pause_menu();
	void draw_options_menu(
		const std::string& res,
		const std::string& vol,
		const std::string& diff);
	void draw_game_over();
	void draw_lvl_switch(
		const uint32& p1, 
		const uint32& p2, 
		const uint32& p3, 
		const uint32& p4,
		uint32& sum,
		const uint32& stage);
	void draw_ending(
		const std::string& diff,
		const uint32& hi_score,
		const uint32& score);
	void move_up();
	void move_down();

private:
	float32		k_				{};
	uint32		screen_width_	{}, 
				screen_height_	{};

	Font*		font28_			{};
	Text*		text_			{};
	Sprite*		main_menu_		{};
	Sprite*		pause_menu_		{};
	Sprite*		game_over_		{};
	Sprite*		lvl_switch_		{};
	Sprite*		ending_			{};
	Sprite*		point_			{};
};

