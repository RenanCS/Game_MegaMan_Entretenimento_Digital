#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>

#include "Game.h"
#include "PlayState.h"

using namespace std;

int main()
{
    cgf::Game game(5, 30);
    game.init("Megaman", 800, 600, false);

    game.changeState(PlayState::instance());

    while(game.isRunning()) {
        game.handleEvents();
        game.update();
        game.draw();
    }
    return 0;
}
