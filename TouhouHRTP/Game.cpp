#include "Game.h"

Game::Game()
{
	this->init_window();
	this->init_text();
	this->init_textures();
	this->init_objects();
}

Game::~Game()
{
	//delete this->stage_txt_;
	//delete this->diffic_txt_;
	//delete this->score_txt_;
	//delete this->hi_score_txt_;

	delete this->text_;

	delete this->reimu_;
	delete this->ball_;
	delete this->font16_;
	delete this->inter_;
	delete this->bomb_;
	delete this->hp_;

	for (auto bullets : this->player_bullets_)
		delete bullets;
	for (auto bullets : this->enemy_bullets_)
		delete bullets;

	Level_manager::clear(this->lvl_objs_);

	Resources::clear();
	delete this->render_;

	glfwTerminate();
}

void Game::update_enemies()
{
	for (auto turret : this->lvl_objs_.turrets)
	{
		turret->shoot(this->reimu_->get_pos(), this->enemy_bullets_);
		turret->update(this->delta_time_);
	}	

	for (auto bullet : this->enemy_bullets_)
	{
		bullet->update(this->delta_time_);
		if (bullet->get_pos().x < 0.0f
			&& bullet->get_pos().x > static_cast<float32>(this->scr_width_)
			&& bullet->get_pos().y < 0.0f
			&& bullet->get_pos().y > static_cast<float32>(this->scr_width_) - this->upper_bound_)
		{
			this->enemy_bullets_.erase(
				std::remove(this->enemy_bullets_.begin(), 
					this->enemy_bullets_.end(), 
					bullet), this->enemy_bullets_.end());
			delete bullet;
			continue;
		}

		if (this->ball_->check_intersect_circle_circle(*bullet))
		{
			this->enemy_bullets_.erase(
				std::remove(this->enemy_bullets_.begin(), 
					this->enemy_bullets_.end(), 
					bullet), this->enemy_bullets_.end());
			delete bullet;
			continue;
		}

		if (bullet->check_intersect_circle_rect(*this->reimu_)
			&& this->reimu_->get_state() != Player::State::DEAD
			&& !this->reimu_->get_invis())
		{
			this->reimu_->set_state(Player::State::DEAD);
			this->time_ += static_cast<float32>(this->timeout_) * 1000.0f
				+ static_cast<float32>(!this->timeout_) * 200.0f;
			this->time_ = glm::clamp(this->time_, 0.0f, 1000.0f);
		}

		for (auto p_bullet : this->player_bullets_)
			if (bullet->check_intersect_circle_rect(*p_bullet))
			{
				this->enemy_bullets_.erase(
					std::remove(this->enemy_bullets_.begin(), 
						this->enemy_bullets_.end(), 
						bullet), this->enemy_bullets_.end());
				delete bullet;

				this->player_bullets_.erase(
				std::remove(this->player_bullets_.begin(),
					this->player_bullets_.end(), 
				p_bullet), this->player_bullets_.end());
				delete p_bullet;

				break;
			}
	}
	if (this->timeout_)
	{
		this->bullet_flurry_timer_ += delta_time_;
		if (this->bullet_flurry_timer_ > 0.25f)
		{
			constexpr int32 k = 4;
			const int32 step = this->scr_width_ / (k + 1);
			//for (int32 i = 0; i < k; ++i)
			//{
			const int32 i = this->rd_() % k + 1;
			this->enemy_bullets_.push_back(new EnemyBullet(this->scr_width_, this->scr_height_));
			
			this->enemy_bullets_.back()->set_pos(
				static_cast<float32>(step * i), 
				static_cast<float32>(this->scr_height_) - this->upper_bound_ - 10.0f);

			this->enemy_bullets_.back()->set_direction({
				(i >= k / 2) * -sqrt(2.0f) / 2.0f + (i < k / 2) * sqrt(2.0f) / 2.0f,
				-sqrt(3.0f) / 2.0f});
			//}
			this->bullet_flurry_timer_ = 0.0f;
		}
	}
}

