//#include <custom/Sprite.h>
//#include <custom/Shaders.h>
#include "Objects/Player.h"
#include "Objects/Ball.h"
#include "Objects/Tile.h"
#include <stb_image.h>

void framebufferSizeCallback(GLFWwindow*, int32, int32);
void processInput(GLFWwindow*);
void keyCallback(GLFWwindow*, int32, int32, int32, int32);

// screen sizes
const uint32 WIN_WIDTH = 1440;
const uint32 WIN_HEIGHT = 900;

const uint32 SCR_WIDTH = 640;
const uint32 SCR_HEIGHT = 400;

const char* TITLE = "TOUHOU";

// frametime vars
float32 deltaTime = 0.0f;
float32 lastFrame = 0.0f;
float32 lastTime = 0.0f;
int32 frameCount = 0;

bool polygonFlag = true;

float32 pixelSize = 1.0f;

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OS X
#endif // __APPLE__

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load GLAD" << std::endl;
	}

	//glfwSetKeyCallback(window, keyCallback);

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader Screen(glsl::vScreen, glsl::fScreen);
	RenderWindow Render(Screen, WIN_WIDTH, WIN_HEIGHT);

	Player* Reimu = new Player(window, SCR_WIDTH, SCR_HEIGHT);
	Ball* ball = new Ball(SCR_WIDTH, SCR_HEIGHT);
	std::vector<Tile*> Tiles;
	for (uint32 i = 0; i < 10; ++i)
	{
		for (uint32 j = 0; j < 10; ++j)
		{
			int32 v = i * 10 + j;
			Tiles.push_back(new Tile(SCR_WIDTH, SCR_HEIGHT, 1));
			Tiles.at(v)->setPos(2 * Tiles.at(v)->getSize().x * (i + 1), 2 * Tiles.at(v)->getSize().y * (j + 1));
		}
	}

	//Tiles.at(0)->setPos(SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);
	Reimu->setPos(SCR_WIDTH / 2.0f, Reimu->getSize().y);
	ball->setPos(SCR_WIDTH / 2.0f, SCR_HEIGHT - 30.0f);
	//glfwSetKeyCallback(window, &Reimu->updateInput);
	while (!glfwWindowShouldClose(window))
	{
		float32 currentFrame = static_cast<float32>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		Render.clear();			
		/*==============================*/

		glm::vec2 pushDir = glm::normalize(ball->getPos() - Reimu->getPos());
		if (Reimu->checkIntersect(*ball))
		{
			std::cout << "spotted\n";
			ball->setVel(600.0f * pushDir.x, 700.0f);
			//ball->setVel(200.0f * pushDir.x, abs(200.0f * ((int)(pushDir.y * 30.0f) % 3 + 1)) + 10.0f);
		}
		
		//std::cout << pushDir.x << ", " << pushDir.y << std::endl;
		//std::cout << abs(200.0f * ((int)(pushDir.y * 30.0f) % 3 + 1)) + 10.0f << std::endl;

		for (unsigned int i = 0; i < Tiles.size(); ++i)
		{
			if (Tiles.at(i)->checkIntersect(*ball))
				Tiles.at(i)->flip();
			Tiles.at(i)->update(deltaTime);
			if (Tiles.at(i)->getFlip())
				Tiles.erase(Tiles.begin() + i);
		}
		Reimu->update(deltaTime);
		ball->update(deltaTime);
		
		for (auto& tile : Tiles)
			tile->draw();
		Reimu->draw();
		ball->draw();
		
		/*==============================*/
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		Render.display();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//glfwTerminate();
	return 0;
}

int WinMain(int argc, char** argv) {
	main(argc, argv);
}

void framebufferSizeCallback(GLFWwindow* window, int32 width, int32 height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (polygonFlag)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
	{

	}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void keyCallback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
{
	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
		polygonFlag = !polygonFlag;
}