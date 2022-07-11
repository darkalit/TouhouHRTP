#pragma once
#include "Object.h"
#include "Rectangle.h"

class Player : public Object
{
public:
	enum State
	{
		STAND,
		RUN_R,		RUN_L,
		SLIDE_R,	SLIDE_L,
		SLIDE_R_A2,	SLIDE_L_A2,
		SLIDE_R_A3,	SLIDE_L_A3,
		ATTACK1,
		ATTACK2_R,	ATTACK2_L,
		BOMB,
		DEAD
	};

	Player	(const uint32& screen_width, const uint32& screen_height);
	~Player	() override;

	void stand			();
	void run			(const float32& delta_time, const int8& direction);
	void attack1		(const float32& delta_time);
	void attack2		(const float32& delta_time, const int8& direction, const bool& no_stop);
	void attack3		(const float32& delta_time, const int8& direction);
	void bomb			(const float32& delta_time);
	void slide			(const float32& delta_time, const int8& direction);
	void dead			(const float32& delta_time);
	auto get_hp			()							-> uint32;
	auto get_bombs		()							-> uint32;
	auto get_state		()							-> State;
	auto get_invis		()							-> bool;
	void set_state		(const State& state);
	void set_old_state	(const State& state);
	
	void update_input	(const float32& delta_time);
	void update			(const float32& delta_time)	override;
	void draw			()							override;

	
private:
	bool		flag_			{true};
	bool		invis_			{false};
	uint32		hp_				{4};
	uint32		bombs_			{1};
	float32		speed_			{270.0f};
	float32		anim_time_		{0.0f};
	float32		invis_time_		{0.0f};
	int32		state_			{STAND};
	int32		new_state_		{STAND};

	std::map<std::string, Rectangle*>	effects_ {};

	void init_textures() override;
};

