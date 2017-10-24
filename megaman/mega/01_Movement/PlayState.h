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

typedef struct enemy;
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

    void CreateEnemies();
    void CreateMegaMan();
    void ControlSetting();
    void CreateLayout();

    void UpdateMegaman(cgf::Game* game);
    void AddEnemy(int id, int x, int y);
    void AddBoss();
    void UpdateEnemy(cgf::Game* game);
    void UpdateShoots(cgf::Game* game);
    void UpdateBoss(cgf::Game* game);
    void UpdateRock(cgf::Game* game);
    void InitScore();
    void InitSound();
    void InitHpBar();
    void DamageMegaman(int damage);

    void BossLand(bool mirror);
    void BossRock(int x);

    void SetMegamanAnim();
    void Shoot();
    void Clear();
    void Died();

    // Implement Singleton Pattern
    static PlayState* instance()
    {
        return &m_PlayState;
    }

    protected:

    PlayState() {}

    private:

    static PlayState m_PlayState;

    int dirx, diry, last, jumpCount, posx,posy, lastAnim, shootDelay, damageDelay, score, alive, gameoverCount, enemyDelay, bossState, bossTimer, rockDir;
    bool shooting, jumping, walking, onBoss;
    cgf::Sprite megaman, hpbar, rock;
    std::vector<cgf::Sprite> hpbartiles;
    sf::RenderWindow* screen;
    cgf::InputManager* im;
    tmx::MapLoader* map;
    sf::Music music;
    sf::Font font;
    sf::Text scoreText;

    // Centers the camera on the player position
    void centerMapOnPlayer();

    // Checks collision between a sprite and a map layer
    sf::Uint16 checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj);

    // get a cell GID from the map (x and y in world coords)
    sf::Uint16 getCellFromMap(uint8_t layernum, float x, float y);


};

#endif
