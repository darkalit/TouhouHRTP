#include "Game.h"

#include "stb_image.h"

Game::Game()
{
	this->configs_["screen_res_mult"] = "2.0";
	this->configs_["volume"] = "1.00";
	this->configs_["difficulty"] = "1";
	this->configs_["ball_gravity"] = "-900.0";
	this->configs_["energy_loss"] = "0.8";
	this->configs_["player_speed"] = "270.0";
	this->configs_["max_fps"] = "60.0";
	this->configs_["levels_dir"] = "Levels/";

	this->read_config();
	this->scr_res_mult_ = std::stof(this->configs_.at("screen_res_mult"));
	this->win_width_ = static_cast<int32>(
		this->scr_res_mult_
		* static_cast<float32>(this->scr_width_));
	this->win_height_ = static_cast<int32>(
		this->scr_res_mult_
		* static_cast<float32>(this->scr_height_));
	this->k_ = static_cast<float32>(this->win_width_) / static_cast<float32>(this->scr_width_);
	this->max_period_ = 1.0f / std::stof(this->configs_.at("max_fps"));

	this->init_window();
	this->init_text();
	this->init_textures();
	this->init_objects();

	this->se_->setSoundVolume(std::stof(this->configs_.at("volume")));
	for (auto sound : this->sfx_)
		sound->setDefaultVolume(std::clamp(std::stof(this->configs_.at("volume")) - 0.04f, 0.05f, 1.0f));
	this->se_->play2D(this->bgm_[0], true);
}

Game::~Game()
{
	delete this->text_;
	delete this->font20_;

	delete this->reimu_;
	delete this->ball_;
	delete this->inter_;
	delete this->bomb_;
	delete this->hp_;

	for (auto bullets : this->player_bullets_)
		delete bullets;
	for (auto bullets : this->enemy_bullets_)
		delete bullets;
	delete menu_;

	delete this->lvl_objs_.bg;
	Level_manager::clear(this->lvl_objs_);

	Resources::clear();
	delete this->render_;

	this->se_->drop();
	glfwTerminate();
}

void Game::smooth_sound_stop()
{
	this->se_->setSoundVolume(0.0f);
	this->se_->stopAllSounds();
	this->se_->setSoundVolume(std::stof(this->configs_.at("volume")));
	for (auto sound : this->sfx_)
		sound->setDefaultVolume(std::clamp(std::stof(this->configs_.at("volume")) - 0.04f, 0.05f, 1.0f));
}

void Game::write_save()
{
	std::ofstream ofile("save.dat", std::ios::binary);
	this->save_.stage			= this->stage_;
	this->save_.hi_mult			= this->hi_mult_;
	this->save_.hi_score		= this->hi_score_;
	this->save_.score			= this->score_;
	this->save_.player_lives	= this->reimu_->hp;
	this->save_.player_bombs	= this->reimu_->bombs;
	this->save_.difficulty		= this->configs_.at("difficulty");
	ofile.write(reinterpret_cast<char*>(&this->save_), sizeof this->save_);
	ofile.close();
}

void Game::read_save()
{
	try
	{
		std::ifstream ifile("save.dat", std::ios::binary);
		ifile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		ifile.read(reinterpret_cast<char*>(&this->save_), sizeof this->save_);
		ifile.close();

		this->stage_		= this->save_.stage;
		this->hi_mult_		= this->save_.hi_mult;
		this->hi_score_		= this->save_.hi_score;
		this->score_		= this->save_.score;
		this->reimu_->hp	= this->save_.player_lives;
		this->reimu_->bombs	= this->save_.player_bombs;
		this->configs_.at("difficulty") = this->save_.difficulty;

		Level_manager::load(this->lvl_objs_, (this->configs_.at("levels_dir") + this->lvls_list_[this->stage_ - 1]).c_str());
		this->clear_stage();
	}
	catch (std::ifstream::failure&)
	{
		this->stage_ = 1;
		Level_manager::load(this->lvl_objs_, (this->configs_.at("levels_dir") + this->lvls_list_[this->stage_ - 1]).c_str());
		this->reset();
	}
}

void Game::write_config()
{
	if (std::ofstream config_file("config.cfg", std::ios::out); config_file)
	{
		for (const auto& [key, value] : this->configs_)
			config_file << key << ":" << value << "\n";
		config_file.close();
	}
}

void Game::read_config()
{
	if (std::ifstream config_file{"config.cfg"}; config_file)
	{
		for (std::string line{}; std::getline(config_file, line);)
		{
			if (line.find(':') != std::string::npos)
			{
				std::istringstream iss{line};
				if (std::string key{}, value{};				
					std::getline(
						std::getline(iss, key, ':') >> std::ws, 
						value))
					try
					{
						this->configs_.at(key) = value;
					}
					catch (std::out_of_range&){}
			}
		}
		config_file.close();
	}
}

void Game::set_difficulty()
{
	switch (std::stoi(this->configs_.at("difficulty")))
	{
	case 0:
		for (auto turret : this->lvl_objs_.turrets)
			turret->set_bullets_count(0);
		this->shoot_flip_ = false;
		this->score_calc_ = [&](const uint32& base)
			{ return (this->mult_ - 1) * (this->mult_ - 1) * 20 + base; };
		break;

	case 1:
		for (auto turret : this->lvl_objs_.turrets)
			turret->set_delay(2.0f);
		this->shoot_flip_ = false;
		this->score_calc_ = [&](const uint32& base)
			{ return (this->mult_ - 1) * (this->mult_ - 1) * 20 + base; };
		break;

	case 2:
		for (auto turret : this->lvl_objs_.turrets)
			turret->set_delay(1.25f);
		this->shoot_flip_ = false;
		this->score_calc_ = [&](const uint32& base)
			{ return (this->mult_ - 1) * (this->mult_ - 1) * 20 + base; };
		break;

	case 3:
		for (auto turret : this->lvl_objs_.turrets)
			turret->set_delay(1.25f);
		this->shoot_flip_ = true;
		this->score_calc_ = [&](const uint32& base)
			{ return (this->mult_ - 1) * (this->mult_ - 1) * 35 + base; };
		break;

	default:
		break;
	}
}

