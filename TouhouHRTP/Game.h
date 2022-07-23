#pragma once

#include "Render/RenderWindow.h"
#include "Objects/Player.h"
#include "Objects/PlayerBullet.h"
#include "Objects/Ball.h"
#include "Objects/Bonus.h"
#include "Level_manager.h"
#include "Image/Text.h"
#include "Menu.h"
#include <irrKlang/irrKlang.h>
#include <filesystem>
#include <functional>
#include <random>

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
	struct Save
	{
		uint32		hi_score	{};
		uint32		score		{};
		uint32		hi_mult		{1};
		uint32		player_lives{};
		uint32		player_bombs{};
		uint32		stage		{1};
		std::string difficulty	{};
	} save_;

	// ==Options==
	// Screen resolution:
	// 1 - 640x400
	// 1.25 - 800x500
	// 1.5 - 960x600
	// 2 - 1280x800
	// 2.25 - 1440x900
	// 2.5 - 1600x1000
	// and so on...
	//
	// Volume
	// 0.0 - 1.0
	//
	// Difficulty
	// Easy, Normal, Hard, Lunatic
	//
	// Options in file
	// All from above
	// Ball gravity
	// Energy loss on bounce
	// Player speed
	std::unordered_map<std::string, std::string> configs_;

	std::array<std::string, 4> difficulties_ {"EASY", "NORMAL", "HARD", "LUNATIC"};

	Menu::States		state_		{Menu::MAIN_MENU};
	std::random_device	rd_;

	GLFWwindow*			window_		{};
	RenderWindow*		render_		{};

	irrklang::ISoundEngine* se_		{};
	std::array<irrklang::ISoundSource*, 8> bgm_ {};
	std::array<irrklang::ISoundSource*, 8> sfx_ {};

	Font*	  			font20_		{};
	Text*				text_		{};
	std::unordered_map<uint32*, glm::vec2> text_pos_;
	glm::vec2			diffic_pos_	{};
	glm::vec2			time_pos_	{};

	float32				bullet_flurry_timer_	{0.0f};

	bool				timeout_	{false};
	float32				time_		{1000.0f};
	uint32				stage_		{1};
	uint32				score_		{};
	uint32				hi_score_	{};
	uint32				level_score_{};
	uint32				mult_		{1};
	uint32				hi_mult_	{1};
	uint32				tile_val_	{100};
	int8				point_lim_	{3};
	uint16				point_c_	{};
	float32				point_timer_{};

	Player*				reimu_	{};
	Ball*				ball_	{};
	Sprite*				inter_	{};
	Sprite*				bomb_	{};
	Sprite*				hp_		{};

	uint8							bg_temp_		{};
	Level_manager::Level_objects	lvl_objs_		{};
	std::vector<std::string>		lvls_list_		{};
	std::vector<PlayerBullet*>		player_bullets_	{};
	std::vector<EnemyBullet*>		enemy_bullets_	{};
	std::vector<Bonus*>				bonuses_		{};

	Menu*				menu_		{};
	float32				upper_bound_{};

	bool				shoot_flip_	{false};
	bool				ball_upd_f_	{false};
	bool				stage_clear_{false};
	bool				starting_f_	{false};

	// screen sizes
	float32			scr_res_mult_{};
	int32			win_width_	{};
	int32			win_height_ {};

	const uint32	scr_width_	{640};
	const uint32	scr_height_ {400};

	float32			k_			{};

	const char*		title_		{"TOUHOU"};

	// frametime vars
	float32 max_period_		{};
	float32	delta_time_		{};
	float32	last_frame_		{};

	// flags
	bool	polygon_flag_	{true};
	bool	runtime_flag_	{true};

	// keys map
	std::unordered_map<uint16, std::array<bool, 2>> keys_ {};
	
	std::function<uint32(uint32)> score_calc_ {};
	void write_save			();
	void read_save			();
	void write_config		();
	void read_config		();

	void set_difficulty		();
	void reset				();
	void clear_stage		();

	void update_enemies		();
	void update_ball		();
	void update_player		();
	void update_tiles		();

	void draw_text			();

	void update_gameplay	();
	void draw_gameplay		();

	void init_window		();
	void init_text			();
	void init_textures		();
	void init_objects		();

	static void framebuffer_size_callback(GLFWwindow*, int32, int32);
	static void window_size_callback(GLFWwindow*, int32, int32);
};

