
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

//#include <sdl\SDL.h>
//#undef main

#include "SDLManager.h"
#include "Game.h"

using namespace std;


int main(int argc, char **argv){

	try{
		SDL::Init("Civ", 50, 50, 800, 600);
	}
	catch (const SDLException& e){
		cerr << e.what() << endl;
		return 1;
	}

	auto game = make_unique<Game>();

	return game->Mainloop();
}