void Game::reset()
{
	this->time_ = 1000.0f;
	this->stage_ = 1;
	this->timeout_ = false;
	this->bullet_flurry_timer_ = 0.0f;
	this->score_ = 0;
	this->mult_ = 1;
	this->hi_mult_ = 1;

	for (auto tile : this->lvl_objs_.tiles)
		tile->reset();

	for (auto turret : this->lvl_objs_.turrets)
		turret->reset();

	for (auto bullet : this->enemy_bullets_)
		delete bullet;
	this->enemy_bullets_.clear();

	for (auto bullet : this->player_bullets_)
		delete bullet;
	this->player_bullets_.clear();

	for (auto bonus : this->bonuses_)
		delete bonus;
	this->bonuses_.clear();

	this->reimu_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, this->reimu_->get_size().y / 2.0f);
	this->reimu_->reset();
	this->ball_->set_pos(620.0f, 100.0f);
	this->ball_->set_vel(-400.0f, 800.0f);
	this->ball_upd_f_ = false;
}

void Game::clear_stage()
{
	this->time_ = 1000.0f;
	this->timeout_ = false;
	this->bullet_flurry_timer_ = 0.0f;
	this->mult_ = 1;

	for (auto tile : this->lvl_objs_.tiles)
		tile->reset();

	for (auto bullet : this->enemy_bullets_)
		delete bullet;
	this->enemy_bullets_.clear();

	for (auto bullet : this->player_bullets_)
		delete bullet;
	this->player_bullets_.clear();

	this->reimu_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, this->reimu_->get_size().y / 2.0f);
	this->ball_->set_pos(620.0f, 100.0f);
	this->ball_->set_vel(-400.0f, 800.0f);
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

		if (bullet->check_intersect(*this->reimu_)
			&& this->reimu_->get_state() == Player::State::ATTACK1)
		{
			const glm::vec2 dir = glm::normalize(bullet->get_pos() - this->reimu_->get_pos());
			bullet->set_direction(dir);
			bullet->move(dir.x * 2.0f, dir.y * 2.0f);
			continue;
		}		

		for (auto p_bullet : this->player_bullets_)
			if (bullet->check_intersect_circle_rect(*p_bullet))
			{
				this->se_->play2D(this->sfx_[2]);
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
			const auto step = static_cast<int32>(this->scr_width_ / (k + 1));
			const auto i	= static_cast<int32>(this->rd_() % k + 1);
			this->enemy_bullets_.push_back(new EnemyBullet(this->scr_width_, this->scr_height_));
			
			this->enemy_bullets_.back()->set_pos(
				static_cast<float32>(step * i), 
				static_cast<float32>(this->scr_height_) - this->upper_bound_ - 10.0f);

			this->enemy_bullets_.back()->set_direction({
				static_cast<float32>(i >= k / 2) * -sqrt(2.0f) / 2.0f + static_cast<float32>(i < k / 2) * sqrt(2.0f) / 2.0f,
				-sqrt(3.0f) / 2.0f});
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
				this->ball_->set_vel(this->ball_->get_vel().x, 200.0f * static_cast<float32>(signum(pushDirB.y)));

			if (rail->get_pos().x - rail->get_size().x / 2.1f < this->ball_->get_pos().x
				&& rail->get_pos().x + rail->get_size().x / 2.1f > this->ball_->get_pos().x
				&& rail->get_rot())
				this->ball_->set_vel(-100.0f * static_cast<float32>(signum(pushDirB.x)), this->ball_->get_vel().y);
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
		const glm::vec2 pushDir = glm::normalize(this->ball_->get_pos() - bullet->get_pos()); 
		if (bullet->check_intersect(*this->ball_))
			this->ball_->set_vel(600.0f * pushDir.x, 700.0f);

		if (bullet->get_pos().y > static_cast<float32>(this->scr_height_) - this->upper_bound_ || bullet->check_intersect(*this->ball_))
		{
			this->player_bullets_.erase(
				std::remove(this->player_bullets_.begin(),
					this->player_bullets_.end(), 
				bullet), this->player_bullets_.end());
			delete bullet;
			break;
		}
	}

	if (this->ball_->check_intersect(*this->reimu_) 
		&& this->reimu_->get_state() != Player::State::DEAD
		&& !this->reimu_->get_invis())
	{
		this->reimu_->set_state(Player::State::DEAD);
		if (this->reimu_->get_state() == Player::State::STAND
			|| this->reimu_->get_state() == Player::State::RUN_L
			|| this->reimu_->get_state() == Player::State::RUN_R)
		{
			this->se_->play2D(this->sfx_[3]);
			this->time_ += static_cast<float32>(this->timeout_) * 1000.0f
						+ static_cast<float32>(!this->timeout_) * 200.0f;
			this->time_ = glm::clamp(this->time_, 0.0f, 1000.0f);
		}
	}
	for (auto bullet : this->enemy_bullets_)
		if (bullet->check_intersect(*this->reimu_)
			&& this->reimu_->get_state() != Player::State::DEAD
			&& !this->reimu_->get_invis())
		{
			this->se_->play2D(this->sfx_[3]);
			this->reimu_->set_state(Player::State::DEAD);
			this->reimu_->set_old_state(Player::State::DEAD);
			this->time_ += static_cast<float32>(this->timeout_) * 1000.0f
						+ static_cast<float32>(!this->timeout_) * 200.0f;
			this->time_ = glm::clamp(this->time_, 0.0f, 1000.0f);
		}

	this->reimu_->update(this->delta_time_);
	if (this->reimu_->hp <= 0)
		this->state_ = Menu::GAME_OVER;
}

