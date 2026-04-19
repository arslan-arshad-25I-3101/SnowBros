/**
 * @file Enemy.cpp
 * @brief Implementation of abstract Enemy class
 */

#include "Enemy.h"
#include <random>

Enemy::Enemy(const sf::Vector2f& position, const EnemyStats& stats)
    : Entity(position), stats(stats), currentHealth(stats.health), encased(false)
{
}

bool Enemy::TakeDamage(int damage)
{
    if (encased) return true;  // Already encased
    
    currentHealth -= damage;
    if (currentHealth <= 0)
    {
        encased = true;
        return true;
    }
    return false;
}

int Enemy::GetHealth() const
{
    return currentHealth;
}

int Enemy::GetRandomScore() const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(stats.scoreMin, stats.scoreMax);
    return dis(gen);
}

bool Enemy::IsEncased() const
{
    return encased;
}

void Enemy::SetEncased(bool encased)
{
    this->encased = encased;
}

const EnemyStats& Enemy::GetStats() const
{
    return stats;
}
