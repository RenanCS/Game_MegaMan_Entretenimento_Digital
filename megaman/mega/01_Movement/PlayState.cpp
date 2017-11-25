/*
 *  PlayState.cpp
 *  Normal "play" state
 */

#include <iostream>
#include <cmath>
#include <vector>
#include <unistd.h>
#include <time.h>
#include <tmx/MapLoader.h>
#include "Game.h"
#include "MenuState.h"
#include "PlayState.h"
#include "PauseState.h"
#include "InputManager.h"
#include "SFML/Audio.hpp"

PlayState PlayState::m_PlayState;

using namespace std;

/* Global variables */
typedef struct shot_str {
    bool friendly;
    int dir;
    cgf::Sprite sprite;
}shot_str;

typedef struct sfx_str{
    sf::Music buster;
    sf::Music damage;
    sf::Music die;
    sf::Music jump;
    sf::Music explosion;
    sf::Music pause;
} sfx_str;

typedef struct enemy{
    cgf::Sprite sprite;
    std::string name;
    int id;
    int health;
    int damageDelay;
    int score;
    int damage;
} enemy;


std::vector<shot_str> shoots;
std::vector<enemy> enemies;
sfx_str soundfx;

#pragma region EVENT_GAME

void PlayState::init() {
    //Musica e sfx do cenario
    InitSound();

    //Pontuação
    InitScore();

    //Adiciona inimigos
    CreateEnemies();

    //Desenha Megaman
    CreateMegaMan();

    //Configuração de controle
    ControlSetting();

    //Desenha Mapa
    map = new tmx::MapLoader("data/maps");       // todos os mapas/tilesets ser�o lidos de data/maps
    map->Load("megaman-v2.tmx");

    cout << "PlayState: Init" << endl;
}

void PlayState::cleanup() {
    delete map;
    cout << "PlayState: Clean" << endl;
}

void PlayState::pause() {
    cout << "PlayState: Paused" << endl;
}

void PlayState::resume() {
    cout << "PlayState: Resumed" << endl;
}

