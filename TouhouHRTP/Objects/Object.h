#pragma once
#include "../Image/Sprite.h"

class Object
{
public:
	Object();
	virtual ~Object();

	virtual void set_pos		(const float32& x, const float32& y);
	virtual void move			(const float32& x, const float32& y);
	virtual void set_rot		(const float32& angle);
	virtual void set_scale		(const glm::vec2& scale);

	virtual auto get_pos			() -> glm::vec2;
	virtual auto get_edges			() -> glm::ivec4;
	virtual auto get_bounds			() -> Rect;
	virtual auto get_size			() -> glm::vec2;
	virtual auto check_intersect	(Object& object) -> bool;

	virtual void update	(const float32& delta_time)	= 0;
	virtual void draw	()							= 0;

protected:
	std::map<std::string, std::vector<Sprite*>> sprites_ {};
	Sprite*		temp_	{};
	glm::vec2	pos_	{};
	glm::vec2	scale_	{};
	Rect		bounds_	{};
	float32		angle_	{}; 

	float32		time_			{};
	uint32		screen_width_	{}, 
				screen_height_	{};
	uint32		iter_			{};
	Texture		texture_		{};

	virtual void init_textures() = 0;
};