void Game::update_tiles()
{
	this->stage_clear_ = true;
	this->starting_f_ = false;
	for (auto tile : this->lvl_objs_.tiles)
	{
		if (tile->hp != 0)
		{
			if (tile->check_intersect(*this->ball_))
			{
				if (tile->play_s_f)
				{
					this->se_->stopAllSoundsOfSoundSource(this->sfx_[0]);
					this->se_->play2D(this->sfx_[0]);
				}
				tile->flip();		
			}				

			tile->update(this->delta_time_);
			this->score_ += tile->get_hp_change() * this->score_calc_(this->tile_val_);
			this->mult_ += tile->get_hp_change();
			if (tile->get_hp_change())
				tile->play_s_f = true;
			if (tile->get_hp_change()
				&& this->shoot_flip_)
			{
				this->enemy_bullets_.push_back(new EnemyBullet(this->scr_width_, this->scr_height_));
				const glm::vec2 direction = glm::normalize(this->reimu_->get_pos() - tile->get_pos());
				this->enemy_bullets_.back()->set_pos(tile->get_pos().x, tile->get_pos().y);
				this->enemy_bullets_.back()->set_direction(direction);
			}
			if (tile->get_hp_change() 
				&& this->point_lim_ > 0
				&& this->rd_() % 4 == 3
				&& this->point_timer_ > 1.0f)
			{
				this->point_timer_ = 0.0f;
				this->bonuses_.push_back(new Bonus(1, this->scr_width_, this->scr_height_));
				this->bonuses_.back()->set_pos(tile->get_pos().x, tile->get_pos().y);
				this->point_lim_--;
			}
			if (tile->get_hp_change()
				&& this->rd_() % 500 == 1)
			{
				this->bonuses_.push_back(new Bonus(0, this->scr_width_, this->scr_height_));
				this->bonuses_.back()->set_pos(tile->get_pos().x, tile->get_pos().y);
			}
			if (tile->get_starting())
				this->starting_f_ = true;
			this->stage_clear_ = false;
		}
	}
	this->point_timer_ += this->delta_time_;

	if (this->stage_clear_)
		this->state_ = Menu::LVL_SWITCH;

	for (auto bonus : this->bonuses_)
	{
		bonus->update(this->delta_time_);
		if (bonus->get_pos().y < 0.0f || bonus->check_intersect(*this->reimu_))
		{
			if (bonus->is_bomb())
			{
				this->reimu_->bombs += bonus->check_intersect(*this->reimu_);
				if (bonus->check_intersect(*this->reimu_))
					this->se_->play2D(this->sfx_[5]);
			}
			else
			{
				this->point_c_ *= !bonus->jump_f;
				this->point_c_ += bonus->check_intersect(*this->reimu_);
				this->score_ += bonus->check_intersect(*this->reimu_) * 1000 * this->point_c_;
				if (bonus->check_intersect(*this->reimu_))
					this->se_->play2D(this->sfx_[7]);
			}			
			this->bonuses_.erase(
				std::remove(this->bonuses_.begin(),
					this->bonuses_.end(),
					bonus), this->bonuses_.end());
			delete bonus;
		}
	}

	for (auto ball_tp : this->lvl_objs_.ball_tps)
		ball_tp->update(this->delta_time_);
}

void Game::draw_text()
{
	this->text_->set_color(1.0f, 1.0f, 1.0f);
	for (auto& [fst, snd] : this->text_pos_)
	{
		this->text_->set_text(std::to_string(*fst));
		this->text_->set_pos(snd.x, snd.y);
		this->text_->draw(true);
	}

	this->text_->set_text(this->difficulties_[std::stoi(this->configs_.at("difficulty"))]);
	this->text_->set_pos(this->diffic_pos_.x, this->diffic_pos_.y);
	this->text_->draw(true);

	this->text_->set_text(std::to_string(static_cast<int32>(this->time_)));
	this->text_->set_pos(this->time_pos_.x, this->time_pos_.y);
	this->text_->draw(true);
}

void Game::update_gameplay()
{
	this->hi_score_ = (this->score_ >= this->hi_score_) * this->score_
						+ (this->score_ < this->hi_score_) * this->hi_score_;
	this->hi_mult_	= (this->mult_ >= this->hi_mult_) * this->mult_
						+ (this->mult_ < this->hi_mult_) * this->hi_mult_;
	
	const auto temp_score = this->score_;
	this->update_tiles();
	if (!this->starting_f_)
	{
		this->update_player();
		if (this->ball_upd_f_)
		{
			this->time_ -= delta_time_ * 20.0f * static_cast<float32>(this->time_ > 0.0f);
			this->timeout_  = this->time_ <= 1.0f;
			this->update_enemies();
			this->update_ball();
		}
	}
	if (temp_score % 400000 > this->score_ % 400000)
		this->reimu_->hp = glm::clamp(this->reimu_->hp++, 0u, 7u);
	
}

