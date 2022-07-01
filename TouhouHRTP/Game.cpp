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

	for (auto val : this->textures_)
		delete val.second;

	delete this->shader_;
	delete this->render_;
	delete this->screen_;

	glfwTerminate();
}

void Game::update_ball()
{
	glm::vec2 pushDir = glm::normalize(this->ball_->get_pos() - this->reimu_->get_pos());
	if (this->ball_->check_intersect(*this->reimu_))
	{
		//std::cout << "spotted\n";
		this->ball_->set_vel(600.0f * pushDir.x, 700.0f);
		//ball->setVel(200.0f * pushDir.x, abs(200.0f * ((int)(pushDir.y * 30.0f) % 3 + 1)) + 10.0f);
	}

	this->ball_->update(this->delta_time_);
}

void Game::update_player	()
{
	reimu_->update(this->delta_time_);
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

	// process_input(this->window_);
	this->render_->clear();

	this->text_->set_pos(100.0f * cosf(2.0f * currentFrame) + 700.0f, 
						100.0f * sinf(2.0f * currentFrame) + 400.0f);
	this->text_->set_color((sinf(currentFrame) + 2.0f) / 2.0f, 
						(cosf(currentFrame) + 2.0f) / 2.0f, 
						(sinf(currentFrame) + 2.0f) / 2.0f);

	for (auto tile : tiles_)
	{
		if (tile->check_intersect(*this->ball_))
			tile->flip();
		tile->update(this->delta_time_);

		auto it = std::remove_if(
			this->tiles_.begin(),
			this->tiles_.end(),
			[](Tile* tile) { return tile->get_state() == Tile::State::DEAD; });
		this->tiles_.erase(it, this->tiles_.end());
		if (tile->get_state() == Tile::State::DEAD)
			delete tile;
	}

	this->update_player();
	this->update_ball();
}

void Game::display()
{
	for (auto tile : tiles_)
		tile->draw(this->shader_);
	reimu_->draw(this->shader_);
	ball_->draw(this->shader_);
	text_->draw();

	/*==============================*/

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

	this->screen_ = new Shader(glsl::vScreen, glsl::fScreen, nullptr, true);
	this->render_ = new RenderWindow(*this->screen_, this->win_width_, this->win_height_);
}

void Game::init_textures()
{
	this->font_		= new Font("Fonts/Heuristica-Regular.ttf");
	this->shader_	= new Shader(glsl::vSprite, glsl::fSprite, nullptr, true);

	this->textures_["Player"] = new Texture;
	this->textures_.at("Player")->texFromBin("Textures/reimu.dat");

	this->textures_["Tiles"] = new Texture;
	this->textures_.at("Tiles")->texFromBin("Textures/tiles.dat");
}

void Game::init_objects()
{
	this->text_		= new Text	(this->font_, this->win_width_, this->win_height_);
	this->reimu_	= new Player(this->textures_.at("Player"), this->scr_width_, this->scr_height_);
	this->ball_		= new Ball	(this->textures_.at("Tiles"), this->scr_width_, this->scr_height_);
	for (int32 i = 0; i < 10; ++i)
		for (int32 j = 0; j < 10; ++j)
		{
			const int32 v = i * 10 + j;
			this->tiles_.push_back(new Tile(this->textures_.at("Tiles"), this->scr_width_, this->scr_height_, Tile::State::PHASE3));
			this->tiles_[v]->set_pos(
				2 * this->tiles_[v]->get_size().x * static_cast<float32>(i + 1), 
				2 * this->tiles_[v]->get_size().y * static_cast<float32>(j + 1));
		}

	this->text_->set_text("text");
	this->text_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, static_cast<float32>(this->scr_height_) / 2.0f);
	this->reimu_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, this->reimu_->get_size().y);
	this->ball_->set_pos(static_cast<float32>(this->scr_width_) / 2.0f, static_cast<float32>(this->scr_height_) - 30.0f);
}

void Game::framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	glViewport(0, 0, width, height);
}

void Game::process_input()
{
	if (!(glfwGetKey(window_, GLFW_KEY_RIGHT) || glfwGetKey(window_, GLFW_KEY_LEFT)))
		this->reimu_->stand();

	else if (glfwGetKey(this->window_, GLFW_KEY_LEFT) && !glfwGetKey(window_, GLFW_KEY_X))
		this->reimu_->run(this->delta_time_, -1);

	else if (glfwGetKey(this->window_, GLFW_KEY_RIGHT) && !glfwGetKey(window_, GLFW_KEY_X))
		this->reimu_->run(this->delta_time_, 1);

	if (glfwGetKey(window_, GLFW_KEY_X) && glfwGetKey(window_, GLFW_KEY_LEFT))
		this->reimu_->slide(this->delta_time_, -1);

	else if (glfwGetKey(window_, GLFW_KEY_X) && glfwGetKey(window_, GLFW_KEY_RIGHT))
		this->reimu_->slide(this->delta_time_, 1);

	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL * this->polygon_flag_ + GL_LINE * !this->polygon_flag_);
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