void Game::update_ball()
{
	if (this->ball_->check_intersect(*this->reimu_) 
		&& this->reimu_->get_state() != Player::State::DEAD
		&& (!this->reimu_->get_invis()
			|| (this->reimu_->get_state() != Player::State::STAND
				&& this->reimu_->get_state() != Player::State::RUN_L
				&& this->reimu_->get_state() != Player::State::RUN_R)))
	{
		const glm::vec2 pushDir = glm::normalize(this->ball_->get_pos() - this->reimu_->get_pos());
		this->ball_->set_vel(600.0f * pushDir.x, 700.0f);
		//ball->setVel(200.0f * pushDir.x, abs(200.0f * ((int)(pushDir.y * 30.0f) % 3 + 1)) + 10.0f);
	}
	for (auto bumper : this->lvl_objs_.bumpers)
		if (this->ball_->check_intersect_circle_circle(*bumper))
		{
			const glm::vec2 pushDirB = glm::normalize(this->ball_->get_pos() - bumper->get_pos());
			this->ball_->set_vel(600.0f * pushDirB.x, 200.0f * pushDirB.y);
		}
	for (auto rail : this->lvl_objs_.rails)
		if (this->ball_->check_intersect_circle_rect(*rail))
		{
			// this method is stupid af, but works perfectly, almost ;)
			// needs to find more optimized method

			const glm::vec2 pushDirB = glm::normalize(this->ball_->get_pos() - rail->get_pos());

			if (rail->get_pos().y - rail->get_size().y / 2.1f < this->ball_->get_pos().y
				&& rail->get_pos().y + rail->get_size().y / 2.1f > this->ball_->get_pos().y
				&& !rail->get_rot())
				this->ball_->set_vel(this->ball_->get_vel().x, 0.0f);
			if ((rail->get_pos().y - rail->get_size().y / 2.4f >= this->ball_->get_pos().y
					|| rail->get_pos().y + rail->get_size().y / 2.4f <= this->ball_->get_pos().y)
					&& !rail->get_rot())
				this->ball_->set_vel(this->ball_->get_vel().x, 200.0f * signum(pushDirB.y));

			if (rail->get_pos().x - rail->get_size().x / 2.1f < this->ball_->get_pos().x
				&& rail->get_pos().x + rail->get_size().x / 2.1f > this->ball_->get_pos().x
				&& rail->get_rot())
				this->ball_->set_vel(-100.0f * signum(pushDirB.x), this->ball_->get_vel().y);
			if ((rail->get_pos().x - rail->get_size().x / 2.4f >= this->ball_->get_pos().x
					|| rail->get_pos().x + rail->get_size().x / 2.4f <= this->ball_->get_pos().x)
					&& rail->get_rot())
				this->ball_->set_vel(-this->ball_->get_vel().x, this->ball_->get_vel().y);
		}
	for (uint32 i = 0; i < this->lvl_objs_.ball_tps.size(); ++i)
		if (this->ball_->check_intersect_circle_circle(*this->lvl_objs_.ball_tps[i])
			&& this->lvl_objs_.ball_tps[i]->tp_ready())
		{
			uint32 rand_index = 0;
			do
				rand_index = this->rd_() % this->lvl_objs_.ball_tps.size();
			while (rand_index == i && this->lvl_objs_.ball_tps.size() > 1);
			this->lvl_objs_.ball_tps[i]->teleport(*this->ball_, *this->lvl_objs_.ball_tps[rand_index]);
		}
	if (this->ball_->get_pos().y < this->ball_->get_size().y / 2.0f + 3.0f)
		this->mult_ = 1;
	this->ball_->update(this->delta_time_);
}

