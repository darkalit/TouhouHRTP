#pragma once
#include "Object.h"
class PlayerBullet : public Object
{
public:
	PlayerBullet(Texture* texture, const glm::vec2& pos, const uint32& screen_width, const uint32& screen_height);
	~PlayerBullet() override;

	void update(const float32& deltaTime) override;
	void draw(Shader* shader) override;

private:
	void init_textures() override;
};

