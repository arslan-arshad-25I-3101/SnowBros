/**
 * @file Game.cpp
 * @brief Implementation of main Game class
 */

#include "Game.h"

Game::Game()
    : window(sf::VideoMode({800u, 600u}), "Snow Bros - OOP Project"),
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
    window.clear(sf::Color::Black);
    window.display();
}

sf::RenderWindow& Game::GetWindow()
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
    while (auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
            {
                window.close();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::F1)
            {
                debugMode = !debugMode;
            }
        }
    }
}
