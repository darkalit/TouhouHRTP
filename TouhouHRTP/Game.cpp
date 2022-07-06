#include "Game.h"

Game::Game()
{
	this->init_window();
	this->init_textures();
	this->init_objects();
}

Game::~Game()
{
	delete this->reimu_;
	delete this->ball_;
	delete this->text_;
	delete this->font_;
	for (auto tile : this->tiles_)
		delete tile;

	Resources::clear();
	delete this->render_;

	glfwTerminate();
}

void Game::update_ball()
{
	glm::vec2 pushDir = glm::normalize(this->ball_->get_pos() - this->reimu_->get_pos());
	if (this->ball_->check_intersect(*this->reimu_) 
		&& this->reimu_->get_state() != Player::State::DEAD
		&& !this->reimu_->get_invis()
		|| (this->ball_->check_intersect(*this->reimu_)
			&& this->reimu_->get_state() != Player::State::DEAD
			&& this->reimu_->get_state() != Player::State::STAND
			&& this->reimu_->get_state() != Player::State::RUN_L
			&& this->reimu_->get_state() != Player::State::RUN_R))
	{
		this->ball_->set_vel(600.0f * pushDir.x, 700.0f);
		//ball->setVel(200.0f * pushDir.x, abs(200.0f * ((int)(pushDir.y * 30.0f) % 3 + 1)) + 10.0f);
	}

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

		if (static_cast<uint32>(bullet->get_pos().y) > this->scr_height_ || bullet->check_intersect(*this->ball_))
		{
			auto it = std::remove(
				this->player_bullets_.begin(),
				this->player_bullets_.end(), 
				bullet);
			this->player_bullets_.erase(it, this->player_bullets_.end());
			delete bullet;
		}
	}

	if (this->ball_->check_intersect(*this->reimu_) 
		&& this->reimu_->get_state() != Player::State::DEAD
		&& !this->reimu_->get_invis())
	{
		this->reimu_->set_state(Player::State::DEAD);
	}
	this->reimu_->update(this->delta_time_);
}

void Game::update_tiles()
{
	for (auto tile : this->tiles_)
	{
		if (tile->get_state() != Tile::State::DEAD)
		{
			if (tile->check_intersect(*this->ball_))
				tile->flip();
			tile->update(this->delta_time_);
		}
		//auto it = std::remove_if(
		//	this->tiles_.begin(),
		//	this->tiles_.end(),
		//	[](Tile* tile) { return tile->get_state() == Tile::State::DEAD; });
		//this->tiles_.erase(it, this->tiles_.end());
	}
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

	this->text_->set_pos(100.0f * cosf(2.0f * currentFrame) + 700.0f, 
						100.0f * sinf(2.0f * currentFrame) + 400.0f);
	this->text_->set_color((sinf(currentFrame) + 2.0f) / 2.0f, 
						(cosf(currentFrame) + 2.0f) / 2.0f, 
						(sinf(currentFrame) + 2.0f) / 2.0f);

	//Resources::get_shader("flash")->use();
	//Resources::get_shader("flash")->setFloat("time", this->temp_t_);
	//this->temp_t_ += this->delta_time_ * 2.0f;
	//this->temp_t_ = (this->temp_t_ > 2.0f) * -2.0f + this->temp_t_ * (this->temp_t_ < 2.0f);

	this->update_tiles();
	this->update_player();
	this->update_ball();
}

void Game::display()
{
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL * this->polygon_flag_ + GL_LINE * !this->polygon_flag_);

	for (auto tile : this->tiles_)
		tile->draw();
	for (auto bullet : this->player_bullets_)
		bullet->draw();
	this->reimu_->draw();
	this->ball_->draw();
	this->text_->draw();

	//this->flash_->draw(Resources::get_shader("flash"));

	/*==============================*/

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

void Game::init_textures()
{
	this->font_	= new Font("Fonts/Heuristica-Regular.ttf");

	Resources::load_shader("text", "Shaders/vText.glsl", "Shaders/fText.glsl");
	Resources::load_shader("sparkle", "Shaders/vSparkle.glsl", "Shaders/fSparkle.glsl");
	Resources::load_shader("sprite", "Shaders/vSprite.glsl", "Shaders/fSprite.glsl");
	Resources::load_shader("screen", "Shaders/vScreen.glsl", "Shaders/fScreen.glsl");
	//Resources::load_shader("flash", "Shaders/vFlash.glsl", "Shaders/fFlash.glsl");

	Resources::load_texture("reimu", "Textures/reimu.dat");
	Resources::load_texture("tiles", "Textures/tiles.dat");
}

void Game::init_objects()
{
	for (uint16 i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i)
		this->keys_[i][0] = this->keys_[i][1] = GLFW_RELEASE;

	this->text_		= new Text	(this->font_, this->win_width_, this->win_height_);
	this->reimu_	= new Player(this->scr_width_, this->scr_height_);
	this->ball_		= new Ball	(this->scr_width_, this->scr_height_);
	for (int32 i = 0; i < 10; ++i)
		for (int32 j = 0; j < 10; ++j)
		{
			const int32 v = i * 10 + j;
			this->tiles_.push_back(new Tile(this->scr_width_, this->scr_height_, Tile::State::PHASE3));
			this->tiles_[v]->set_pos(
				2 * this->tiles_[v]->get_size().x * static_cast<float32>(i + 1), 
				2 * this->tiles_[v]->get_size().y * static_cast<float32>(j + 1));
		}

	this->text_->set_text("text");
	this->text_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, static_cast<float32>(this->scr_height_) / 2.0f);
	this->reimu_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, this->reimu_->get_size().y);
	this->ball_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, static_cast<float32>(this->scr_height_) - 30.0f);
	
	//this->flash_ = new Rectangle(glm::ivec2(60, 60), glm::ivec2(this->scr_width_, this->scr_height_));
	//this->flash_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, static_cast<float32>(this->scr_height_) / 2.0f);
}

void Game::framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	glViewport(0, 0, width, height);
}

void Game::process_input()
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