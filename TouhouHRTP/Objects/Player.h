#pragma once
#include "Object.h"

class Player : public Object
{
public:
	enum State
	{
		STAND,
		RUN_R,
		RUN_L,
		SLIDE_R,
		SLIDE_L,
		ATTACK1,
		DEAD
	};

	Player	(Texture* texture, const uint32& screen_width, const uint32& screen_height);
	~Player	() override;

	auto stand			()							-> bool;
	auto run			(const float32& delta_time, const int8& direction)	-> bool;
	auto attack1		(const float32& delta_time)	-> bool;
	auto attack2		(const float32& delta_time)	-> bool;
	auto slide			(const float32& delta_time, const int8& direction)	-> bool;
	auto dead			(const float32& delta_time)	-> bool;
	auto get_state		()							-> State;
	auto get_invis		()							-> bool;
	void set_state		(const State& state);
	
	void update_input	(const float32& delta_time);
	void update			(const float32& delta_time)	override;
	void draw			(Shader* shader)			override;

	
private:
	bool		flag_			{true};
	bool		invis_			{false};
	bool		invis_flag_		{false};
	float32		speed_			{270.0f};
	float32		anim_time_		{0.0f};
	float32		invis_time_		{0.0f};
	int32		state_			{STAND};
	int32		new_state_		{STAND};

	void init_textures() override;
};

