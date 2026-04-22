#include "Enemy.h"
#include <cstdlib>

using namespace std;
using namespace sf;

Enemy::Enemy(const Vector2f& position, const EnemyStats& stats)
    : Entity(position), stats(stats), currentHealth(stats.health), encased(false)
{
}

bool Enemy::TakeDamage(int damage)
{
    if (encased)
    {
        return true;
    }

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
    return stats.scoreMin + (rand() % (stats.scoreMax - stats.scoreMin + 1));
}

bool Enemy::IsEncased() const
{
    return encased;
}

void Enemy::SetEncased(bool encasedState)
{
    encased = encasedState;
}

const EnemyStats& Enemy::GetStats() const
{
    return stats;
}
