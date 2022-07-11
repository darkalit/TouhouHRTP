#pragma once
#include "Object.h"
class EnemyBullet : public Object
{
public:
	EnemyBullet		(const uint32& screen_width, const uint32& screen_height);
	~EnemyBullet	() override;

	void set_direction	(const glm::vec2& direction);
	void update			(const float32& delta_time)	override;
	void draw			()							override;

private:
	glm::vec2 direction_ {};
	void init_textures() override;
};

