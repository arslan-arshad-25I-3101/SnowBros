#pragma once
#include <SFML/Graphics.hpp>

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
 
    ~Game();

    bool Update();

    void Render();
    sf::RenderWindow& GetWindow();

   
    bool IsRunning() const;

   
    void SetGameState(GameState newState);

    
    GameState GetGameState() const;

private:
    
    void HandleEvents();

    sf::RenderWindow window;                
    sf::Clock frameClock;                   
    GameState currentState;                 
    
    bool debugMode;                         
};
