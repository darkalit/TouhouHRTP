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
		DEAD
	};

	Player	(const uint32& screen_width, const uint32& screen_height);
	~Player	() override;

	auto stand			()							-> bool;
	auto run			(const float32& delta_time, const int8& direction)	-> bool;
	auto attack1		(const float32& delta_time)	-> bool;
	auto attack2		(const float32& delta_time, const int8& direction, const bool& no_stop)	-> bool;
	auto attack3		(const float32& delta_time, const int8& direction)	-> bool;
	auto slide			(const float32& delta_time, const int8& direction)	-> bool;
	auto dead			(const float32& delta_time)	-> bool;
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
	float32		speed_			{270.0f};
	float32		anim_time_		{0.0f};
	float32		invis_time_		{0.0f};
	int32		state_			{STAND};
	int32		new_state_		{STAND};

	std::map<std::string, Rectangle*>	effects_ {};

	void init_textures() override;
};