void Game::update_player	()
{
	for (auto bullet : this->player_bullets_)
	{
		bullet->update(this->delta_time_);
		glm::vec2 pushDir = glm::normalize(this->ball_->get_pos() - bullet->get_pos()); 
		if (bullet->check_intersect(*this->ball_))
			this->ball_->set_vel(600.0f * pushDir.x, 700.0f);

		if (bullet->get_pos().y > static_cast<float32>(this->scr_height_) - this->upper_bound_ || bullet->check_intersect(*this->ball_))
		{
			this->player_bullets_.erase(
				std::remove(this->player_bullets_.begin(),
					this->player_bullets_.end(), 
				bullet), this->player_bullets_.end());
			delete bullet;
		}
	}

	if (this->ball_->check_intersect(*this->reimu_) 
		&& this->reimu_->get_state() != Player::State::DEAD
		&& !this->reimu_->get_invis())
	{
		this->reimu_->set_state(Player::State::DEAD);
		this->time_ += static_cast<float32>(this->timeout_) * 1000.0f
			+ static_cast<float32>(!this->timeout_) * 200.0f;
		this->time_ = glm::clamp(this->time_, 0.0f, 1000.0f);
	}

	this->reimu_->update(this->delta_time_);
}

void Game::update_tiles()
{
	for (auto tile : this->lvl_objs_.tiles)
	{
		if (tile->get_state() != Tile::State::DEAD)
		{
			if (tile->check_intersect(*this->ball_))
				tile->flip();
			tile->update(this->delta_time_);
			this->score_ += tile->get_state_change() * ((this->mult_ - 1) * (this->mult_ - 1) * 20 + 100);
			this->mult_ += tile->get_state_change();
		}
		//auto it = std::remove_if(
		//	this->tiles_.begin(),
		//	this->tiles_.end(),
		//	[](Tile* tile) { return tile->get_state() == Tile::State::DEAD; });
		//this->tiles_.erase(it, this->tiles_.end());
	}

	for (auto ball_tp : this->lvl_objs_.ball_tps)
		ball_tp->update(this->delta_time_);
}

void Game::draw_text()
{

	for (auto& [fst, snd] : this->text_pos_)
	{
		this->text_->set_text(std::to_string(*fst));
		this->text_->set_pos(snd.x, snd.y);
		this->text_->draw(true);
	}

	this->text_->set_text("Normal");
	this->text_->set_pos(this->diffic_pos_.x, this->diffic_pos_.y);
	this->text_->draw(true);

	this->text_->set_text(std::to_string(static_cast<int32>(this->time_)));
	this->text_->set_pos(this->time_pos_.x, this->time_pos_.y);
	this->text_->draw(true);
}

auto Game::is_running	() -> bool
{
	return (this->runtime_flag_ && !glfwWindowShouldClose(this->window_));
}

void Game::clear		()
{
	this->render_->clear();
}

void Game::update		()
{
	float32 currentFrame = static_cast<float32>(glfwGetTime());
	this->delta_time_ = currentFrame - this->last_frame_;
	this->last_frame_ = currentFrame;
	
	this->render_->clear();

	if (!pause_)
	{
		this->hi_score_ = (this->score_ >= this->hi_score_) * this->score_
							+ (this->score_ < this->hi_score_) * this->hi_score_;
		this->hi_mult_ = (this->mult_ >= this->hi_mult_) * this->mult_
							+ (this->mult_ < this->hi_mult_) * this->hi_mult_;
		this->time_ -= delta_time_ * 50.0f * static_cast<float32>(this->time_ > 0.0f);
		this->timeout_  = this->time_ <= 1.0f;
		//Resources::get_shader("flash")->use();
		//Resources::get_shader("flash")->setFloat("time", this->temp_t_);
		//this->temp_t_ += this->delta_time_ * 2.0f;
		//this->temp_t_ = (this->temp_t_ > 2.0f) * -2.0f + this->temp_t_ * (this->temp_t_ < 2.0f);

		this->update_enemies();
		this->update_tiles();
		this->update_player();
		this->update_ball();
	}
}

