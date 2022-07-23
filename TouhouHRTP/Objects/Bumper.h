#pragma once
#include "Object.h"
class Bumper : public Object
{
public:
	Bumper	(const uint32& screen_width, const uint32& screen_height);
	~Bumper	() override {}

	void update		(const float32& delta_time)	override {}
	void draw		()							override;

private:
};

