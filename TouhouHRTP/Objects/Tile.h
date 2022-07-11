#pragma once
#include "Object.h"

class Tile : public Object 
{
public:
	enum State
	{
		DEAD,
		PHASE1,
		PHASE2,
		PHASE3
	};

	Tile	(const uint32& screen_width, const uint32& screen_height, const uint8& state);
	~Tile	() override;

	auto flip		()	-> bool;
	auto get_state	()	-> State;
	auto get_state_change	()	-> bool;

	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	bool flip_f_		{false};
	bool state_change_	{false};
	uint8 temp_state_	{};
	uint8 state_		{};

	void init_textures() override;
};