void Game::display()
{
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL * this->polygon_flag_ + GL_LINE * !this->polygon_flag_);

	Level_manager::draw(this->lvl_objs_);
	for (auto bullet : this->player_bullets_)
		bullet->draw();
	this->reimu_->draw();

	for (auto bullet : this->enemy_bullets_)
		bullet->draw();

	this->ball_->draw();
	
	for (uint32 i = 2; i <= this->reimu_->get_hp(); ++i)
		this->hp_->draw(Resources::get_shader("sprite"), 
			glm::vec2(
				static_cast<float32>(i - 2) * this->hp_->getSize().x + 128.0f,
				static_cast<float32>(this->scr_height_) - 15.0f));

	for (uint32 i = 0; i < this->reimu_->get_bombs(); ++i)
		this->bomb_->draw(Resources::get_shader("sprite"), 
			glm::vec2(
				static_cast<float32>(i) * this->bomb_->getSize().x + 128.0f,
				static_cast<float32>(this->scr_height_) - 31.0f));

	this->draw_text();
	

	//this->flash_->draw(Resources::get_shader("flash"));

	/*==============================*/

	this->inter_->draw(Resources::get_shader("sprite"), 
		glm::vec2(0.0f, static_cast<float32>(this->scr_height_) - this->inter_->getSize().y));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	this->render_->display();

	glfwPollEvents();
	glfwSwapBuffers(this->window_);
}

void Game::init_window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OS X
#endif // __APPLE__

	this->window_ = glfwCreateWindow(this->win_width_, this->win_height_, this->title_, nullptr, nullptr);
	glfwMakeContextCurrent(this->window_);

	if (!this->window_)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwSetFramebufferSizeCallback(this->window_, framebuffer_size_callback);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to load GLAD" << std::endl;
	}
	glfwSetWindowUserPointer(this->window_, this);
	glfwSetKeyCallback(this->window_, Game::keyCallback);

	glViewport(0, 0, this->win_width_, this->win_height_);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	this->render_ = new RenderWindow(this->win_width_, this->win_height_);
}

void Game::init_text()
{
	this->font16_	= new Font("Fonts/Heuristica-Regular.ttf", 20.0f * this->k_height_);
	Resources::load_shader("text", "Shaders/vText.glsl", "Shaders/fText.glsl");

	this->text_			= new Text	(this->font16_, this->win_width_, this->win_height_);
	this->text_->set_text(std::to_string(this->stage_));

	this->text_pos_[&this->stage_] = {620.0f * this->k_width_, 43.0f * this->k_height_};
	this->text_pos_[&this->score_] = {360.0f * this->k_width_, 30.0f * this->k_height_};
	this->text_pos_[&this->hi_score_] = {360.0f * this->k_width_, 13.0f * this->k_height_};
	this->text_pos_[&this->mult_] = {425.0f * this->k_width_, 30.0f * this->k_height_};
	this->text_pos_[&this->hi_mult_] = {425.0f * this->k_width_, 13.0f * this->k_height_};

	this->diffic_pos_ = {
		620.0f * this->k_width_,
		60.0f * this->k_height_};

	this->time_pos_	= {
		540.0f * this->k_width_,
		14.0f * this->k_height_};
}

void Game::init_textures()
{
	Resources::load_shader("sparkle", "Shaders/vSparkle.glsl", "Shaders/fSparkle.glsl");
	Resources::load_shader("sprite", "Shaders/vSprite.glsl", "Shaders/fSprite.glsl");
	Resources::load_shader("screen", "Shaders/vScreen.glsl", "Shaders/fScreen.glsl");
	//Resources::load_shader("flash", "Shaders/vFlash.glsl", "Shaders/fFlash.glsl");

	Resources::load_texture("reimu", "Textures/reimu.png", false);
	Resources::load_texture("tiles", "Textures/tiles.png", false);
	Resources::load_texture("interface", "Textures/interface.png", false);

	Resources::load_texture("bg0", "Textures/bg0.png", false);
	Resources::load_texture("bg1", "Textures/bg1.png", false);
	Resources::load_texture("bg2", "Textures/bg2.png", false);
	Resources::load_texture("bg3", "Textures/bg3.png", false);
	Resources::load_texture("bg4", "Textures/bg4.png", false);
	Resources::load_texture("bg5", "Textures/bg5.png", false);
	Resources::load_texture("bg6", "Textures/bg6.png", false);
}

