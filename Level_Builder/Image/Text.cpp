#include "Text.h"

//#define STB_TRUETYPE_IMPLEMENTATION

#include "stb_truetype.h"
#include "../Debug.h"

Text::Text(Font* font, const uint32& screen_width, const uint32& screen_height)
	:
	font_(font)
{
	this->win_height_ = static_cast<float32>(screen_height);
	this->proj_	= glm::ortho(0.0f, 
		static_cast<float32>(screen_width), 
		static_cast<float32>(screen_height), 
		0.0f, -1.0f, 1.0f);
	Resources::get_shader("text")->use();
	Resources::get_shader("text")->setMat4("projection", this->proj_);
	Resources::get_shader("text")->setVec3("textColor", glm::vec3(1.0f, 1.0f, 1.0f));
	
	glGenVertexArrays(1, &this->vao_);
    glGenBuffers(1, &this->vbo_);
    glBindVertexArray(this->vao_);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Text::set_color(float32 r, float32 g, float32 b)
{
	Resources::get_shader("text")->use();
	Resources::get_shader("text")->setVec3("textColor", glm::vec3(r, g, b));
}

void Text::set_pos(const float32& x, const float32& y)
{
	//this->pos_ = glm::vec2(x, this->win_height_ - y);
	this->pos_ = glm::vec2(x, y);
}

void Text::set_text(const std::string& text)
{
	this->text_ = text;
	this->init_r_pos();
}

void Text::set_font(Font* font)
{
	this->font_ = font;
}

void Text::draw(const bool& right_align)
{
	Resources::get_shader("text")->use();

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->vao_);

	glm::vec2 temp = this->pos_;
	for (const auto& ch : this->text_)
		if (ch >= 32)
		{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(this->font_->cdata_, 512,512, ch-32, 
							&this->pos_.x, &this->pos_.y, &q, 1);

			//float32 quadV[6][4];

			//if (!right_align)
			float32 quadV[6][4] = 
			{
				{q.x1 - this->r_pos_ * static_cast<float32>(right_align), q.y0,		q.s1, q.t0},
				{q.x0 - this->r_pos_ * static_cast<float32>(right_align), q.y1,		q.s0, q.t1},
				{q.x0 - this->r_pos_ * static_cast<float32>(right_align), q.y0,		q.s0, q.t0},

				{q.x0 - this->r_pos_ * static_cast<float32>(right_align), q.y1,		q.s0, q.t1},
				{q.x1 - this->r_pos_ * static_cast<float32>(right_align), q.y0,		q.s1, q.t0},
				{q.x1 - this->r_pos_ * static_cast<float32>(right_align), q.y1,		q.s1, q.t1},
	        };
			glBindTexture(GL_TEXTURE_2D, this->font_->tex_id_);
			
	        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
	        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof quadV, &quadV);

	        glBindBuffer(GL_ARRAY_BUFFER, 0);
	        glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	this->pos_ = temp;
	glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::init_r_pos()
{
	glm::vec2 temp (0.0f);
	for (const auto& ch : this->text_)
		if (ch >= 32)
		{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(this->font_->cdata_, 512,512, ch-32, 
							&temp.x, &temp.y, &q, 1);
		}
	this->r_pos_ = temp.x;
}
