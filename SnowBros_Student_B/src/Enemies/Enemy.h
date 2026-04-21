/**
 * @file Enemy.h
 * @brief Abstract base class for all enemy types
 * 
 * All enemies inherit from this class and must implement
 * Update(), Draw(), and TakeDamage() methods.
 */

#pragma once
#include "../Entities/Entity.h"

/// Damage model for enemies
struct EnemyStats
{
    int health = 1;                 ///< Health points required to fully encase
    int speed = 50;                 ///< Movement speed (pixels/second)
    int scoreMin = 100;             ///< Minimum score for killing this enemy
    int scoreMax = 500;             ///< Maximum score for killing this enemy
    int gemDropChance = 30;         ///< Percentage chance to drop gems (0-100)
};

class Enemy : public Entity
{
public:
    virtual ~Enemy() = default;

    /**
     * @brief Apply damage to enemy (snowball hit)
     * @param damage Amount of damage to apply
     * @return true if fully encased, false otherwise
     */
    virtual bool TakeDamage(int damage) = 0;

    /**
     * @brief Get enemy's current health
     * @return Current health points
     */
    int GetHealth() const;

    /**
     * @brief Get random score for this enemy type
     * @return Random value between scoreMin and scoreMax
     */
    int GetRandomScore() const;

    /**
     * @brief Check if enemy is fully encased (snowball)
     * @return true if fully encased
     */
    bool IsEncased() const;

    /**
     * @brief Mark as encased or un-encased
     * @param encased Encased state
     */
    void SetEncased(bool encased);

    /**
     * @brief Get enemy's stats
     * @return Reference to stats structure
     */
    const EnemyStats& GetStats() const;

    

protected:
    /**
     * @brief Protected constructor - only derived classes can instantiate
     * @param position Starting position
     * @param stats Enemy statistics
     */
    Enemy(const sf::Vector2f& position, const EnemyStats& stats);

    EnemyStats stats;           ///< Enemy statistics
    int currentHealth;          ///< Current health (before fully encased)
    bool encased;               ///< Whether fully encased in snow
};
