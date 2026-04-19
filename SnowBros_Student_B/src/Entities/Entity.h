/**
 * @file Entity.h
 * @brief Base class for all game entities
 * 
 * This abstract class represents any drawable, updatable object in the game.
 * All entities have position, velocity, hitbox, and active state.
 */

#pragma once
#include <SFML/Graphics.hpp>

class Entity
{
public:
    /**
     * @brief Virtual destructor for proper cleanup in inheritance
     */
    virtual ~Entity() = default;

    /**
     * @brief Update entity logic each frame
     * @param deltaTime Time elapsed since last frame (seconds)
     */
    virtual void Update(float deltaTime) = 0;

    /**
     * @brief Draw entity to the window
     * @param window Target render window
     */
    virtual void Draw(sf::RenderWindow& window) = 0;

    /**
     * @brief Get entity's current position
     * @return Position vector
     */
    virtual sf::Vector2f GetPosition() const;

    /**
     * @brief Set entity's position
     * @param pos New position
     */
    virtual void SetPosition(const sf::Vector2f& pos);

    /**
     * @brief Get entity's hitbox for collision detection
     * @return Bounding rectangle
     */
    virtual sf::FloatRect GetHitBox() const;

    /**
     * @brief Check if entity is active
     * @return true if active, false if should be removed
     */
    bool IsActive() const;

    /**
     * @brief Deactivate entity (mark for removal)
     */
    void Deactivate();

protected:
    /**
     * @brief Protected constructor - only derived classes can instantiate
     */
    Entity(const sf::Vector2f& position = sf::Vector2f(0, 0));

    sf::Vector2f position;      ///< Entity world position
    sf::Vector2f velocity;      ///< Entity velocity (pixels per second)
    sf::FloatRect hitbox;       ///< Collision hitbox
    bool active;                ///< Whether entity is active in game
};
