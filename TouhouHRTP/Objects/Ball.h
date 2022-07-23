#pragma once
#include "Object.h"

class Ball : public Object
{
public:
	Ball			(const uint32& screen_width, const uint32& screen_height);
	~Ball			() override = default;
	
	void fall		(const float32& delta_time);
	auto get_vel	()							->glm::vec2;
	void set_vel	(const float32& x_vel, const float32& y_vel);
	void set_upper_bound (const float32& upper_bound);
	void set_gravity(const float32& gravity);
	void set_e_loss	(const float32& e_loss);
	void reflectx	();
	void reflecty	();

	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	float32	upper_b_	{0.0f},
			gravity_	{-900.0f}, 
			y_vel_		{0.0f}, 
			x_vel_		{10.0f}, 
			e_loss_		{0.8f};
};