void Game::draw_gameplay()
{
	Level_manager::draw(this->lvl_objs_);
	for (auto bullet : this->player_bullets_)
		bullet->draw();
	this->reimu_->draw();

	for (auto bullet : this->enemy_bullets_)
		bullet->draw();

	this->ball_->draw();

	for (auto bonus : this->bonuses_)
		bonus->draw();

	for (uint32 i = 2; i <= this->reimu_->hp; ++i)
		this->hp_->draw(Resources::get_shader("sprite"), 
			glm::vec2(
				static_cast<float32>(i - 2) * this->hp_->getSize().x + 128.0f,
				static_cast<float32>(this->scr_height_) - 15.0f));

	for (uint32 i = 0; i < this->reimu_->bombs; ++i)
		this->bomb_->draw(Resources::get_shader("sprite"), 
			glm::vec2(
				static_cast<float32>(i) * this->bomb_->getSize().x + 128.0f,
				static_cast<float32>(this->scr_height_) - 31.0f));

	this->draw_text();

	/*==============================*/

	this->inter_->draw(Resources::get_shader("sprite"), 
		glm::vec2(0.0f, static_cast<float32>(this->scr_height_) - this->inter_->getSize().y));
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
	const auto currentFrame = static_cast<float32>(glfwGetTime());
	this->delta_time_ = currentFrame - this->last_frame_;

	if (this->delta_time_ >= this->max_period_)
	{
		this->last_frame_ = currentFrame;
	
		switch (this->state_)
		{
		case Menu::GAMEPLAY:
			this->update_gameplay();
			break;

		default: 
			break;
		}
	}	
}

void Game::display()
{
	if (this->delta_time_ >= this->max_period_)
	{
		this->render_->clear();
		glLineWidth(2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL * this->polygon_flag_ + GL_LINE * !this->polygon_flag_);

		/*==============================*/

		switch (this->state_)
		{
		case Menu::GAMEPLAY:
			this->draw_gameplay();
			break;
		case Menu::MAIN_MENU:
			this->menu_->draw_main_menu();
			break;
		case Menu::OPTIONS:
			this->menu_->draw_options_menu(
				this->configs_.at("screen_res_mult"),
				this->configs_.at("volume"),
				this->configs_.at("difficulty"));
			break;
		case Menu::PAUSE:
			this->draw_gameplay();
			this->menu_->draw_pause_menu();
			break;
		case Menu::GAME_OVER:
			this->menu_->draw_game_over();
			break;
		case Menu::LVL_SWITCH:
			this->draw_gameplay();
			this->menu_->draw_lvl_switch(
				static_cast<uint32>(floor(this->time_)) * 30,
				this->hi_mult_ * 1000,
				(this->reimu_->hp - 1) * 2000 + this->reimu_->bombs * 1000,
				(this->stage_ - 1) * 2000,
				this->level_score_,
				this->stage_);
			break;
		case Menu::ENDING:
			this->menu_->draw_ending(this->difficulties_[std::stoi(this->configs_.at("difficulty"))], this->hi_score_, this->score_);
			break;
		}

		/*==============================*/

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		this->render_->display();

		glfwPollEvents();
		glfwSwapBuffers(this->window_);
	}
	
}

void Game::init_window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->window_ = glfwCreateWindow(this->win_width_, this->win_height_, this->title_, nullptr, nullptr);
	glfwMakeContextCurrent(this->window_);

	if (!this->window_)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwSetFramebufferSizeCallback(this->window_, framebuffer_size_callback);
	glfwSetWindowSizeCallback(this->window_, window_size_callback);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to load GLAD" << std::endl;
	}
	glfwSetWindowUserPointer(this->window_, this);

	glViewport(0, 0, this->win_width_, this->win_height_);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetWindowAspectRatio(this->window_, 8, 5);
	this->render_ = new RenderWindow(this->win_width_, this->win_height_);

	GLFWimage images[1];
	images[0].pixels = stbi_load("Icon.png", &images[0].width, &images[0].height, nullptr, 4);
	glfwSetWindowIcon(this->window_, 1, images); 
	stbi_image_free(images[0].pixels);
}

void Game::init_text()
{
	this->font20_	= new Font("Fonts/Heuristica-Regular.ttf", 20.0f * this->k_);
	Resources::load_shader("text", "Shaders/vText.glsl", "Shaders/fText.glsl");

	this->text_		= new Text	(this->font20_, this->win_width_, this->win_height_);
	this->text_->set_text(std::to_string(this->stage_));

	this->text_pos_[&this->stage_]		= {620.0f * this->k_, 43.0f * this->k_};
	this->text_pos_[&this->score_]		= {360.0f * this->k_, 30.0f * this->k_};
	this->text_pos_[&this->hi_score_]	= {360.0f * this->k_, 13.0f * this->k_};
	this->text_pos_[&this->mult_]		= {425.0f * this->k_, 30.0f * this->k_};
	this->text_pos_[&this->hi_mult_]	= {425.0f * this->k_, 13.0f * this->k_};

	this->diffic_pos_ = {
		620.0f * this->k_,
		60.0f * this->k_};

	this->time_pos_	= {
		540.0f * this->k_,
		14.0f * this->k_};
}

