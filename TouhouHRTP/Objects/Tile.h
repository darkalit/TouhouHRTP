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

	Tile	(Texture* texture, const uint32& screen_width, const uint32& screen_height, const State& state);
	~Tile	() override;

	auto flip		()	-> bool;
	auto get_flip	()	-> bool;
	auto get_state	()	-> State;

	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	bool flip_f_	{false};
	uint8 state_	{};

	void init_textures() override;
};

