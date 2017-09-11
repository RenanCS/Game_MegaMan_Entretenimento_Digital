/*
 *  MenuState.h
 *  Example "menu" state
 *
 *  Created by Marcelo Cohen on 04/11.
 *  Copyright 2011 PUCRS. All rights reserved.
 *
 */

#ifndef _CONFIGURATION_STATE_H_
#define _CONFIGURATION_STATE_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"

class ConfigurationState : public cgf::GameState
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
    static ConfigurationState* instance()
    {
        return &m_ConfigurationState;
    }

    protected:

    ConfigurationState() {}

    private:

    static ConfigurationState m_ConfigurationState;

    cgf::Sprite configurationSprite;
    cgf::InputManager* im;

};

#endif

