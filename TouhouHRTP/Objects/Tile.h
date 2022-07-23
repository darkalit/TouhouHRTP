#pragma once
#include "Object.h"

class Tile : public Object 
{
public:
	Tile	(const uint32& screen_width, const uint32& screen_height, const uint8& hp, const uint8& skin);
	~Tile	() override {}

	uint8	hp			{};
	bool	play_s_f	{true};

	auto flip			()	-> bool;
	auto get_starting	()	-> bool;
	auto get_hp_change	()	-> bool;

	void reset		();
	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	bool	start_f_		{true};
	bool	flip_f_			{false};
	bool	hp_change_		{false};
	uint8	skin_			{};
	uint8	temp_hp_		{};
	uint8	initial_hp_		{};
};

