/**
 * @file Game.cpp
 * @brief Implementation of main Game class
 */

#include "Game.h"
#include "Entities/Entity.h"
#include "Systems/EventBus.h"

Game::Game()
    : window(sf::VideoMode(800, 600), "Snow Bros - OOP Project"),
      currentState(GameState::MainMenu),
      debugMode(false)
{
    window.setFramerateLimit(60);  // 60 FPS target
}

bool Game::Update()
{
    HandleEvents();

    float deltaTime = frameClock.restart().asSeconds();

    if (currentState == GameState::Playing)
    {
        UpdateEntities(deltaTime);
    }

    return window.isOpen();
}

void Game::Render()
{
    window.clear(sf::Color::Black);

    // Draw all active entities
    for (auto& entity : entities)
    {
        if (entity->IsActive())
        {
            entity->Draw(window);
        }
    }

    // Draw debug hitboxes if enabled
    if (debugMode)
    {
        for (auto& entity : entities)
        {
            if (entity->IsActive())
            {
                sf::FloatRect hitbox = entity->GetHitBox();
                sf::RectangleShape debugBox(sf::Vector2f(hitbox.width, hitbox.height));
                debugBox.setPosition(hitbox.left, hitbox.top);
                debugBox.setFillColor(sf::Color::Transparent);
                debugBox.setOutlineColor(sf::Color::Red);
                debugBox.setOutlineThickness(2.0f);
                window.draw(debugBox);
            }
        }
    }

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
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                if (currentState == GameState::Playing)
                {
                    currentState = GameState::Paused;
                }
                else if (currentState == GameState::Paused)
                {
                    currentState = GameState::Playing;
                }
            }
            else if (event.key.code == sf::Keyboard::F1 || event.key.code == sf::Keyboard::H)
            {
                debugMode = !debugMode;
            }
        }
    }
}

void Game::UpdateEntities(float deltaTime)
{
    for (auto& entity : entities)
    {
        if (entity->IsActive())
        {
            entity->Update(deltaTime);
        }
    }

    // Remove inactive entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const std::unique_ptr<Entity>& e) { return !e->IsActive(); }),
        entities.end()
    );
}
