#pragma once
#include "Object.h"
class PlayerBullet : public Object
{
public:
	PlayerBullet(const glm::vec2& pos, const uint32& screen_width, const uint32& screen_height);
	~PlayerBullet() override {}

	Sprite* get_temp ();
	void update	(const float32& deltaTime)	override;
	void draw	()							override;

private:
};

