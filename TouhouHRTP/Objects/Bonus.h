#pragma once
#include "Object.h"
class Bonus : public Object
{
public:
	Bonus(const uint8& var, const uint32& screen_width, const uint32& screen_height);
	~Bonus() override = default;

	bool jump_f		{false};

	auto is_bomb	() -> bool;
	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	bool bomb_ex_	{false};
};

