/*
 *  MenuState.cpp
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
#include "SFML/Audio.hpp"

MenuState MenuState::m_MenuState;

using namespace std;

void MenuState::init()
{
    music.openFromFile("data/audio/StageSelect.wav");
    music.setVolume(50);
    music.play();
    music.setLoop(true);


    menuSprite.load("data/img/capa.png"); // load menu state bitmap
    cout << "MenuState Init Successful" << endl;
}

void MenuState::cleanup()
{
    cout << "MenuState Cleanup Successful" << endl;
}

void MenuState::pause()
{
    cout << "MenuState Paused" << endl;
}

void MenuState::resume()
{
    cout << "MenuState Resumed" << endl;
}

void MenuState::handleEvents(cgf::Game* game)
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
            if(event.key.code == sf::Keyboard::Return){
                music.stop();
                game->changeState(PlayState::instance());
            }

            if(event.key.code == sf::Keyboard::Escape)
                game->quit();

            if(event.key.code == sf::Keyboard::F1)
                game->pushState(ConfigurationState::instance());

            //game->changeState(PlayMap::instance());
            //game->changeState(PlayMapTop::instance());
            //game->changeState(PlayMapAI::instance());
            //game->changeState(PlayPhysics::instance());
            //game->changeState(PlayMapPhysics::instance());
            break;

            // we don't process other types of events
        default:
            break;
        }
    }
}

void MenuState::update(cgf::Game* game)
{
}

void MenuState::draw(cgf::Game *game)
{
    menuSprite.setPosition(1,1);
//    menuSprite->draw(game->getScreen());
    game->getScreen()->draw(menuSprite);
}

