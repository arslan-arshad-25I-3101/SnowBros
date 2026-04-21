/**
 * @file Botom.h
 * @brief Standard enemy type - basic walker
 * 
 * Botom is the most basic enemy. It walks left and right,
 * falls with gravity, and can be encased with snowballs.
 */

#pragma once
#include "Enemy.h"

using namespace sf;

class Botom : public Enemy
{
public:
    /**
     * @brief Constructor
     * @param position Starting position
     */
    explicit Botom(const sf::Vector2f& position);

    /**
     * @brief Update Botom movement and physics
     * @param deltaTime Time elapsed
     */
    void Update(float deltaTime) override;

    /**
     * @brief Draw Botom to window
     * @param window Target window
     */
    void Draw(sf::RenderWindow& window) override;

    /**
     * @brief Apply damage (snowball hit)
     * @param damage Damage amount
     * @return true if fully encased
     */
    bool TakeDamage(int damage) override;

protected:
    Sprite* sprite; ///< Simple circle sprite for now
    int direction;              ///< Movement direction: -1 (left) or 1 (right)
    float gravity;              ///< Gravity acceleration
    bool onPlatform;            ///< Whether standing on platform


};
