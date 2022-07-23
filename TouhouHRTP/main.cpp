#include "Game.h"

auto main(int argc, char** argv) -> int32
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

auto WinMain(int argc, char** argv) -> int32
{
	main(argc, argv);
}
