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
    posx = 100;
    posy = 285;
    walking = false;

    playSprite1.load("data/img/megaman.png", 32, 32, 0, 0, 0, 0, 3, 5, 14);
    playSprite1.setPosition(posx,posy);
    playSprite1.setFrameRange(12,13);
    playSprite1.setAnimRate(15);
    playSprite1.setLooped(true);
    playSprite1.play();

    map = new tmx::MapLoader("data/maps");       // todos os mapas/tilesets ser�o lidos de data/maps
    map->Load("megaman-v2.tmx");


    dirx = 0; // sprite direction: right (1), left (-1)
    diry = 1; // down (1), up (-1)
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
            jumping = true;
            jumpCount = 10;
        }
    }

    if(last != dirx && dirx != 0){
        playSprite1.setMirror(dirx == -1);
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

void PlayState::setAnim(){
    int currentAnim = 0;

    int w = walking ? 1 : 0;
    int j = jumping ? 1 : 0;
    int s = shooting ? 1 : 0;

    int state[2][2][2] = {{{0,1},{2,3}},{{4,5},{6,7}}};

    state[0][0][0] = 0;
    state[1][0][0] = 1;
    state[0][1][0] = 2;
    state[0][0][1] = 3;
    state[1][1][0] = 4;
    state[0][1][1] = 5;
    state[1][0][1] = 6;
    state[1][1][1] = 7;

    currentAnim = state[w][j][s];

    if(lastAnim != currentAnim){
        playSprite1.stop();
       // cout << currentAnim << endl;
        switch(currentAnim){
            case 0:
                playSprite1.play();
                playSprite1.setFrameRange(12,13);
                break;
            case 1:
                playSprite1.play();
                playSprite1.setFrameRange(6,8);
                break;
            case 2:
                playSprite1.setCurrentFrame(1);
                break;
            case 3:
                playSprite1.setCurrentFrame(3);
                break;
            case 4:
                playSprite1.setCurrentFrame(1);
                break;
            case 5:
                playSprite1.setCurrentFrame(4);
                break;
            case 6:
                playSprite1.play();
                playSprite1.setFrameRange(9,11);
                break;
            case 7:
                playSprite1.setCurrentFrame(4);
                break;
        }
    }

    lastAnim = currentAnim;
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

    sf::Vector2f center(panX,panY);
    view.setCenter(center);
    screen->setView(view);
}


sf::Uint16 PlayState::checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj)
{
    int i, x1, x2, y1, y2;
    sf::Uint16 bump = 0;

    // Get the limits of the map
    sf::Vector2u mapsize = map->GetMapSize();
    // Get the width and height of a single tile
    sf::Vector2u tilesize = map->GetMapTileSize();

    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    mapsize.x--;
    mapsize.y--;

    // Get the height and width of the object (in this case, 100% of a tile)
    sf::Vector2u objsize = obj->getSize();
    objsize.x *= obj->getScale().x * 0.95;
    objsize.y *= obj->getScale().y * 0.95;

    float px = obj->getPosition().x;
    float py = obj->getPosition().y;

    double deltaTime = game->getUpdateInterval();

    sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

    float vx = offset.x;
    float vy = offset.y;

    //cout << "px,py: " << px << " " << py << endl;

    //cout << "tilesize " << tilesize.x << " " << tilesize.y << endl;
    //cout << "mapsize " << mapsize.x << " " << mapsize.y << endl;

    // Test the horizontal movement first
    i = objsize.y > tilesize.y ? tilesize.y : objsize.y;
    for (;;)
    {
        x1 = (px + vx) / tilesize.x;
        x2 = (px + vx + objsize.x - 1) / tilesize.x;

        y1 = (py) / tilesize.y;
        y2 = (py + i - 1) / tilesize.y;

        if (x1 >= 0 && x2 < mapsize.x && y1 >= 0 && y2 < mapsize.y)
        {
            if (vx > 0)
            {
                // Trying to move right
                int upRight   = getCellFromMap(layer, x2*tilesize.x, y1*tilesize.y);
                int downRight = getCellFromMap(layer, x2*tilesize.x, y2*tilesize.y);
                if (upRight || downRight)
                {
                    // Place the player as close to the solid tile as possible
                    px = x2 * tilesize.x;
                    px -= objsize.x;// + 1;
                    vx = 0;

                    if(upRight)
                        bump = upRight;
                    else
                        bump = downRight;
                }
            }

            else if (vx < 0)
            {
                // Trying to move left
                int upLeft   = getCellFromMap(layer, x1*tilesize.x, y1*tilesize.y);
                int downLeft = getCellFromMap(layer, x1*tilesize.x, y2*tilesize.y);
                if (upLeft || downLeft)
                {
                    // Place the player as close to the solid tile as possible
                    px = (x1+1) * tilesize.x;
                    vx = 0;

                    if(upLeft)
                        bump = upLeft;
                    else
                        bump = downLeft;
                }
            }
        }

        if (i == objsize.y) // Checked player height with all tiles ?
        {
            break;
        }

        i += tilesize.y; // done, check next tile upwards

        if (i > objsize.y)
        {
            i = objsize.y;
        }
    }

    // Now test the vertical movement

    i = objsize.x > tilesize.x ? tilesize.x : objsize.x;

    for (;;)
    {
        x1 = (px / tilesize.x);
        x2 = ((px + i-1) / tilesize.x);

        y1 = ((py + vy) / tilesize.y);
        y2 = ((py + vy + objsize.y-1) / tilesize.y);

        if (x1 >= 0 && x2 < mapsize.x && y1 >= 0 && y2 < mapsize.y)
        {
            if (vy > 0)
            {
                  // Trying to move down
                int downLeft  = getCellFromMap(layer, x1*tilesize.x, y2*tilesize.y);
                int downRight = getCellFromMap(layer, x2*tilesize.x, y2*tilesize.y);
                if (downLeft || downRight)
                {
                    // Place the player as close to the solid tile as possible
                    py = y2 * tilesize.y;
                    py -= objsize.y;
                    vy = 0;

                    if(downLeft)
                        bump = downLeft;
                    else
                        bump = downRight;
                }
            }

            else if (vy < 0)
            {
                // Trying to move up
                int upLeft  = getCellFromMap(layer, x1*tilesize.x, y1*tilesize.y);
                int upRight = getCellFromMap(layer, x2*tilesize.x, y1*tilesize.y);
                if (upLeft || upRight)
                {
                    // Place the player as close to the solid tile as possible
                    py = (y1 + 1) * tilesize.y;
                    vy = 0;

                    if(upLeft)
                        bump = upLeft;
                    else
                        bump = upRight;
                }
            }
        }

        if (i == objsize.x)
        {
            break;
        }

        i += tilesize.x; // done, check next tile to the right

        if (i > objsize.x)
        {
            i = objsize.x;
        }
    }


    // Now apply the movement and animation

    obj->setPosition(px+vx,py+vy);
    px = obj->getPosition().x;
    py = obj->getPosition().y;

    obj->update(deltaTime, false); // only update animation

  /*

    // Check collision with edges of map
    if (px < 0)
        obj->setPosition(px,py);
    else if (px + objsize.x >= mapsize.x * tilesize.x)
        obj->setPosition(mapsize.x*tilesize.x - objsize.x - 1,py);

    if(py < 0)
        obj->setPosition(px,0);
    else if(py + objsize.y >= mapsize.y * tilesize.y)
        obj->setPosition(px, mapsize.y*tilesize.y - objsize.y - 1);
    */
    return bump;
}


