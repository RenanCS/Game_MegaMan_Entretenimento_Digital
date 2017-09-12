/*
 *  PlayState.h
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#ifndef PLAY_STATE_H_
#define PLAY_STATE_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <tmx/MapLoader.h>

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

    // Implement Singleton Pattern
    static PlayState* instance()
    {
        return &m_PlayState;
    }

    protected:

    PlayState() {}

    private:

    static PlayState m_PlayState;

    void centerMapOnPlayer();

    int dirx, diry, last, jumpCount, posx,posy;
    bool shooting, jumping;
    cgf::Sprite playSprite1;
    sf::RenderWindow* screen;
    cgf::InputManager* im;
    tmx::MapLoader* map;
};

#endif
