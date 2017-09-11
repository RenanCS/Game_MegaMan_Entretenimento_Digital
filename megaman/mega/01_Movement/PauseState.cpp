/*
 *  PlayState.cpp
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#include <iostream>
#include <cmath>
#include "Game.h"
#include "PauseState.h"
#include "InputManager.h"

PauseState PauseState::m_PauseState;

using namespace std;

void PauseState::init()
{

    //playSprite3.load("data/img/Char01.png");
	//playSprite3.setPosition(50,300);

     //GERENCIADOR DE ENTRADA
    im = cgf::InputManager::instance();

    im->addKeyInput("resume", sf::Keyboard::Return);

	cout << "PauseState: Init" << endl;
}

void PauseState::cleanup()
{
	cout << "PauseState: Clean" << endl;
}

void PauseState::pause()
{
	cout << "PauseState: Paused" << endl;
}

void PauseState::resume()
{
	cout << "PauseState: Resumed" << endl;
}

void PauseState::handleEvents(cgf::Game* game)
{
    sf::Event event;

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
    }

    if(im->testEvent("resume"))
        game->popState();



}

void PauseState::update(cgf::Game* game)
{
}

void PauseState::draw(cgf::Game* game)
{
    screen = game->getScreen();
}
