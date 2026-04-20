#pragma once
#include "Entity.h"


class EnemyStats
{
public:
    int health = 1;
    int speed = 50;                 
    int scoreMin = 100;             
    int scoreMax = 500;             
    int gemDropChance = 30;         
};

class Enemy : public Entity
{
public:
    virtual ~Enemy() = default;

    virtual bool TakeDamage(int damage) = 0;
    int GetHealth() const;

    int GetRandomScore() const;

 
    bool IsEncased() const;

    void SetEncased(bool encased);

    const EnemyStats& GetStats() const;

protected:
  
    Enemy(const sf::Vector2f& position, const EnemyStats& stats);

    EnemyStats stats;           
    int currentHealth;          
    bool encased;
};
