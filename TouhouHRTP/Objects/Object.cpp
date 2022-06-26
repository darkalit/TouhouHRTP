#include "Object.h"

Object::Object()
= default;

Object::~Object()
= default;

void Object::set_pos		(const float32& x, const float32& y)
{
	this->pos_ = glm::vec2(x, y);
}

void Object::move		(const float32& x, const float32& y)
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
	if (this->get_bounds().topLeft.x > object.get_bounds().bottomRight.x ||
		this->get_bounds().topLeft.y < object.get_bounds().bottomRight.y ||
		this->get_bounds().bottomRight.x < object.get_bounds().topLeft.x ||
		this->get_bounds().bottomRight.y > object.get_bounds().topLeft.y)
		return false;
	else
		return true;
}
