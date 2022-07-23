#include "Menu.h"

Menu::Menu(const uint32& screen_width, const uint32& screen_height, const float32& k)
{
	this->k_ = k;
	this->screen_width_ = screen_width;
	this->screen_height_ = screen_height;
	this->font28_	= new Font("Fonts/Heuristica-Bold.ttf", 28.0f * this->k_);
	this->text_		= new Text	(this->font28_, 
		static_cast<uint32>(static_cast<float32>(this->screen_width_) * this->k_),
		static_cast<uint32>(static_cast<float32>(this->screen_height_) * this->k_));
	this->text_->set_color(0.76f, 0.0f, 0.8f);

	this->main_menu_ = new Sprite(Resources::get_texture("main_menu"), 
		glm::ivec4(0, 0, 640, 400),
		glm::ivec2(this->screen_width_, this->screen_height_), false);
	this->pause_menu_ = new Sprite(Resources::get_texture("pause_menu"), 
		glm::ivec4(0, 0, 640, 400),
		glm::ivec2(this->screen_width_, this->screen_height_), false);
	this->game_over_ = new Sprite(Resources::get_texture("game_over"),
		glm::ivec4(0, 0, 640, 400),
		glm::ivec2(this->screen_width_, this->screen_height_), false);
	this->lvl_switch_ = new Sprite(Resources::get_texture("level_switch"),
		glm::ivec4(0, 0, 640, 400),
		glm::ivec2(this->screen_width_, this->screen_height_), false);
	this->ending_ = new Sprite(Resources::get_texture("ending"),
		glm::ivec4(0, 0, 640, 400),
		glm::ivec2(this->screen_width_, this->screen_height_), false);
	this->point_ = new Sprite(Resources::get_texture("tiles"), 
		glm::ivec4(113, 17, 127, 31),
		glm::ivec2(this->screen_width_, this->screen_height_), false);
}

Menu::~Menu()
{
	delete this->text_;
	delete this->font28_;
	delete this->main_menu_;
	delete this->pause_menu_;
	delete this->game_over_;
	delete this->lvl_switch_;
	delete this->ending_;
	delete this->point_;
}

void Menu::draw_main_menu()
{
	this->option %= 4;
	this->main_menu_->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));

	this->text_->set_text("Start");
	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 0) + 1.0f * static_cast<float32>(this->option == 0), 
		0.0f * static_cast<float32>(this->option != 0) + 1.0f * static_cast<float32>(this->option == 0), 
		0.8f * static_cast<float32>(this->option != 0) + 1.0f * static_cast<float32>(this->option == 0));
	this->text_->set_pos(310.0f * this->k_ - this->text_->r_pos_ / 2.0f, 285.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text("Continue");
	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 1) + 1.0f * static_cast<float32>(this->option == 1), 
		0.0f * static_cast<float32>(this->option != 1) + 1.0f * static_cast<float32>(this->option == 1), 
		0.8f * static_cast<float32>(this->option != 1) + 1.0f * static_cast<float32>(this->option == 1));
	this->text_->set_pos(310.0f * this->k_ - this->text_->r_pos_ / 2.0f, 310.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text("Options");
	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 2) + 1.0f * static_cast<float32>(this->option == 2), 
		0.0f * static_cast<float32>(this->option != 2) + 1.0f * static_cast<float32>(this->option == 2), 
		0.8f * static_cast<float32>(this->option != 2) + 1.0f * static_cast<float32>(this->option == 2));
	this->text_->set_pos(310.0f * this->k_ - this->text_->r_pos_ / 2.0f, 335.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text("Quit");
	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 3) + 1.0f * static_cast<float32>(this->option == 3), 
		0.0f * static_cast<float32>(this->option != 3) + 1.0f * static_cast<float32>(this->option == 3), 
		0.8f * static_cast<float32>(this->option != 3) + 1.0f * static_cast<float32>(this->option == 3));
	this->text_->set_pos(310.0f * this->k_ - this->text_->r_pos_ / 2.0f, 360.0f * this->k_);
	this->text_->draw(false);
}

void Menu::draw_pause_menu()
{
	this->option %= 2;
	this->pause_menu_->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));

	this->text_->set_color(1.0f, 1.0f, 1.0f);

	this->text_->set_text("Back");
	this->text_->set_pos(255.0f * this->k_, 160.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text("Exit");
	this->text_->set_pos(335.0f * this->k_, 160.0f * this->k_);
	this->text_->draw(false);

	this->point_->draw(Resources::get_shader("sprite"), 
		glm::vec2(
			240.0f * static_cast<float32>(this->option == 0) 
			+ 320.0f * static_cast<float32>(this->option == 1),
			static_cast<float32>(this->screen_height_) - 160.0f));
}

