/*
 *  ConfigurationState.cpp
 *  Example "menu" state
 *
 *  Created by Marcelo Cohen on 04/11.
 *  Copyright 2011 PUCRS. All rights reserved.
 *
 */


#include <iostream>
#include "Game.h"
#include "InputManager.h"
#include "MenuState.h"
#include "PlayState.h"
#include "ConfigurationState.h"

ConfigurationState ConfigurationState::m_ConfigurationState;

using namespace std;

void ConfigurationState::init()
{
    configurationSprite.load("data/img/pccontrol.png"); // load menu state bitmap
    cout << "MenuState Init Successful" << endl;

    im = cgf::InputManager::instance();
    im->addKeyInput("resume", sf::Keyboard::Return);
}

void ConfigurationState::cleanup()
{
    cout << "MenuState Cleanup Successful" << endl;
}

void ConfigurationState::pause()
{
    cout << "MenuState Paused" << endl;
}

void ConfigurationState::resume()
{
    cout << "MenuState Resumed" << endl;
}

void ConfigurationState::handleEvents(cgf::Game* game)
{
    sf::Event event;
    sf::RenderWindow* screen = game->getScreen();

    while (screen->pollEvent(event))
    {
        // check the type of the event...
        switch (event.type)
        {
            // window closed
        case sf::Event::Closed:
            game->quit();
            break;

            // key pressed
        case sf::Event::KeyPressed:

            if(event.key.code == sf::Keyboard::Space)
                game->changeState(PlayState::instance());

            if(event.key.code == sf::Keyboard::Escape)
                game->quit();

            if(im->testEvent("resume"))
                game->popState();

            break;

            // we don't process other types of events
        default:
            break;
        }
    }
}

void ConfigurationState::update(cgf::Game* game)
{
}

void ConfigurationState::draw(cgf::Game *game)
{
    configurationSprite.setPosition(130,25);

//    configuratopSprites->draw(game->getScreen());

    game->getScreen()->draw(configurationSprite);
}

