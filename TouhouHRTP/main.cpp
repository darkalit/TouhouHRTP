#include "Game.h"

int main(int argc, char** argv)
{
	auto* game = new Game;
	
	while (game->is_running())
	{
		game->process_input();
		game->update();
		game->display();
	}

	delete game;

	return 0;
}

int WinMain(int argc, char** argv) 
{
	main(argc, argv);
}
