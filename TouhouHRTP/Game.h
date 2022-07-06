#pragma once

#include "Objects/Player.h"
#include "Objects/PlayerBullet.h"
#include "Objects/Ball.h"
#include "Objects/Tile.h"
#include "Objects/Rectangle.h"
#include "Image/Text.h"
#include <functional>

class Game
{
public:
	Game	();
	~Game	();

	auto is_running		() -> bool;
	void clear			();
	void process_input	();
	void update			();
	void display		();

private:
	//float32 temp_t_{-2.0f};
	//Rectangle*			flash_	{};

	GLFWwindow*			window_	{};
	RenderWindow*		render_	{};

	Font*	  			font_	{};
	Text*				text_	{};
	Player*				reimu_	{};
	Ball*				ball_	{};
	std::vector<Tile*>	tiles_	{};
	std::vector<PlayerBullet*>	player_bullets_	{};

	// screen sizes
	const int32		win_width_	{1440};//{1440};//{800};
	const int32		win_height_ {900};//{900};//{500};

	const uint32	scr_width_	{640};
	const uint32	scr_height_ {400};

	const char*		title_		{"TOUHOU"};

	// frametime vars
	float32	delta_time_		{};

	float32	last_frame_		{};
	float32	last_time_		{};
	int32	frame_count_	{};

	float32 pixel_size_		{1.0f};

	bool	polygon_flag_	{true};
	bool	runtime_flag_	{true};
	std::map<uint16, std::array<bool, 2>> keys_ {};

	void update_ball		();
	void update_player		();
	void update_tiles		();

	void init_window		();
	void init_textures		();
	void init_objects		();

	static void framebuffer_size_callback(GLFWwindow*, int32, int32);
	static void keyCallback(GLFWwindow*, int, int, int, int);
};