void Game::init_objects()
{
	for (uint16 i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i)
		this->keys_[i][0] = this->keys_[i][1] = GLFW_RELEASE;

	this->inter_	= new Sprite(Resources::get_texture("interface"),
		glm::ivec4(0, 0, Resources::get_texture("interface")->width, Resources::get_texture("interface")->height), 
		glm::ivec2(this->scr_width_, this->scr_height_), false);

	this->hp_		= new Sprite(Resources::get_texture("tiles"),
		glm::ivec4(97, 0, 113, 16),
		glm::ivec2(this->scr_width_, this->scr_height_), false);

	this->bomb_		= new Sprite(Resources::get_texture("tiles"),
		glm::ivec4(114, 0, 130, 16), 
		glm::ivec2(this->scr_width_, this->scr_height_), false);
	
	this->reimu_	= new Player(this->scr_width_, this->scr_height_);
	this->ball_		= new Ball	(this->scr_width_, this->scr_height_);

	this->upper_bound_ =  static_cast<float32>(Resources::get_texture("interface")->height);
	this->ball_->set_upper_bound(this->upper_bound_);

	this->lvl_objs_.tile_size = 32.0f;
	this->lvl_objs_.scr_width = this->scr_width_;
	this->lvl_objs_.scr_height = this->scr_height_;

	Level_manager::load(this->lvl_objs_, ("Levels/" + std::to_string(this->stage_) + ".lvl").c_str());

	this->reimu_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, this->reimu_->get_size().y / 2.0f);
	this->ball_->set_pos(20.0f, static_cast<float32>(this->scr_height_) - 30.0f);
	
	//this->flash_ = new Rectangle(glm::ivec2(60, 60), glm::ivec2(this->scr_width_, this->scr_height_));
	//this->flash_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, static_cast<float32>(this->scr_height_) / 2.0f);
}

void Game::framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	glViewport(0, 0, width, height);
}

