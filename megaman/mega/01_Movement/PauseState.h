/*
 *  PlayState.h
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#ifndef PAUSE_STATE_H_
#define PAUSE_STATE_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"

class PauseState : public cgf::GameState
{
    public:

    void init();
    void cleanup();

    void pause();
    void resume();

    void handleEvents(cgf::Game* game);
    void update(cgf::Game* game);
    void draw(cgf::Game* game);
    void InitText();

    // Implement Singleton Pattern
    static PauseState* instance()
    {
        return &m_PauseState;
    }

    protected:

    PauseState() {}

    private:

    static PauseState m_PauseState;
    sf::RenderWindow* screen;
    cgf::InputManager* im;
    cgf::Sprite background;
    sf::Font font;
    sf::Text scoreText;
};

#endif
