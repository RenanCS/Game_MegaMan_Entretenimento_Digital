#ifndef TITLESTATE_H
#define TITLESTATE_H

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <tmx/MapLoader.h>
#include <string>


class TitleState : public cgf::GameState
{
    public:

    void init();
    void cleanup();

    void pause();
    void resume();

    void handleEvents(cgf::Game* game);
    void update(cgf::Game* game);
    void draw(cgf::Game* game);

    static TitleState* instance() {
        return &m_TitleState;
    }

    protected:

    TitleState() {}

    private:

    static TitleState m_TitleState;
    sf::RenderWindow* screen;
    cgf::InputManager* im;

    sf::Font font;
    sf::Text titulo, botao;
};

#endif // TITLESTATE_H
