#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "PlayState.h"
#include <string>

class Player : public Character
{
    public:
        Player();

        virtual ~Player();

        void init(int curr_dir_p, int state_p, int energy_p);

        void handleEvents();

        void draw(sf::RenderWindow *screen);

        void checkCollision(uint8_t layer, cgf::Game* game, tmx::MapLoader *map);

        void checkCollision2(uint8_t layer, cgf::Game* game, tmx::MapLoader *map);

};

#endif // PLAYER_H
