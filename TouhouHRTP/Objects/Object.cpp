#include "Object.h"

Object::Object()
= default;

Object::~Object()
= default;

void Object::set_pos		(const float32& x, const float32& y)
{
	this->pos_ = glm::vec2(x, y);
}

void Object::move			(const float32& x, const float32& y)
{
	this->set_pos(this->get_pos().x + x, this->get_pos().y + y);
}

void Object::set_rot		(const float32& angle)
{
	this->angle_ = angle;
}

void Object::set_scale	(const glm::vec2& scale)
{
	this->scale_ = scale;
}

auto Object::get_pos		() -> glm::vec2
{
	return this->pos_;
}

auto Object::get_angle() -> float32
{
	return this->angle_;
}

auto Object::get_edges	() -> glm::ivec4
{
	return this->temp_->getEdges();
}

auto Object::get_bounds	() -> Rect
{
	this->bounds_.bottomLeft	= glm::vec2(pos_.x - static_cast<float32>(this->get_edges().z - this->get_edges().x) / 2.0f,
											pos_.y - static_cast<float32>(this->get_edges().w - this->get_edges().y) / 2.0f);
	this->bounds_.topLeft		= glm::vec2(pos_.x - static_cast<float32>(this->get_edges().z - this->get_edges().x) / 2.0f,
											pos_.y + static_cast<float32>(this->get_edges().w - this->get_edges().y) / 2.0f);
	this->bounds_.topRight		= glm::vec2(pos_.x + static_cast<float32>(this->get_edges().z - this->get_edges().x) / 2.0f,
											pos_.y + static_cast<float32>(this->get_edges().w - this->get_edges().y) / 2.0f);
	this->bounds_.bottomRight	= glm::vec2(pos_.x + static_cast<float32>(this->get_edges().z - this->get_edges().x) / 2.0f,
											pos_.y - static_cast<float32>(this->get_edges().w - this->get_edges().y) / 2.0f);
	return this->bounds_;
}

auto Object::get_size	() -> glm::vec2
{
	return this->temp_->getSize();
}

bool Object::check_intersect(Object& object)
{
	Rect boundsl = this->get_bounds();
	Rect boundsr = object.get_bounds();
	if (boundsl.topLeft.x > boundsr.bottomRight.x ||
		boundsl.topLeft.y < boundsr.bottomRight.y ||
		boundsl.bottomRight.x < boundsr.topLeft.x ||
		boundsl.bottomRight.y > boundsr.topLeft.y)
		return false;
	return true;
}

auto Object::check_intersect_ell(Object& object) -> bool
{
	return false;
}

auto Object::check_intersect_circle_rect(Object& object) -> bool
{
	//auto objBounds = object.get_bounds();
	//float32 closestX = glm::clamp(this->get_pos().x, objBounds.topLeft.x, objBounds.topRight.x);
	//float32 closestY = glm::clamp(this->get_pos().y, objBounds.topLeft.y, objBounds.bottomLeft.y);
	//
	//float32 distX = this->get_pos().x - closestX;
	//float32 distY = this->get_pos().y - closestY;
	//
	//float32 distSqr = (distX * distX) + (distY * distY);
	//
	//return distSqr < ((this->get_size().x / 2.0f) * (this->get_size().x / 2.0f));

	glm::vec2 cDist;
	cDist.x = fabs(this->get_pos().x - object.get_pos().x);
	cDist.y = fabs(this->get_pos().y - object.get_pos().y);

	if (cDist.x > (object.get_size().x / 2.0f + this->get_size().x / 2.0f))
		return false;
	if (cDist.y > (object.get_size().y / 2.0f + this->get_size().x / 2.0f))
		return false;

	if (cDist.x <= (object.get_size().x / 2.0f))
		return true;
	if (cDist.y <= (object.get_size().y / 2.0f))
		return true;

	float32 cornDist =	powf(cDist.x - object.get_size().x / 2.0f, 2)
						+ powf(cDist.y - object.get_size().y / 2.0f, 2);
	return cornDist <= powf(object.get_size().x / 2.0f, 2);
}

auto Object::check_intersect_circle_circle(Object& object) -> bool
{
	return	powf(object.get_pos().x - this->get_pos().x, 2)
			+ powf(object.get_pos().y - this->get_pos().y, 2)
			<= powf((object.get_size().x + this->get_size().x) / 2.0f, 2);
}
