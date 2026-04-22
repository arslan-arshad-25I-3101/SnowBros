#include "Game.h"
#include <optional>
void vector(int*& a, int* n, int v) {
    *n += 1;
    int* arr = new int[*n];
    for (int i = 0; i < *n; i++) {
        if (i < *n - 1)
            arr[i] = a[i];
        else
            arr[i] = v;
    }
    delete[] a;
    a = nullptr;
    a = arr;
}
void Game::vector(CircleShape*& s, int* n, CircleShape v) {
    *n += 1;
    CircleShape* arr = new CircleShape[*n];
    for (int i = 0; i < *n; i++) {
        if (i < *n - 1)
            arr[i] = s[i];
        else
            arr[i] = v;
    }
    delete[] s;
    s = nullptr;
    s = arr;
}
Game::Game()
    : window(VideoMode({800u, 600u}), "Snow Bros - OOP Project"),
      currentState(GameState::Playing),
      debugMode(false)
{
    window.setFramerateLimit(60);
}

Game::Game(float r, string col, float x, float y) : window(sf::VideoMode({ 800u, 600u }), "Snow Bros - OOP Project"),currentState(GameState::Playing), debugMode(false) {
window.setFramerateLimit(60);
shape->setRadius(r);
shape->setFillColor(Color::Blue);
shape->setPosition({x, y});
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
for(int i = 0; i < count; i++){
    window.clear(sf::Color::Black);
    window.draw(*shape);
    window.display();
    }
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
        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse->button == sf::Mouse::Button::Left) {
                if (count < 30)
                    count++;
            }
           }
        if (event->is<sf::Event::Closed>())
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
        Render();
    }
}
