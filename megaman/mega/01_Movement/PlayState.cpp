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
#include "PlayState.h"
#include "PauseState.h"
#include "InputManager.h"
#include <tmx/MapLoader.h>

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init()
{
    posx = 10;
    posy = 320;

    playSprite1.load("data/img/megaman.png", 32, 32, 0, 0, 0, 0, 3, 5, 14);
    playSprite1.setPosition(10,320);
    playSprite1.setFrameRange(12,13);
    playSprite1.setAnimRate(15);
    playSprite1.setLooped(true);
    playSprite1.play();

    map = new tmx::MapLoader("data/maps");       // todos os mapas/tilesets serão lidos de data/maps
    map->Load("mapmegaman.tmx");


    dirx = 0; // sprite direction: right (1), left (-1)
    diry = 0; // down (1), up (-1)
    last = 0;

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("shoot", sf::Keyboard::A);
    im->addKeyInput("jump", sf::Keyboard::S);
    im->addMouseInput("rightclick", sf::Mouse::Right);
    im->addKeyInput("pause", sf::Keyboard::Return);
    im->addKeyInput("zoomout", sf::Keyboard::Z);

	cout << "PlayState: Init" << endl;
}

void PlayState::cleanup()
{
     delete map;
	cout << "PlayState: Clean" << endl;
}

void PlayState::pause()
{
	cout << "PlayState: Paused" << endl;
}

void PlayState::resume()
{
	cout << "PlayState: Resumed" << endl;
}

void PlayState::handleEvents(cgf::Game* game)
{
    sf::Event event;
    sf::View view = screen->getView();

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
    }

    dirx = diry = 0;
    shooting = false;

    if(im->testEvent("left"))
        dirx = -1;

    if(im->testEvent("right"))
        dirx = 1;

    if(im->testEvent("shoot"))
        shooting = true;

    if(im->testEvent("jump")){
        if (!jumping){
            jumpCount = 400;
        }
    }

    jumpCount -= jumpCount > 0 ? 1 : 0;
    jumping = jumpCount > 0;

    if(last != dirx && dirx != 0){
        playSprite1.setMirror(dirx == -1);
        if(jumping){
            if(shooting){
                playSprite1.setCurrentFrame(4);
            } else{
                playSprite1.setCurrentFrame(1);
            }
        }else{
            if(shooting){
                playSprite1.setFrameRange(9,11);
            } else{
                playSprite1.setFrameRange(6,8);
            }
        }
    }

    if(jumpCount > 200){
        playSprite1.move(0, -1.0);
    }else if(jumping){
        playSprite1.move(0, 1.0);
    }

    if(!jumping){
        playSprite1.setPosition(playSprite1.getPosition().x,posy);
    }

    if(dirx == 0){
        if(jumping){
            if(shooting){
                playSprite1.setCurrentFrame(4);
            }else{
                playSprite1.setCurrentFrame(1);
            }
        }else{
            if(shooting){
                playSprite1.setCurrentFrame(3);
            }else{
                playSprite1.setCurrentFrame(12);
            }
        }
    }

    last = dirx;

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("stats"))
        game->toggleStats();

    if(im->testEvent("return"))
        game->pushState(PauseState::instance());

    if(im->testEvent("zoomout"))
    {
        view.zoom(0.99);
        screen->setView(view);
    }

}

void PlayState::centerMapOnPlayer()
{
    sf::View view = screen->getView();
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2f viewsize = view.getSize();
    viewsize.x /= 2;
    viewsize.y /= 2;
    sf::Vector2f pos = playSprite1.getPosition();

    float panX = viewsize.x; // minimum pan
    if(pos.x >= viewsize.x)
        panX = pos.x;

    if(panX >= mapsize.x - viewsize.x)
        panX = mapsize.x - viewsize.x;

    float panY = viewsize.y; // minimum pan
    if(pos.y >= viewsize.y)
        panY = pos.y;

    if(panY >= mapsize.y - viewsize.y)
        panY = mapsize.y - viewsize.y;

    view.setCenter(sf::Vector2f(panX,panY));
    screen->setView(view);
}


void PlayState::update(cgf::Game* game)
{
    float x = playSprite1.getPosition().x;
    float y = playSprite1.getPosition().y;
    x += dirx*5;
    y += diry*5;
    playSprite1.setPosition(x,y);
    playSprite1.update(game->getUpdateInterval());

}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    map->Draw(*screen);         // mapa é fundo, precisa desenhar primeiro
    screen->draw(playSprite1);
    centerMapOnPlayer();
}
