#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "GameState.h"
#include "Sprite.h"
#include "Character.h"
#include "Player.h"
#include <tmx/MapLoader.h>
#include <string>


class PlayState : public cgf::GameState
{
    public:

    void init();

    void cleanup();

    void pause();

    void resume();

    void handleEvents(cgf::Game* game);

    void update(cgf::Game* game);

    void draw(cgf::Game* game);

    void centerMapOnPlayer();

    sf::Vector2f getPan();

    sf::Uint16 getCellFromMap(uint8_t layernum, float x, float y);

    static PlayState* instance() {
        return &m_PlayState;
    }

    protected:

    PlayState() {}

    private:

    static PlayState m_PlayState;

    sf::RenderWindow* screen;

    cgf::InputManager* im;

    float panX, panY;

    tmx::MapLoader* map;

    Character *char_list[100];
};

#endif // PLAYSTATE_H