// Get a cell GID from the map (x and y in global coords)
sf::Uint16 PlayState::getCellFromMap(uint8_t layernum, float x, float y)
{
    auto layers = map->GetLayers();
    tmx::MapLayer& layer = layers[layernum];
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2u tilesize = map->GetMapTileSize();
    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    int col = floor(x / tilesize.x);
    int row = floor(y / tilesize.y);
    return layer.tiles[row*mapsize.x + col].gid;
}

void PlayState::update(cgf::Game* game)
{
    screen = game->getScreen();

    jumpCount -= jumpCount > 0 ? 1 : 0;
    walking = dirx != 0;

    if(jumpCount > 0){
        diry = -1;
    } else {
        diry = 1;
    }

    playSprite1.setXspeed(dirx*150);
    playSprite1.setYspeed(diry*250);

    sf::Uint16 tile = checkCollision(1, game, &playSprite1);
    cout << "Tile: " << tile << endl;


    switch(tile){

        case 21: //floor
        break;
        case 2: // ceiling
        break;
        case 1: // obstacle left, right
        break;
        case 58: // game over

        break;


    }

    jumping = tile != 21;

    setAnim();

    //playSprite1.setPosition(x,y);
    //playSprite1.update(game->getUpdateInterval());

    centerMapOnPlayer();
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    map->Draw(*screen);         // mapa � fundo, precisa desenhar primeiro
    screen->draw(playSprite1);
}
