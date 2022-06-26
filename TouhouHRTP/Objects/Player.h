#pragma once
#include "Object.h"

class Player : public Object
{
public:
	Player	(GLFWwindow* window, const uint32& screen_width, const uint32& screen_height);
	~Player	() override;

	auto stand			()							-> bool;
	auto run			(const float32& delta_time, const int8& direction)	-> bool;
	auto attack1		(const float32& delta_time)	-> bool;
	auto attack2		(const float32& delta_time)	-> bool;
	auto slide			(const float32& delta_time, const int8& direction)	-> bool;
	auto dead			(const float32& delta_time)	-> bool;
	
	void update_input	(const float32& delta_time);//(GLFWwindow* window, int key, int scancode, int action, int mods);
	void update			(const float32& delta_time)	override;
	void draw			()							override;
	
	bool		anim_flag_ls_	{false};
	bool		anim_flag_rs_	{false};

private:
	GLFWwindow* window_;
	float		anim_time_		{0.0f};

	void init_textures() override;
};