void PlayState::handleEvents(cgf::Game* game) {
    sf::Event event;
    sf::View view = screen->getView();

    while (screen->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            game->quit();
    }

    dirx = diry = 0;
    shooting = false;

    if (im->testEvent("left"))
        dirx = -1;

    if (im->testEvent("right"))
        dirx = 1;

    if (im->testEvent("shoot")) {
        shooting = true;
        Shoot();
    }

    if (im->testEvent("jump")) {
        if (!jumping) {

            soundfx.jump.play();

            jumping = true;
            jumpCount = 10;
        }
    }

    if (last != dirx && dirx != 0) {
        megaman.setMirror(dirx == -1);
    }

    last = dirx;

    if (im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if (im->testEvent("stats"))
        game->toggleStats();

    if(im->testEvent("pause")){
        soundfx.pause.play();
        game->pushState(PauseState::instance());
    }

    if (im->testEvent("zoomout"))
    {
        view.zoom(0.99);
        screen->setView(view);
    }

}

void PlayState::InitSound() {
    music.openFromFile("data/audio/Stage.ogg");
    music.setVolume(50);
    music.play();
    music.setLoop(true);

    soundfx.buster.openFromFile("data/audio/MegaBuster.wav");
    soundfx.damage.openFromFile("data/audio/Damage.wav");
    soundfx.die.openFromFile("data/audio/Die.wav");
    soundfx.jump.openFromFile("data/audio/Jump.wav");
    soundfx.explosion.openFromFile("data/audio/Explosion.wav");
    soundfx.pause.openFromFile("data/audio/PauseMenu.wav");

    soundfx.buster.setVolume(50);
    soundfx.damage.setVolume(50);
    soundfx.die.setVolume(50);
    soundfx.jump.setVolume(50);
    soundfx.explosion.setVolume(50);
}

void PlayState::InitScore() {
    if (!font.loadFromFile("data/fonts/Altebro.ttf"))
    {
        cout << "Cannot load Altebro.ttf font!" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    scoreText.setString(L"Score:");
    scoreText.setCharacterSize(36); // in pixels
    scoreText.setColor(sf::Color::White);
    scoreText.setStyle(sf::Text::Bold);
    score = 0;
}

void PlayState::AddEnemy(int id, int x, int y) {

    //Cria uma struct do inimigo
    enemy ene;
    ene.id = id;
    ene.damageDelay = 0;

    //Seleciona o tipo de imagem
    switch(id)
    {
        case 1:
            ene.sprite.load("data/img/gutsman.png", 32, 32, 0, 0, 0, 0, 7, 1, 7);
            ene.health = 24;
            ene.damage = 5;
            ene.score = 5000;
            ene.name = "gustsman";
        break;

        case 2:
            ene.sprite.load("data/img/met.png", 20, 20, 0, 0, 0, 0, 5, 1, 5);
            ene.sprite.setFrameRange(2,4);
            ene.health = 2;
            ene.damage = 1;
            ene.score = 500;
            ene.name = "met";
        break;

        case 3:
            ene.sprite.load("data/img/blader.png", 16, 24, 0, 0, 0, 0, 4, 1, 4);
            ene.sprite.setFrameRange(0,1);
            ene.health = 1;
            ene.damage = 2;
            ene.score = 500;
            ene.name = "blader";
        break;
    }

    //Informa posição
    ene.sprite.setPosition(x,y);
    ene.sprite.setAnimRate(15);
    ene.sprite.setLooped(true);
    ene.sprite.play();

    enemies.push_back(ene);
}

void PlayState::AddBoss() {
    bossState = 0;
    AddEnemy(1, 2800, 280);
    enemy* boss = &enemies[0];
    boss->sprite.setMirror(true);
    boss->sprite.setLooped(false);
    rock.load("data/img/rock.png", 32, 32, 0, 0, 0, 0, 1, 1, 1);
}

void PlayState::Shoot() {

    if (shootDelay == 0) {
        //Delay entre disparos
        shootDelay = 5;
        //Som do disparo
        soundfx.buster.play();
        //Cria uma struct da bela
        shot_str shot;
        //Configura srpite
        shot.sprite.load("data/img/shot.png", 6, 6, 0, 0, 0,0, 1, 1,1);
        //Informa posição, conforme localização do mega man
        float posxMM = megaman.getPosition().x;
        float posyMM = megaman.getPosition().y + 16;
        posxMM  = megaman.getMirror() == 0 ? posxMM + 30 : posxMM - 5;
        shot.sprite.setPosition(posxMM, posyMM);
        //Informa direção da bala
        shot.dir = megaman.getMirror() == 0 ? 1 : -1;
        //Informa quem atirou
        shot.friendly = true;
        //Adiciona no vetor de balas
        shoots.push_back(shot);
    }
}

sf::Uint16 PlayState::getCellFromMap(uint8_t layernum, float x, float y) {
    auto& layers = map->GetLayers();
    tmx::MapLayer& layer = layers[layernum];
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2u tilesize = map->GetMapTileSize();
    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    int col = floor(x / tilesize.x);
    int row = floor(y / tilesize.y);
    return layer.tiles[row*mapsize.x + col].gid;
}


std::string to_zero_lead(const int value, const unsigned precision) {
     std::ostringstream oss;
     oss << std::setw(precision) << std::setfill('0') << value;
     return oss.str();
}

void PlayState::update(cgf::Game* game) {
    if (enemyDelay <= 0 && !onBoss) {
        srand (time(NULL));
        int randN = rand() % 100;
        if (randN >= 50) {
            AddEnemy(3, megaman.getPosition().x, 0);
        } else {
            AddEnemy(3, megaman.getPosition().x, 400);
        }
        enemyDelay = 150;
    } else {
        enemyDelay -= 1;
    }

    if (alive) {
        //Atualiza informações do mega man
        UpdateMegaman(game);

        //Configura direção e colisão dos disparos
        UpdateShoots(game);

        // Atualiza comportamento dos inimigos
        UpdateEnemy(game);

        if (onBoss) {
            UpdateRock(game);
        }
    } else {
        gameoverCount -= 1;
        if (gameoverCount <= 0) {
            Clear();
            game->changeState(MenuState::instance());
        }
    }

    // Atualiza score

    std::stringstream str;
    str << "Score: " << to_zero_lead(score, 5);
    scoreText.setString(str.str());

    centerMapOnPlayer();
}

void PlayState::UpdateMegaman(cgf::Game* game) {

    if ((hpbartiles.size() == 0 && damageDelay == 50) || megaman.getPosition().y > 400) {
        Died();
    } else {
        //Obtem o ambiente
        screen = game->getScreen();

        //Delay do tiro
        shootDelay -= shootDelay > 0 ? 1 : 0;

        //Configura pulo
        jumpCount -= jumpCount > 0 ? 1 : 0;
        damageDelay -= damageDelay > 0 ? 1 : 0;
        walking = dirx != 0;

        if (jumpCount > 0) {
            diry = -1;
        } else {
            diry = 1;
        }

        megaman.setVisible(damageDelay % 2 == 0);

        //Configura velocidade de corrida
        megaman.setXspeed(dirx*150);
        megaman.setYspeed(diry*250);

        //Obtem o tile de colisão
        sf::Uint16 tile = checkCollision(1, game, &megaman);
        //cout << "Tile: " << tile << endl;

        switch(tile) {
            case 21: //floor
            break;
            case 2: // ceiling
            break;
            case 1: // obstacle left, right
            break;
            case 58: // game over
            break;
        }

        if (megaman.getPosition().x >= 2435 && !onBoss) {
            onBoss = true;
            enemies.clear();
            AddBoss();
            music.openFromFile("data/audio/StageBoss.ogg");
            music.play();
        }

        jumping = tile != 21;

        SetMegamanAnim();

    }
}

void PlayState::Died() {
    alive = false;
    gameoverCount = 100;
    music.openFromFile("data/audio/GameOver.ogg");
    music.setLoop(false);
    music.play();
    soundfx.die.play();
}

void PlayState::Winned(){
    alive = false;
    gameoverCount = 200;
    music.openFromFile("data/audio/StageClear.ogg");
    music.setLoop(false);
    music.play();
    //soundfx.die.play();
}

void PlayState::Clear() {
    hpbartiles.clear();
    enemies.clear();
    shoots.clear();
}

void PlayState::SetMegamanAnim() {

    //Configura animação do mega man
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

    if (lastAnim != currentAnim) {
        megaman.setAnimRate(15);
        megaman.stop();
        switch(currentAnim) {
            case 0:
                megaman.play();
                megaman.setFrameRange(12,13);
                megaman.setAnimRate(5);
                cout << "position: (" << megaman.getPosition().x << ", " << megaman.getPosition().y << ")" << endl;
                break;
            case 1:
                megaman.play();
                megaman.setFrameRange(6,8);
                break;
            case 2:
                megaman.setCurrentFrame(1);
                break;
            case 3:
                megaman.setCurrentFrame(3);
                break;
            case 4:
                megaman.setCurrentFrame(1);
                break;
            case 5:
                megaman.setCurrentFrame(4);
                break;
            case 6:
                megaman.play();
                megaman.setFrameRange(9,11);
                break;
            case 7:
                megaman.setCurrentFrame(4);
                break;
        }
    }

    lastAnim = currentAnim;
}

void PlayState::UpdateShoots(cgf::Game* game) {
    std::vector<int> shotRemove;
    std::vector<int> enemyRemove;

    for (int i = 0; i < shoots.size(); i++) {
        shoots[i].sprite.move(shoots[i].dir * 7,0);
        for (int j = 0; j < enemies.size(); j++) {
            if (shoots[i].sprite.bboxCollision(enemies[j].sprite)) {
                if (enemies[j].damageDelay == 0) {
                    enemies[j].damageDelay = 5;
                    enemies[j].health -= 1;
                    if (enemies[j].health == 0) {
                        score += enemies[j].score;
                        enemyRemove.push_back(j);
                    }
                }
                shotRemove.push_back(i);
                break;
            }
        }

        if (std::abs(megaman.getPosition().x - shoots[i].sprite.getPosition().x)  > 500) {
            shotRemove.push_back(i);
        }
    }

    // Remove inimigos e disparos
    for (int i = 0; i < shotRemove.size(); i++) {
        shoots.erase(shoots.begin() + shotRemove[i]);
    }

    for (int i = 0; i < enemyRemove.size(); i++) {
        if(onBoss){
            rock.setVisible(false);
            Winned();
        }
        enemies.erase(enemies.begin() + enemyRemove[i]);
        soundfx.explosion.play();
    }
}

void PlayState::UpdateEnemy(cgf::Game* game) {
    int x, y;
    sf::Uint16 tile;
    std::vector<int> enemyRemove;

    for (int i = 0; i < enemies.size(); i++) {
        enemies[i].damageDelay -= enemies[i].damageDelay > 0 ? 1 : 0;
        if (enemies[i].sprite.getPosition().y < 500) {
            switch(enemies[i].id) {
                case 1:
                    UpdateBoss(game);
                    break;
                case 2:
                    x = enemies[i].sprite.getPosition().x > megaman.getPosition().x ? -1 : enemies[i].sprite.getPosition().x < megaman.getPosition().x ? 1 : 0;
                    y = enemies[i].sprite.getPosition().y < 305 ? 1 : 0;
                    enemies[i].sprite.setXspeed(x * 25);
                    enemies[i].sprite.setYspeed(1 * 75);
                    tile = checkCollision(1, game, &enemies[i].sprite);
                    if (tile == 21) {
                        enemies[i].sprite.move(0, -3);
                    }
                    break;
                case 3:
                    x = enemies[i].sprite.getPosition().x > megaman.getPosition().x ? -1 : enemies[i].sprite.getPosition().x < megaman.getPosition().x ? 1 : 0;
                    y = enemies[i].sprite.getPosition().y > megaman.getPosition().y ? -1 : enemies[i].sprite.getPosition().y < megaman.getPosition().y ? 1 : 0;
                    enemies[i].sprite.setXspeed(x * 50);
                    enemies[i].sprite.setYspeed(y * 50);
                    break;
            }
            if (damageDelay == 0 && enemies[i].sprite.bboxCollision(megaman)) {
                DamageMegaman(enemies[i].damage);
            }
            enemies[i].sprite.update(game->getUpdateInterval());
        } else {
            enemyRemove.push_back(i);
        }
    }

    for (int i = 0; i < enemyRemove.size(); i++ ) {
        enemies.erase(enemies.begin() + enemyRemove[i]);
        soundfx.explosion.play();
    }
}

void PlayState::DamageMegaman(int damage) {
    if (hpbartiles.size() > 0) {
        soundfx.damage.play();
        for (int j = 0; j < damage; j++) {
            if (hpbartiles.size() > 0) {
                hpbartiles.pop_back();
            }
        }
        damageDelay = 50;
    }
}

void PlayState::UpdateBoss(cgf::Game* game) {
    enemy* boss = &enemies[0];
    sf::Uint16 tile;

    switch(bossState) {
        case 0: //jump to left
            boss->sprite.setXspeed(-100);
            if (boss->sprite.getPosition().x > 2750) {
                boss->sprite.setYspeed(-50);
            } else {
                boss->sprite.setYspeed(50);
            }

            if (boss->sprite.getPosition().x <= 2500) {
                bossTimer = 75;
                bossState = 3;
                rockDir = 1;
                BossLand(false);
                BossRock(boss->sprite.getPosition().x);
            }
            break;
        case 1: //jump to right
            boss->sprite.setXspeed(100);
            if (boss->sprite.getPosition().x < 2650) {
                boss->sprite.setYspeed(-50);
            } else {
                boss->sprite.setYspeed(50);
            }

            if (boss->sprite.getPosition().x >= 2900) {
                bossTimer = 75;
                bossState = 2;
                rockDir = -1;
                BossLand(true);
                BossRock(boss->sprite.getPosition().x);
            }
            break;
        case 2: // throw rock right
            boss->sprite.play();
            if (bossTimer == 25) {
                boss->sprite.setFrameRange(0,1);
            }

            if (bossTimer <= 0) {
                boss->sprite.setCurrentFrame(2);
                bossState = 0;
            }
            break;
        case 3: // throw rock left
            boss->sprite.play();
            if (bossTimer == 25) {
                boss->sprite.setFrameRange(0,1);
            }
            if (bossTimer <= 0) {
                boss->sprite.stop();
                boss->sprite.setCurrentFrame(2);
                bossState = 1;
            }
            break;
    }
    bossTimer -= 1;
    tile = checkCollision(1, game, &boss->sprite);
    if (tile == 21) {
        boss->sprite.move(0, -3);
    }
    boss->sprite.update(game->getUpdateInterval());
}

void PlayState::BossLand(bool mirror) {
    enemy* boss = &enemies[0];

    boss->sprite.setXspeed(0);
    boss->sprite.setYspeed(0);
    boss->sprite.setLooped(false);
    boss->sprite.setFrameRange(3,6);
    boss->sprite.setAnimRate(1);
    boss->sprite.play();
    boss->sprite.setMirror(mirror);
}

void PlayState::BossRock(int x) {
    rock.setPosition(x, -50);
}

void PlayState::UpdateRock(cgf::Game* game) {
    if (bossTimer > 40) {
        if (rock.getPosition().y < 270) {
            rock.setXspeed(0);
            rock.setYspeed(500);
        } else {
            rock.setYspeed(0);
        }
    } else if (bossState == 3 || bossState == 2) {
        rock.setYspeed(10);
        rock.setXspeed(rockDir * 250);
    }

    if (damageDelay == 0 && rock.bboxCollision(megaman)) {
        DamageMegaman(5);
    }

    rock.update(game->getUpdateInterval());
}

void PlayState::draw(cgf::Game* game) {
    screen = game->getScreen();

    map->Draw(*screen);

    screen->draw(megaman);

    for (int i = 0; i < shoots.size(); i++) {
        screen->draw(shoots[i].sprite);
    }

    for (int i = 0; i < enemies.size(); i++) {
        screen->draw(enemies[i].sprite);
    }

    screen->draw(scoreText);
    screen->draw(hpbar);
    for (int i = 0; i < hpbartiles.size(); i++) {
        screen->draw(hpbartiles[i]);
    }

    if (onBoss) {
        screen->draw(rock);
    }
}

#pragma region INITIAL_SETTING

void PlayState::ControlSetting() {

    cout << "Configurando controles "  << endl;

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("shoot", sf::Keyboard::A);
    im->addKeyInput("jump", sf::Keyboard::S);
    im->addMouseInput("rightclick", sf::Mouse::Right);
    im->addKeyInput("pause", sf::Keyboard::P);
    //im->addKeyInput("pause", sf::Keyboard::Return);
    //im->addKeyInput("zoomout", sf::Keyboard::Z);

}

void PlayState::CreateMegaMan() {

    cout << "Criando Mega Man "  << endl;
    //Vivo
    alive = true;

    //Em chefe
    onBoss = false;

    //Direção do megaman
    dirx = 0; // sprite direction: right (1), left (-1)
    diry = 1; // down (1), up (-1)
    last = 0;

    //Posição inicial do megaman
    //posx = 100;
    posx = 100;
    posy = 285;
    walking = false;

    //Inicializa o Mega mansetVolume(50);
    megaman.load("data/img/megaman.png", 32, 32, 0, 0, 0, 0, 3, 5, 14);
    megaman.setPosition(posx,posy);
    megaman.setAnimRate(15);
    megaman.setLooped(true);
    megaman.setFrameRange(12,13);
    megaman.play();

    damageDelay = 0;
    InitHpBar();
}

void PlayState::InitHpBar() {
    //Inicializa o Mega man
    hpbar.load("data/img/hp_bar_background.png", 16, 84, 0, 0, 0, 0, 1, 1, 1);
    //hpbar.scale(1,1);
    //hpbar.scale(2,2);
    hpbar.setPosition(25,75);

    for (int i = 0; i < 20; i++) {
        cgf::Sprite tile;
        tile.load("data/img/hp_bar_tile.png", 12, 2, 0, 0, 0, 0, 1, 1, 1);
        hpbartiles.push_back(tile);
    }
}

void PlayState::CreateEnemies() {

    cout << "Criando Inimigos "  << endl;
    enemyDelay = 50;

    AddEnemy(2, 250, 290);
    AddEnemy(2, 550, 250);
    AddEnemy(2, 880, 250);
    AddEnemy(2, 928, 150);
    AddEnemy(2, 1270, 250);
    AddEnemy(2, 1650, 250);
    AddEnemy(2, 1700, 250);
    AddEnemy(2, 1850, 250);
    AddEnemy(2, 2415, 250);
}

#pragma region ADDITIONAL_CONFIGURATION

void PlayState::centerMapOnPlayer() {

    sf::View view = screen->getView();
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2f viewsize = view.getSize();
    viewsize.x /= 2;
    viewsize.y /= 2;

    sf::Vector2f pos = megaman.getPosition();

    float panX = viewsize.x; // minimum pan
    if (pos.x >= viewsize.x)
        panX = pos.x;

    if (panX >= mapsize.x - viewsize.x)
        panX = mapsize.x - viewsize.x;

    float panY = viewsize.y; // minimum pan
    if (pos.y >= viewsize.y)
        panY = pos.y;

    if (panY >= mapsize.y - viewsize.y)
        panY = mapsize.y - viewsize.y;

    sf::Vector2f center(panX,panY);
    view.setCenter(center);
    screen->setView(view);

    // set score position
    scoreText.setPosition(panX - 100, panY - 200);

    // set hp bar position
    hpbar.setPosition(panX - 275, panY - 150);

    for (int i = 0; i < hpbartiles.size(); i++) {
        hpbartiles[i].setPosition(hpbar.getPosition().x + 2, hpbar.getPosition().y + 79 - i*4);
    }
}

 sf::Uint16 PlayState::checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj) {
     int i, x1, x2, y1, y2;
     sf::Uint16 bump;

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
     objsize.x *= obj->getScale().x * 1;
     objsize.y *= obj->getScale().y * 1;

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

                     if (upRight)
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

                     if (upLeft)
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

                     if (downLeft)
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

                    if (upLeft)
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

     if (py < 0)
         obj->setPosition(px,0);
     else if (py + objsize.y >= mapsize.y * tilesize.y)
         obj->setPosition(px, mapsize.y*tilesize.y - objsize.y - 1);
     */
     return bump;
 }

