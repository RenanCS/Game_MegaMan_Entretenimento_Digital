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
#include <list>
#include <SFML/Audio.hpp>
#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <tmx/MapLoader.h>
#include <vector>


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

    void CreateEnemy();
    void CreateMegaMan();
    void ControlSetting();
    void CreateLayout();

    void UpdateMegaman(cgf::Game* game);
    void DrawEnemy(struct enemy &ene, int i);

    // Implement Singleton Pattern
    static PlayState* instance()
    {
        return &m_PlayState;
    }

    protected:

    PlayState() {}

    private:

    static PlayState m_PlayState;

    int dirx, diry, last, jumpCount, posx,posy, lastAnim;
    bool shooting, jumping, walking;
    cgf::Sprite megaman;
    sf::RenderWindow* screen;
    cgf::InputManager* im;
    tmx::MapLoader* map;
    sf::Music music;

    void setAnim();

    void shoot();

    // Centers the camera on the player position
    void centerMapOnPlayer();

    // Checks collision between a sprite and a map layer
    sf::Uint16 checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj);

    // get a cell GID from the map (x and y in world coords)
    sf::Uint16 getCellFromMap(uint8_t layernum, float x, float y);


};

#endif
