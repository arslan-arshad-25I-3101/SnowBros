/**
 * @file Game.cpp
 * @brief Implementation of main Game class
 */

#include "Game.h"
#include <optional>

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
    while (true)
    {
        std::optional<sf::Event> event = window.pollEvent();
        if (!event.has_value())
        {
            break;
        }

        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else
        {
            const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>();
            if (keyPressed != nullptr)
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
}
