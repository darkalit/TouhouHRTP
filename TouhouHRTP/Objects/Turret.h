#pragma once
#include "Object.h"
#include "EnemyBullet.h"

class Turret : public Object
{
public:
	Turret	(const uint32& screen_width, const uint32& screen_height, const uint8& bullets_count);
	~Turret	() override {}

	void set_delay	(const float32& delay);
	void set_bullets_count (const uint8& count);
	void shoot		(const glm::vec2& target, std::vector<EnemyBullet*>& bullets);

	void reset		();
	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	float32 sh_delay_		{2.0f};
	float32 anim_time_		{0.0f};
	bool	shoot_f_		{false};
	uint8	bullets_count_	{2};
};

