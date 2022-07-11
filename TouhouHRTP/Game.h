#pragma once

#include "Objects/Player.h"
#include "Objects/PlayerBullet.h"
#include "Objects/Ball.h"
#include "Level_manager.h"
#include "Image/Text.h"
#include <functional>
#include <any>
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
	//float32 temp_t_{-2.0f};
	//Rectangle*			flash_	{};
	std::random_device	rd_;

	GLFWwindow*			window_	{};
	RenderWindow*		render_	{};

	Font*	  			font16_		{};
	Text*				text_		{};

	float32				upper_bound_{};
	std::unordered_map<uint32*, glm::vec2> text_pos_;

	float32				bullet_flurry_timer_	{0.0f};

	bool				timeout_	{false};
	float32				time_		{1000.0f};
	uint32				stage_		{1};
	uint32				score_		{};
	uint32				hi_score_	{1000};
	uint32				mult_		{1};
	uint32				hi_mult_	{10};

	glm::vec2			diffic_pos_	{};
	glm::vec2			time_pos_	{};

	//uint32				player_hp_		{4};
	//uint32				player_bombs_	{1};

	Player*				reimu_	{};
	Ball*				ball_	{};
	Sprite*				inter_	{};
	Sprite*				bomb_	{};
	Sprite*				hp_		{};

	Level_manager::Level_objects	lvl_objs_		{};
	std::vector<PlayerBullet*>		player_bullets_	{};
	std::vector<EnemyBullet*>		enemy_bullets_	{};

	// screen sizes
	const int32		win_width_	{1440}; //{1440};//{800};
	const int32		win_height_ {900};	//{900};//{500};

	const uint32	scr_width_	{640};
	const uint32	scr_height_ {400};

	const float32	k_width_	{static_cast<float32>(this->win_width_) / static_cast<float32>(this->scr_width_)};
	const float32	k_height_	{static_cast<float32>(this->win_height_) / static_cast<float32>(this->scr_height_)};
	const char*		title_		{"TOUHOU"};

	// frametime vars
	float32	delta_time_		{};

	float32	last_frame_		{};
	float32	last_time_		{};
	int32	frame_count_	{};

	float32 pixel_size_		{1.0f};

	bool	polygon_flag_	{true};
	bool	pause_			{false};
	bool	runtime_flag_	{true};
	std::unordered_map<uint16, std::array<bool, 2>> keys_ {};

	void update_enemies		();
	void update_ball		();
	void update_player		();
	void update_tiles		();
	void draw_text			();

	void init_window		();
	void init_text			();
	void init_textures		();
	void init_objects		();

	static void framebuffer_size_callback(GLFWwindow*, int32, int32);
	static void keyCallback(GLFWwindow*, int, int, int, int);
};

