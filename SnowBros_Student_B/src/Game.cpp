/**
 * @file Game.cpp
 * @brief Implementation of main Game class
 */

#include "Game.h"

using namespace std;
using namespace sf;

Game::Game()
    : window(VideoMode({800u, 600u}), "Snow Bros - OOP Project"),
      currentState(GameState::Playing),
      debugMode(false)
{
    window.setFramerateLimit(60);
}

Game::~Game() = default;

bool Game::Update()
{
    HandleEvents();
    frameClock.restart();
    return window.isOpen();
}

void Game::Render()
{
    window.clear(Color::Black);
    window.display();
}

RenderWindow& Game::GetWindow()
{
    return window;
}

bool Game::IsRunning() const
{
    return window.isOpen();
}

void Game::SetGameState(GameState newState)
{
    currentState = newState;
}

GameState Game::GetGameState() const
{
    return currentState;
}

void Game::HandleEvents()
{
    while (true)
    {
        auto event = window.pollEvent();
        if (!event)
        {
            break;
        }

        if (event->is<Event::Closed>())
        {
            window.close();
        }
        else
        {
            const Event::KeyPressed* keyPressed = event->getIf<Event::KeyPressed>();
            if (keyPressed != nullptr)
            {
                if (keyPressed->scancode == Keyboard::Scancode::Escape)
                {
                    window.close();
                }
                else if (keyPressed->scancode == Keyboard::Scancode::F1)
                {
                    debugMode = !debugMode;
                }
            }
        }
    }
}
