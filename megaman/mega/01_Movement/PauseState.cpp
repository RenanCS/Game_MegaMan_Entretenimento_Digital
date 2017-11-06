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

     //INFORMAÇÕES DE CONTROL
    InitText();

    //GERENCIADOR DE ENTRADA
    im = cgf::InputManager::instance();
    im->addKeyInput("resume", sf::Keyboard::Return);
    im->addKeyInput("quit", sf::Keyboard::Escape);

    //LOAD BACKGROUND
    //background.load("data/img/imgpause.png");

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

    if(im->testEvent("quit"))
        game->quit();
    if(im->testEvent("resume"))
        game->popState();



}

void PauseState::update(cgf::Game* game)
{
    std::stringstream str;
    str << "PAUSE GAME \n [ENTER] TO RETURN \n [ESC] TO QUIT";
    scoreText.setString(str.str());
}

void PauseState::draw(cgf::Game* game)
{
    screen = game->getScreen();

    // set text position
    screen->draw(scoreText);
    sf::View view = screen->getView();
    scoreText.setPosition(view.getCenter().x - 100, view.getCenter().y - 100);

    //Set image background
    //background.setPosition(1,1);
    //game->getScreen()->draw(background);

}

void PauseState::InitText(){
    if (!font.loadFromFile("data/fonts/Altebro.ttf"))
    {
        cout << "Cannot load arial.ttf font!" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    scoreText.setString("PAUSE GAME\n[ENTER] TO RETURN \n[ESC] TO QUIT");
    scoreText.setCharacterSize(48); // in pixels
    scoreText.setColor(sf::Color::White);
    scoreText.setStyle(sf::Text::Bold);
}

