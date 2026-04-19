/**
 * @file Game.h
 * @brief Main game controller - manages state, loop, and entity updates
 * 
 * This is the core orchestrator that ties together all game systems.
 * Responsible for the main game loop, state management, and delegating
 * updates to appropriate managers.
 */

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
    /**
     * @brief Constructor
     */
    Game();
    /**
     * @brief Destructor
     */
    ~Game();

    /**
     * @brief Main game loop - called repeatedly
     * @return false if game should exit
     */
    bool Update();

    /**
     * @brief Render current frame
     */
    void Render();

    /**
     * @brief Get game window
     * @return Reference to render window
     */
    sf::RenderWindow& GetWindow();

    /**
     * @brief Check if game is running
     * @return true if window is open
     */
    bool IsRunning() const;

    /**
     * @brief Change game state
     * @param newState New state
     */
    void SetGameState(GameState newState);

    /**
     * @brief Get current game state
     * @return Current state
     */
    GameState GetGameState() const;

private:
    /**
     * @brief Handle input events
     */
    void HandleEvents();

    sf::RenderWindow window;                ///< Game window
    sf::Clock frameClock;                   ///< For measuring delta time
    GameState currentState;                 ///< Current game state
    
    bool debugMode;                         ///< Debug hitbox display toggle
};
