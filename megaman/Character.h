#ifndef CHARACTER_H
#define CHARACTER_H

#include "Sprite.h"
#include "GameState.h"
#include <tmx/MapLoader.h>
#include <string>

#define DIR_LEFT -1
#define DIR_RIGHT 1
#define DIR_UP -1
#define DIR_DOWN 1

class Character
{
    public:
        Character();

        virtual ~Character();

        virtual void init(int curr_dir_x, int state, int energy_p) {}

        virtual void draw(sf::RenderWindow *screen) {}

        virtual void handleEvents() {}

        virtual void checkCollision(uint8_t layer, cgf::Game* game, tmx::MapLoader *map) {}


        cgf::Sprite* getSprite();

        void setCurrDirX(int dir_x_p);
        int getCurrDirX();

        void setNewDirX(int dir_x_p);
        int getNewDirX();

        void setCurrDirY(int dir_y_p);
        int getCurrDirY();

        void setNewDirY(int dir_y_p);
        int getNewDirY();

        void setCurrState(int state_p);
        int getCurrState();

        void setNewState(int state_p);
        int getNewState();

        void updateEnergy(int energy_p);


};

#endif // CHARACTER_H
