#pragma once
#include "Object.h"
class Ball_tp : public Object
{
public:
	Ball_tp		(const uint32& screen_width, const uint32& screen_height);
	~Ball_tp	() override {}

	auto tp_ready	() -> bool;
	void teleport	(Object& object, Ball_tp& ball_tp);
	void update		(const float32& delta_time)	override;
	void draw		()							override;


private:
	bool tp_flag_ {true};
	float32 anim_time_ {0.0f};
};