void Game::init_textures()
{
	Resources::load_shader("sparkle", "Shaders/vSparkle.glsl", "Shaders/fSparkle.glsl");
	Resources::load_shader("sprite", "Shaders/vSprite.glsl", "Shaders/fSprite.glsl");
	Resources::load_shader("screen", "Shaders/vScreen.glsl", "Shaders/fScreen.glsl");

	Resources::load_texture("main_menu", "Textures/main_menu.png", false);
	Resources::load_texture("pause_menu", "Textures/pause.png", false);
	Resources::load_texture("game_over", "Textures/game_over.png", false);
	Resources::load_texture("level_switch", "Textures/level_switch.png", false);
	Resources::load_texture("ending", "Textures/ending.png", false);

	Resources::load_texture("reimu", "Textures/reimu.png", false);
	Resources::load_texture("tiles", "Textures/tiles.png", false);
	Resources::load_texture("interface", "Textures/interface.png", false);
	Resources::load_texture("boom", "Textures/boom.png", false);

	Resources::load_texture("bg0", "Textures/bg0.png", false);
	Resources::load_texture("bg1", "Textures/bg1.png", false);
	Resources::load_texture("bg2", "Textures/bg2.png", false);
	Resources::load_texture("bg3", "Textures/bg3.png", false);
	Resources::load_texture("bg4", "Textures/bg4.png", false);
	Resources::load_texture("bg5", "Textures/bg5.png", false);
	Resources::load_texture("bg6", "Textures/bg6.png", false);

	Resources::load_sprites("boom", {
		new Sprite(Resources::get_texture("boom"),	glm::ivec4(0,  0, 640, 400),		glm::ivec2(this->scr_width_, this->scr_height_), false),
		new Sprite(Resources::get_texture("boom"),	glm::ivec4(640,  0, 1280, 400),		glm::ivec2(this->scr_width_, this->scr_height_), false),
		new Sprite(Resources::get_texture("boom"),	glm::ivec4(1280,  0, 1920, 400),	glm::ivec2(this->scr_width_, this->scr_height_), false),
		new Sprite(Resources::get_texture("boom"),	glm::ivec4(0,  400, 640, 800),		glm::ivec2(this->scr_width_, this->scr_height_), false),
		new Sprite(Resources::get_texture("boom"),	glm::ivec4(640,  400, 640, 800),	glm::ivec2(this->scr_width_, this->scr_height_), false),
		new Sprite(Resources::get_texture("boom"),	glm::ivec4(1280,  400, 640, 800),	glm::ivec2(this->scr_width_, this->scr_height_), false)});
	Resources::load_sprite("Points",
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(101, 911, 124, 934), glm::ivec2(this->scr_width_, this->scr_height_)));
	Resources::load_sprite("BombEx",
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(96, 874, 128, 906),	glm::ivec2(this->scr_width_, this->scr_height_)));
	Resources::load_sprite("EBullet", 
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 17, 105, 25),	glm::ivec2(this->scr_width_, this->scr_height_)));
	Resources::load_sprite("PBullet",
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(99, 32, 109, 48),	glm::ivec2(this->scr_width_, this->scr_height_)));
	Resources::load_sprites("Turret", {
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 586, 127, 616),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 616, 127, 646),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("Ball", {
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97,  97, 122, 122),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 123, 122, 148),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 149, 122, 174),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 175, 122, 199),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("T0", {
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(96, 519, 128, 551),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("T1", {
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 423, 129, 455),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 455, 129, 487),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 486, 129, 518),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("T2", {
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 328, 129, 360),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 360, 129, 392),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 391, 129, 423),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("T3",
		{new Sprite(Resources::get_texture("tiles"),	glm::ivec4(97, 233, 129, 265),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 265, 129, 297),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(97, 296, 129, 328),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("Rail",
		{new Sprite(Resources::get_texture("tiles"),	glm::ivec4(96, 647, 128, 653),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("tiles"),		glm::ivec4(96, 679, 102, 711),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("Bumper",
		{new Sprite(Resources::get_texture("tiles"),	glm::ivec4(101, 555, 125, 579), glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("Ball_TP",
		{new Sprite(Resources::get_texture("tiles"),	glm::ivec4(96, 776, 128, 808),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PStand",
		{new Sprite(Resources::get_texture("reimu"),	glm::ivec4(7, 0, 28, 34),		glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PRun", {
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(66, 0, 96, 34),		glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(98, 0, 128, 34),		glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PAttack1", {
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(4, 86, 46, 128),		glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(48, 86, 90, 128),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(90, 86, 132, 128),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(134, 86, 176, 128),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PAttack2",
		{new Sprite(Resources::get_texture("reimu"),	glm::ivec4(1, 36, 49, 84),		glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(52, 36, 100, 84),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(103, 36, 151, 84),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(152, 36, 200, 84),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(203, 36, 251, 84),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(254, 36, 302, 84),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PAttack3",
		{new Sprite(Resources::get_texture("reimu"),	glm::ivec4(262, 0, 310, 33),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PRapid_Fire", {
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(2, 130, 48, 176),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(50, 130, 96, 176),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(98, 130, 144, 176),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(146, 130, 192, 176),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PSlide", {
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(162, 0, 210, 33),	glm::ivec2(this->scr_width_, this->scr_height_)),
		new Sprite(Resources::get_texture("reimu"),		glm::ivec4(212, 0, 260, 33),	glm::ivec2(this->scr_width_, this->scr_height_))});
	Resources::load_sprites("PDead",
		{new Sprite(Resources::get_texture("reimu"),	glm::ivec4(2, 212, 32, 242),	glm::ivec2(this->scr_width_, this->scr_height_))});
}

void Game::init_objects()
{
	this->se_ = irrklang::createIrrKlangDevice();
	this->bgm_[0] = this->se_->addSoundSourceFromFile("Music/menu.mp3");
	this->bgm_[1] = this->se_->addSoundSourceFromFile("Music/0_6.mp3");
	this->bgm_[2] = this->se_->addSoundSourceFromFile("Music/1.mp3");
	this->bgm_[3] = this->se_->addSoundSourceFromFile("Music/2.mp3");
	this->bgm_[4] = this->se_->addSoundSourceFromFile("Music/3.mp3");
	this->bgm_[5] = this->se_->addSoundSourceFromFile("Music/4.mp3");
	this->bgm_[6] = this->se_->addSoundSourceFromFile("Music/5.mp3");
	this->bgm_[7] = this->se_->addSoundSourceFromFile("Music/end.mp3");

	this->sfx_[0] = this->se_->addSoundSourceFromFile("Sfx/flip.wav");
	this->sfx_[1] = this->se_->addSoundSourceFromFile("Sfx/shoot.wav");
	this->sfx_[2] = this->se_->addSoundSourceFromFile("Sfx/bullet_destr.wav");
	this->sfx_[3] = this->se_->addSoundSourceFromFile("Sfx/death.wav");
	this->sfx_[4] = this->se_->addSoundSourceFromFile("Sfx/slide.wav");
	this->sfx_[5] = this->se_->addSoundSourceFromFile("Sfx/bomb_bonus.wav");
	this->sfx_[6] = this->se_->addSoundSourceFromFile("Sfx/boom.wav");
	this->sfx_[7] = this->se_->addSoundSourceFromFile("Sfx/point_bonus.wav");

	this->menu_ = new Menu(this->scr_width_, this->scr_height_, this->k_);

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

	this->upper_bound_ =  static_cast<float32>(Resources::get_texture("interface")->height);

	this->lvl_objs_.tile_size = 32.0f;
	this->lvl_objs_.scr_width = this->scr_width_;
	this->lvl_objs_.scr_height = this->scr_height_;
	
	const auto lvls_path = std::filesystem::path(this->configs_.at("levels_dir"));
	for (const auto& entry : std::filesystem::directory_iterator(lvls_path))
		if (const auto filename = entry.path().filename(); 
			filename.extension().string() == ".lvl")
			this->lvls_list_.push_back(filename.string());

	std::sort(this->lvls_list_.begin(), this->lvls_list_.end(), [](const std::string& a, const std::string& b){ return a.back() < b.back() || a.size() < b.size(); });

	this->reimu_	= new Player(this->scr_width_, this->scr_height_);
	this->reimu_->set_speed(std::stof(this->configs_.at("player_speed")));

	this->ball_		= new Ball	(this->scr_width_, this->scr_height_);
	this->ball_->set_gravity(std::stof(this->configs_.at("ball_gravity")));
	this->ball_->set_e_loss(std::stof(this->configs_.at("energy_loss")));
	this->ball_->set_upper_bound(this->upper_bound_);
}

void Game::framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	glViewport(0, 0, width, height);
}

void Game::window_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	auto game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	delete game->render_;
	game->render_ = new RenderWindow(width, height);
}

void Game::process_input()
{
	if (this->delta_time_ >= this->max_period_)
	{
		switch (this->state_)
		{
		case Menu::GAMEPLAY:
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
				{
					this->se_->play2D(this->sfx_[4]);
					this->reimu_->set_state(Player::State::SLIDE_L);
				}
				if (!glfwGetKey(this->window_, GLFW_KEY_LEFT)
					&& glfwGetKey(this->window_, GLFW_KEY_RIGHT)
					&& this->reimu_->get_state() != Player::State::SLIDE_R_A2
					&& this->reimu_->get_state() != Player::State::SLIDE_L_A2)
				{
					this->se_->play2D(this->sfx_[4]);
					this->reimu_->set_state(Player::State::SLIDE_R);
				}

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
					this->se_->play2D(this->sfx_[4]);
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
					this->se_->play2D(this->sfx_[4]);
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
					|| this->reimu_->get_state() == Player::State::RUN_R)
				&& !this->starting_f_)
			{
				this->ball_upd_f_ = true;
				this->se_->play2D(this->sfx_[1]);
				this->player_bullets_.push_back(
					new PlayerBullet(glm::vec2(
						this->reimu_->get_pos().x,
						this->reimu_->get_pos().y + this->reimu_->get_size().y / 2.0f),
						this->scr_width_, this->scr_height_));
			}			
			this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];

			this->keys_.at(GLFW_KEY_C)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_C));
			if (this->keys_.at(GLFW_KEY_C)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_C)[0] == GLFW_RELEASE
				&& (this->reimu_->get_state() == Player::State::STAND
					|| this->reimu_->get_state() == Player::State::RUN_L
					|| this->reimu_->get_state() == Player::State::RUN_R)
				&& this->reimu_->bombs > 0
				&& !this->starting_f_)
			{
				this->se_->stopAllSoundsOfSoundSource(this->sfx_[6]);
				this->se_->play2D(this->sfx_[6]);
				for (auto tile : this->lvl_objs_.tiles)
				{
					tile->flip();
					this->score_ += tile->get_hp_change() * ((this->mult_ - 1) * (this->mult_ - 1) * 20 + 100);
					this->mult_ += tile->get_hp_change();
				}
				this->reimu_->set_state(Player::State::BOMB);
				this->reimu_->set_old_state(Player::State::BOMB);
			}
			this->keys_.at(GLFW_KEY_C)[0] = this->keys_.at(GLFW_KEY_C)[1];

			this->keys_.at(GLFW_KEY_ESCAPE)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_ESCAPE));
			if (this->keys_.at(GLFW_KEY_ESCAPE)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_ESCAPE)[0] == GLFW_RELEASE)
				this->state_ = Menu::PAUSE;
			this->keys_.at(GLFW_KEY_ESCAPE)[0] = this->keys_.at(GLFW_KEY_ESCAPE)[1];
			break;

		case Menu::PAUSE:
			this->keys_.at(GLFW_KEY_Z)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_Z));
			if (this->keys_.at(GLFW_KEY_Z)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_Z)[0] == GLFW_RELEASE)
			{
				switch (this->menu_->option)
				{
				case 0:
					this->state_ = Menu::GAMEPLAY;
					break;

				case 1:
					this->state_ = Menu::MAIN_MENU;
					this->smooth_sound_stop();
					this->se_->play2D(this->bgm_[0], true);
					this->menu_->option = 0;
					break;

				default:
					break;
				}
			}
			this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];

			this->keys_.at(GLFW_KEY_LEFT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_LEFT));
			if (this->keys_.at(GLFW_KEY_LEFT)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_LEFT)[0] == GLFW_RELEASE)
				this->menu_->move_up();
			this->keys_.at(GLFW_KEY_LEFT)[0] = this->keys_.at(GLFW_KEY_LEFT)[1];

			this->keys_.at(GLFW_KEY_RIGHT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_RIGHT));
			if (this->keys_.at(GLFW_KEY_RIGHT)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_RIGHT)[0] == GLFW_RELEASE)
				this->menu_->move_down();
			this->keys_.at(GLFW_KEY_RIGHT)[0] = this->keys_.at(GLFW_KEY_RIGHT)[1];

			this->keys_.at(GLFW_KEY_ESCAPE)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_ESCAPE));
			if (this->keys_.at(GLFW_KEY_ESCAPE)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_ESCAPE)[0] == GLFW_RELEASE)
				this->state_ = Menu::GAMEPLAY;
			this->keys_.at(GLFW_KEY_ESCAPE)[0] = this->keys_.at(GLFW_KEY_ESCAPE)[1];
			break;

		case Menu::GAME_OVER:
			this->keys_.at(GLFW_KEY_Z)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_Z));
			if (this->keys_.at(GLFW_KEY_Z)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_Z)[0] == GLFW_RELEASE)
			{
				this->state_ = Menu::MAIN_MENU;
				this->smooth_sound_stop();
				this->se_->play2D(this->bgm_[0], true);
			}
			this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];
			break;
		
		case Menu::LVL_SWITCH:
			this->keys_.at(GLFW_KEY_Z)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_Z));
			if (this->keys_.at(GLFW_KEY_Z)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_Z)[0] == GLFW_RELEASE)
			{
				this->point_lim_ = 3;
				const auto temp_stage = this->stage_;
				this->stage_++;
				if (temp_stage % 5 + 1 >= this->stage_ % 5)
					this->tile_val_ += 100;

				this->score_ += this->level_score_;
				this->hi_score_ = (this->score_ >= this->hi_score_) * this->score_
								+ (this->score_ < this->hi_score_) * this->hi_score_;
				if (this->stage_ <= this->lvls_list_.size())
				{
					Level_manager::load(this->lvl_objs_, (this->configs_.at("levels_dir") + this->lvls_list_[this->stage_ - 1]).c_str());
					this->state_ = Menu::GAMEPLAY;
					this->clear_stage();
					this->write_save();
					if (this->bg_temp_ != this->lvl_objs_.bg_num)
					{
						this->smooth_sound_stop();
						this->se_->play2D(this->bgm_[this->lvl_objs_.bg_num % 6 + 1]);
						this->bg_temp_ = this->lvl_objs_.bg_num;
					}
				}			
				else
				{
					this->state_ = Menu::ENDING;
					this->smooth_sound_stop();
					this->se_->play2D(this->bgm_[7], true);
				}
			}
			this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];
			break;

		case Menu::ENDING:
			this->keys_.at(GLFW_KEY_Z)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_Z));
			if (this->keys_.at(GLFW_KEY_Z)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_Z)[0] == GLFW_RELEASE)
			{
				this->state_ = Menu::MAIN_MENU;
				this->smooth_sound_stop();
				this->se_->play2D(this->bgm_[0], true);
			}
			this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];
			break;
		
		case Menu::MAIN_MENU:
			this->keys_.at(GLFW_KEY_Z)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_Z));
			if (this->keys_.at(GLFW_KEY_Z)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_Z)[0] == GLFW_RELEASE)
			{
				switch (this->menu_->option)
				{
				case 0:
					this->point_lim_ = 3;
					this->state_ = Menu::GAMEPLAY;
					this->stage_ = 1;
					Level_manager::load(this->lvl_objs_, (this->configs_.at("levels_dir") + this->lvls_list_[this->stage_ - 1]).c_str());
					this->smooth_sound_stop();
					this->se_->play2D(this->bgm_[this->lvl_objs_.bg_num % 6 + 1], true);
					try
					{
						std::ifstream ifile("save.dat", std::ios::binary);
						ifile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
						ifile.read(reinterpret_cast<char*>(&this->save_), sizeof this->save_);
						ifile.close();

						this->hi_score_		= this->save_.hi_score;
					}
					catch (std::ifstream::failure&)
					{}
					this->bg_temp_ = this->lvl_objs_.bg_num;
					this->reset();
					this->set_difficulty();
					break;

				case 1:
					this->state_ = Menu::GAMEPLAY;
					this->read_save();
					this->tile_val_ = (this->stage_ / 4) * (this->stage_ % 4 > 0) + 100;
					this->bg_temp_ = this->lvl_objs_.bg_num;
					this->smooth_sound_stop();
					this->se_->play2D(this->bgm_[this->lvl_objs_.bg_num % 6 + 1], true);
					this->ball_upd_f_ = false;
					this->set_difficulty();
					break;

				case 2:
					this->menu_->option = 0;
					this->state_ = Menu::OPTIONS;
					break;

				case 3:
					this->runtime_flag_ = false;
					glfwSetWindowShouldClose(this->window_, true);
					break;

				default:
					break;
				}
			}
			this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];

			this->keys_.at(GLFW_KEY_UP)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_UP));
			if (this->keys_.at(GLFW_KEY_UP)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_UP)[0] == GLFW_RELEASE)
				this->menu_->move_up();
			this->keys_.at(GLFW_KEY_UP)[0] = this->keys_.at(GLFW_KEY_UP)[1];

			this->keys_.at(GLFW_KEY_DOWN)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_DOWN));
			if (this->keys_.at(GLFW_KEY_DOWN)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_DOWN)[0] == GLFW_RELEASE)
				this->menu_->move_down();
			this->keys_.at(GLFW_KEY_DOWN)[0] = this->keys_.at(GLFW_KEY_DOWN)[1];
			break;

		case Menu::OPTIONS:
			switch (this->menu_->option)
			{
			case 0:
				this->keys_.at(GLFW_KEY_LEFT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_LEFT));
				if (this->keys_.at(GLFW_KEY_LEFT)[1] == GLFW_PRESS 
					&& this->keys_.at(GLFW_KEY_LEFT)[0] == GLFW_RELEASE)
					this->configs_.at("screen_res_mult") = to_str(std::stof(this->configs_.at("screen_res_mult")) - 0.05f, 2);
				this->keys_.at(GLFW_KEY_LEFT)[0] = this->keys_.at(GLFW_KEY_LEFT)[1];

				this->keys_.at(GLFW_KEY_RIGHT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_RIGHT));
				if (this->keys_.at(GLFW_KEY_RIGHT)[1] == GLFW_PRESS 
					&& this->keys_.at(GLFW_KEY_RIGHT)[0] == GLFW_RELEASE)
					this->configs_.at("screen_res_mult") = to_str(std::stof(this->configs_.at("screen_res_mult")) + 0.05f, 2);
				this->keys_.at(GLFW_KEY_RIGHT)[0] = this->keys_.at(GLFW_KEY_RIGHT)[1];
				glfwSetWindowSize(this->window_, static_cast<int32>(static_cast<float32>(this->scr_width_) * std::stof(this->configs_.at("screen_res_mult"))), 
					static_cast<int32>(static_cast<float32>(this->scr_height_) * std::stof(this->configs_.at("screen_res_mult"))));
				break;
			case 1:
				this->keys_.at(GLFW_KEY_LEFT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_LEFT));
				if (this->keys_.at(GLFW_KEY_LEFT)[1] == GLFW_PRESS 
					&& this->keys_.at(GLFW_KEY_LEFT)[0] == GLFW_RELEASE)
					this->configs_.at("volume") = to_str(std::stof(this->configs_.at("volume")) - 0.05f, 2);
				this->keys_.at(GLFW_KEY_LEFT)[0] = this->keys_.at(GLFW_KEY_LEFT)[1];

				this->keys_.at(GLFW_KEY_RIGHT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_RIGHT));
				if (this->keys_.at(GLFW_KEY_RIGHT)[1] == GLFW_PRESS 
					&& this->keys_.at(GLFW_KEY_RIGHT)[0] == GLFW_RELEASE)
					this->configs_.at("volume") = to_str(std::stof(this->configs_.at("volume")) + 0.05f, 2);
				this->keys_.at(GLFW_KEY_RIGHT)[0] = this->keys_.at(GLFW_KEY_RIGHT)[1];

				this->configs_.at("volume") = to_str(std::clamp(std::stof(this->configs_.at("volume")), 0.0f, 1.0f), 2);
				this->se_->setSoundVolume(std::stof(this->configs_.at("volume")));
				for (auto sound : this->sfx_)
					sound->setDefaultVolume(std::clamp(std::stof(this->configs_.at("volume")) - 0.04f, 0.05f, 1.0f));
				break;

			case 2:
				this->keys_.at(GLFW_KEY_LEFT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_LEFT));
				if (this->keys_.at(GLFW_KEY_LEFT)[1] == GLFW_PRESS 
					&& this->keys_.at(GLFW_KEY_LEFT)[0] == GLFW_RELEASE)
					this->configs_.at("difficulty") = std::to_string(std::stoi(this->configs_.at("difficulty")) - 1);
				this->keys_.at(GLFW_KEY_LEFT)[0] = this->keys_.at(GLFW_KEY_LEFT)[1];

				this->keys_.at(GLFW_KEY_RIGHT)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_RIGHT));
				if (this->keys_.at(GLFW_KEY_RIGHT)[1] == GLFW_PRESS 
					&& this->keys_.at(GLFW_KEY_RIGHT)[0] == GLFW_RELEASE)
					this->configs_.at("difficulty") = std::to_string(std::stoi(this->configs_.at("difficulty")) + 1);
				this->keys_.at(GLFW_KEY_RIGHT)[0] = this->keys_.at(GLFW_KEY_RIGHT)[1];

				this->configs_.at("difficulty") = std::to_string(static_cast<uint8>(std::stoi(this->configs_.at("difficulty"))) % 4);
				break;

			default:
				break;
			}
			this->keys_.at(GLFW_KEY_Z)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_Z));
			if (this->keys_.at(GLFW_KEY_Z)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_Z)[0] == GLFW_RELEASE)
			{
				switch (this->menu_->option)
				{
				case 3:
					this->menu_->option = 0;
					this->state_ = Menu::MAIN_MENU;
					this->write_config();
					break;

				default:
					break;
				}
			}
			this->keys_.at(GLFW_KEY_Z)[0] = this->keys_.at(GLFW_KEY_Z)[1];

			this->keys_.at(GLFW_KEY_UP)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_UP));
			if (this->keys_.at(GLFW_KEY_UP)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_UP)[0] == GLFW_RELEASE)
				this->menu_->move_up();
			this->keys_.at(GLFW_KEY_UP)[0] = this->keys_.at(GLFW_KEY_UP)[1];

			this->keys_.at(GLFW_KEY_DOWN)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_DOWN));
			if (this->keys_.at(GLFW_KEY_DOWN)[1] == GLFW_PRESS 
				&& this->keys_.at(GLFW_KEY_DOWN)[0] == GLFW_RELEASE)
				this->menu_->move_down();
			this->keys_.at(GLFW_KEY_DOWN)[0] = this->keys_.at(GLFW_KEY_DOWN)[1];
			break;
		}

		this->keys_.at(GLFW_KEY_F1)[1] = static_cast<uint8>(glfwGetKey(this->window_, GLFW_KEY_F1));
		if (this->keys_.at(GLFW_KEY_F1)[1] == GLFW_PRESS 
			&& this->keys_.at(GLFW_KEY_F1)[0] == GLFW_RELEASE)
		{
			this->polygon_flag_ = !this->polygon_flag_;
		}
		this->keys_.at(GLFW_KEY_F1)[0] = this->keys_.at(GLFW_KEY_F1)[1];
	}
}