void Game::process_input()
{
	if (!this->pause_)
	{
		if (!(glfwGetKey(this->window_, GLFW_KEY_RIGHT) 
			|| glfwGetKey(this->window_, GLFW_KEY_LEFT)))
			this->reimu_->set_state(Player::State::STAND);

		if (glfwGetKey(this->window_, GLFW_KEY_LEFT) 
			&& !glfwGetKey(this->window_, GLFW_KEY_X))
			this->reimu_->set_state(Player::State::RUN_L);
		
		if (glfwGetKey(this->window_, GLFW_KEY_RIGHT) 
			&& !glfwGetKey(this->window_, GLFW_KEY_X))
			this->reimu_->set_state(Player::State::RUN_R);

		this->keys_.at(GLFW_KEY_X)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_X));
		if (this->keys_.at(GLFW_KEY_X)[1] == GLFW_PRESS 
			&& this->keys_.at(GLFW_KEY_X)[0] == GLFW_RELEASE)
		{		
			if (glfwGetKey(this->window_, GLFW_KEY_LEFT)
				&& !glfwGetKey(this->window_, GLFW_KEY_RIGHT)
				&& this->reimu_->get_state() != Player::State::SLIDE_R_A2
				&& this->reimu_->get_state() != Player::State::SLIDE_L_A2)
				this->reimu_->set_state(Player::State::SLIDE_L);
			if (!glfwGetKey(this->window_, GLFW_KEY_LEFT)
				&& glfwGetKey(this->window_, GLFW_KEY_RIGHT)
				&& this->reimu_->get_state() != Player::State::SLIDE_R_A2
				&& this->reimu_->get_state() != Player::State::SLIDE_L_A2)
				this->reimu_->set_state(Player::State::SLIDE_R);

			if (this->reimu_->get_state() == Player::State::SLIDE_L
				&& glfwGetKey(this->window_, GLFW_KEY_LEFT))
			{
				this->reimu_->set_state(Player::State::SLIDE_L_A3);
				this->reimu_->set_old_state(Player::State::SLIDE_L_A3);
			}
			if (this->reimu_->get_state() == Player::State::SLIDE_R
				&& glfwGetKey(this->window_, GLFW_KEY_RIGHT))
			{
				this->reimu_->set_state(Player::State::SLIDE_R_A3);
				this->reimu_->set_old_state(Player::State::SLIDE_R_A3);
			}

			if (this->reimu_->get_state() == Player::State::SLIDE_L
				&& this->reimu_->get_state() != Player::State::SLIDE_L_A2
				&& glfwGetKey(this->window_, GLFW_KEY_RIGHT)
				&& !glfwGetKey(this->window_, GLFW_KEY_LEFT))
			{
				this->reimu_->set_state(Player::State::ATTACK2_L);
				this->reimu_->set_old_state(Player::State::ATTACK2_L);
			}
			else if (this->reimu_->get_state() == Player::State::SLIDE_L
				&& this->reimu_->get_state() != Player::State::ATTACK2_L
				&& !glfwGetKey(this->window_, GLFW_KEY_RIGHT)
				&& !glfwGetKey(this->window_, GLFW_KEY_LEFT))
			{
				this->reimu_->set_state(Player::State::SLIDE_L_A2);
				this->reimu_->set_old_state(Player::State::SLIDE_L_A2);
			}

			if (this->reimu_->get_state() == Player::State::SLIDE_R
				&& !glfwGetKey(this->window_, GLFW_KEY_RIGHT)
				&& glfwGetKey(this->window_, GLFW_KEY_LEFT))
			{
				this->reimu_->set_state(Player::State::ATTACK2_R);
				this->reimu_->set_old_state(Player::State::ATTACK2_R);
			}		
			else if (this->reimu_->get_state() == Player::State::SLIDE_R
				&& !glfwGetKey(this->window_, GLFW_KEY_RIGHT)
				&& !glfwGetKey(this->window_, GLFW_KEY_LEFT))
			{
				this->reimu_->set_state(Player::State::SLIDE_R_A2);
				this->reimu_->set_old_state(Player::State::SLIDE_R_A2);
			}

			if (!glfwGetKey(this->window_, GLFW_KEY_LEFT)
				&& !glfwGetKey(this->window_, GLFW_KEY_RIGHT))
				this->reimu_->set_state(Player::State::ATTACK1);		
		}
		this->keys_.at(GLFW_KEY_X)[0] = this->keys_.at(GLFW_KEY_X)[1];

		this->keys_.at(GLFW_KEY_Z)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_Z));
		if (this->keys_.at(GLFW_KEY_Z)[1] == GLFW_PRESS 
			&& this->keys_.at(GLFW_KEY_Z)[0] == GLFW_RELEASE
			&& (this->reimu_->get_state() == Player::State::STAND
				|| this->reimu_->get_state() == Player::State::RUN_L
				|| this->reimu_->get_state() == Player::State::RUN_R))
			this->player_bullets_.push_back(
				new PlayerBullet(glm::vec2(
					this->reimu_->get_pos().x,
					this->reimu_->get_pos().y + this->reimu_->get_size().y / 2.0f),
					this->scr_width_, this->scr_height_));
		this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];

		this->keys_.at(GLFW_KEY_C)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_C));
		if (this->keys_.at(GLFW_KEY_C)[1] == GLFW_PRESS 
			&& this->keys_.at(GLFW_KEY_C)[0] == GLFW_RELEASE
			&& (this->reimu_->get_state() == Player::State::STAND
				|| this->reimu_->get_state() == Player::State::RUN_L
				|| this->reimu_->get_state() == Player::State::RUN_R)
			&& this->reimu_->get_bombs() > 0)
			this->reimu_->set_state(Player::State::BOMB);

		this->keys_.at(GLFW_KEY_C)[0] = this->keys_.at(GLFW_KEY_C)[1];
	}
	this->keys_.at(GLFW_KEY_P)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_P));
	if (this->keys_.at(GLFW_KEY_P)[1] == GLFW_PRESS 
		&& this->keys_.at(GLFW_KEY_P)[0] == GLFW_RELEASE)
		this->pause_ = !this->pause_;
	this->keys_.at(GLFW_KEY_P)[0] = this->keys_.at(GLFW_KEY_P)[1];
}


void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
			
		case GLFW_KEY_F1:
			game->polygon_flag_ = !game->polygon_flag_;
			break;

		default:
			break;
		}
	}
}