#include <iostream>
#include <cmath>
#include "Game.h"
#include "TitleState.h"
#include "PlayState.h"
#include "InputManager.h"

TitleState TitleState::m_TitleState;

using namespace std;

void TitleState::init() {


    titulo.setString("MEGAMAN");
    titulo.setFillColor(sf::Color::Blue);

}

void TitleState::cleanup() {}
void TitleState::pause() {}
void TitleState::resume() {}

void TitleState::handleEvents(cgf::Game* game) {

}

void TitleState::update(cgf::Game* game) {

}
void TitleState::draw(cgf::Game* game) {

}
