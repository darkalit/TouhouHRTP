#pragma once
#include "Object.h"

class Tile : public Object 
{
public:
	enum class State
	{
		DEAD,
		ALIVE
	};

	Tile	(const uint32& screen_width, const uint32& screen_height, const uint8& phase);
	~Tile	() override;

	auto flip		()	-> bool;
	auto get_flip	()	-> bool;
	auto get_state	()	-> State;

	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	bool flip_f_	{false};
	State state_	{State::ALIVE};

	void init_textures() override;
};

