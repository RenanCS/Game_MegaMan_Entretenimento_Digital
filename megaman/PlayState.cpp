#include <iostream>
#include <cmath>
#include "Game.h"
#include "PlayState.h"
#include "Character.h"
#include "Player.h"

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init() {

}

void PlayState::cleanup() {
   }

void PlayState::pause() {}
void PlayState::resume() {}

void PlayState::handleEvents(cgf::Game* game) {

}

void PlayState::update(cgf::Game* game) {

}

void PlayState::draw(cgf::Game* game) {


}

void PlayState::centerMapOnPlayer() {

}

sf::Vector2f PlayState::getPan() {

}


// Obter uma célula GID do mapa x e y em coords globais
sf::Uint16 PlayState::getCellFromMap(uint8_t layernum, float x, float y) {


}
