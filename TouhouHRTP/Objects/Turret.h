#pragma once
#include "Object.h"
#include "EnemyBullet.h"

class Turret : public Object
{
public:
	Turret	(const uint32& screen_width, const uint32& screen_height);
	~Turret	() override;

	void set_bullets_count (const int32& count);
	void shoot		(const glm::vec2& target, std::vector<EnemyBullet*>& bullets);
	void update		(const float32& delta_time)	override;
	void draw		()							override;

private:
	float32 anim_time_	{0.0f};
	bool	shoot_f_	{false};
	int32	bullets_count_ {2};
	void init_textures() override;
};

