#pragma once
#include "Object.h"

class Player : public Object
{
public:
	enum State
	{
		STAND,
		RUNNING,
		SLIDING_R,
		SLIDING_L,
		DYING,
		DEAD,
		ATTACKING
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
	
	void update_input	(const float32& delta_time);
	void update			(const float32& delta_time)	override;
	void draw			()							override;
	
	bool		anim_flag_ls_	{false};
	bool		anim_flag_rs_	{false};

private:
	float		anim_time_		{0.0f};
	int			state_			{STAND};

	void init_textures() override;
};

