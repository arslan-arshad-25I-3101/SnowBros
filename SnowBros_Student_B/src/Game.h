#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
using namespace std;
using namespace sf;
enum class GameState
{
    MainMenu,
    CharacterSelect,
    Playing,
    Paused,
    LevelComplete,
    GameOver
};

class Game
{
public:
    
    Game();
    Game(float r, string col, float x, float y);
    ~Game();

    bool Update();

    void Render();
    sf::RenderWindow& GetWindow();

   
    bool IsRunning() const;

   
    void SetGameState(GameState newState);
    void vector(CircleShape*& s, int* n, CircleShape v);
    
    GameState GetGameState() const;

private:
    
    void HandleEvents();

    sf::RenderWindow window;                
    sf::Clock frameClock;                   
    GameState currentState;                 
    sf::CircleShape* shape;
    bool debugMode;          
    int count = 0;
};
