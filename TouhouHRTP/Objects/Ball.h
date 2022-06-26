#pragma once
#include "Object.h"

class Ball : public Object
{
public:
	Ball			(const uint32& screen_width, const uint32& screen_height);
	~Ball			() override;

	auto fall		(const float32& delta_time)	-> bool;
	auto get_vel	()							->glm::vec2;
	void set_vel	(const float32& x_vel, const float32& y_vel);

	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	float32 gravity_	{-900.0f}, 
			y_vel_		{0.0f}, 
			x_vel_		{10.0f}, 
			e_loss_		{0.8f};
	void init_textures()						override;
};