void Menu::draw_options_menu(
	const std::string& res,
	const std::string& vol,
	const std::string& diff)
{
	this->option %= 4;
	this->main_menu_->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));

	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 0) + 1.0f * static_cast<float32>(this->option == 0), 
		0.0f * static_cast<float32>(this->option != 0) + 1.0f * static_cast<float32>(this->option == 0), 
		0.8f * static_cast<float32>(this->option != 0) + 1.0f * static_cast<float32>(this->option == 0));

	this->text_->set_text("Resolution");
	this->text_->set_pos(230.0f * this->k_, 285.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text(res);
	this->text_->set_pos(410.0f * this->k_, 285.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 1) + 1.0f * static_cast<float32>(this->option == 1), 
		0.0f * static_cast<float32>(this->option != 1) + 1.0f * static_cast<float32>(this->option == 1), 
		0.8f * static_cast<float32>(this->option != 1) + 1.0f * static_cast<float32>(this->option == 1));

	this->text_->set_text("Volume");
	this->text_->set_pos(230.0f * this->k_, 310.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text(vol);
	this->text_->set_pos(410.0f * this->k_, 310.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 2) + 1.0f * static_cast<float32>(this->option == 2), 
		0.0f * static_cast<float32>(this->option != 2) + 1.0f * static_cast<float32>(this->option == 2), 
		0.8f * static_cast<float32>(this->option != 2) + 1.0f * static_cast<float32>(this->option == 2));

	this->text_->set_text("Difficulty");
	this->text_->set_pos(230.0f * this->k_, 335.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text(diff);
	this->text_->set_pos(410.0f * this->k_, 335.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_text("Quit");
	this->text_->set_color(
		0.76f * static_cast<float32>(this->option != 3) + 1.0f * static_cast<float32>(this->option == 3), 
		0.0f * static_cast<float32>(this->option != 3) + 1.0f * static_cast<float32>(this->option == 3), 
		0.8f * static_cast<float32>(this->option != 3) + 1.0f * static_cast<float32>(this->option == 3));
	this->text_->set_pos(230.0f * this->k_, 360.0f * this->k_);
	this->text_->draw(false);
}

void Menu::draw_game_over()
{
	this->game_over_->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));

	this->text_->set_text("Game Over");
	this->text_->set_color(1.0f, 1.0f, 1.0f);
	this->text_->set_pos(320.0f * this->k_ - this->text_->r_pos_ / 2.0f, 160.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text("Hit Z to continue");
	this->text_->set_color(0.9f, 0.0f, 0.0f);
	this->text_->set_pos(320.0f * this->k_ - this->text_->r_pos_ / 2.0f, 200.0f * this->k_);
	this->text_->draw(false);
}

void Menu::draw_lvl_switch(
		const uint32& p1, 
		const uint32& p2, 
		const uint32& p3, 
		const uint32& p4,
		uint32& sum,
		const uint32& stage)
{
	this->lvl_switch_->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));

	this->text_->set_color(1.0f, 0.0f, 0.0f);
	this->text_->set_text(std::to_string(stage));
	this->text_->set_pos(190.0f * this->k_, 95.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_color(1.0f, 1.0f, 1.0f);

	this->text_->set_text(std::to_string(p1));
	this->text_->set_pos(335.0f * this->k_, 140.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_text(std::to_string(p2));
	this->text_->set_pos(335.0f * this->k_, 175.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_text(std::to_string(p3));
	this->text_->set_pos(335.0f * this->k_, 210.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_text(std::to_string(p4));
	this->text_->set_pos(335.0f * this->k_, 240.0f * this->k_);
	this->text_->draw(true);

	sum = p1 + p2 + p3 + p4;
	this->text_->set_text(std::to_string(sum));
	this->text_->set_pos(335.0f * this->k_, 270.0f * this->k_);
	this->text_->draw(true);
}

void Menu::draw_ending(const std::string& diff, const uint32& hi_score, const uint32& score)
{
	this->ending_->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));

	this->text_->set_color(1.0f, 1.0f, 1.0f);

	this->text_->set_text("DIFFICULTY:");
	this->text_->set_pos(300.0f * this->k_, 120.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text(diff);
	this->text_->set_pos(600.0f * this->k_, 120.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_text("HI-SCORE:");
	this->text_->set_pos(300.0f * this->k_, 150.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text(std::to_string(hi_score));
	this->text_->set_pos(600.0f * this->k_, 150.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_text("OVERALL SCORE:");
	this->text_->set_pos(300.0f * this->k_, 180.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text(std::to_string(score));
	this->text_->set_pos(600.0f * this->k_, 180.0f * this->k_);
	this->text_->draw(true);

	this->text_->set_color(1.0f, 0.0f, 0.0f);
	this->text_->set_text("!! Thanks for playing !!");
	this->text_->set_pos(450.0f * this->k_ - this->text_->r_pos_ / 2.0f, 220.0f * this->k_);
	this->text_->draw(false);

	this->text_->set_text("Hit Z to continue");
	this->text_->set_pos(450.0f * this->k_ - this->text_->r_pos_ / 2.0f, 250.0f * this->k_);
	this->text_->draw(false);
}

void Menu::move_up()
{
	this->option--;
}

void Menu::move_down()
{
	this->option++;
}
