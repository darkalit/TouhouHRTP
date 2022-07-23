#pragma once
#include "Object.h"
class Rail : public Object
{
public:
	Rail	(const uint32& screen_width, const uint32& screen_height);
	~Rail	() override {}

	void set_rot	(const bool& vert);
	auto get_rot	() -> bool;
	void update		(const float32& delta_time)	override {}
	void draw		()							override;

private:
	bool vertical_f_{false};
};

