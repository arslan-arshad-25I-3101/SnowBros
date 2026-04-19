/**
 * @file Botom.cpp
 * @brief Implementation of Botom enemy
 */

#include "Botom.h"

Botom::Botom(const sf::Vector2f& position)
    : Enemy(position, EnemyStats{
        .health = 2,
        .speed = 50,
        .scoreMin = 100,
        .scoreMax = 500,
        .gemDropChance = 30
    }),
    direction(1),
    gravity(300.0f),
    onPlatform(true)
{
    // Create sprite (temporary - will be replaced with textures)
    sprite.setRadius(15.0f);
    sprite.setFillColor(sf::Color::Blue);
    sprite.setPosition(position);
    
    hitbox.width = 30.0f;
    hitbox.height = 30.0f;
}

void Botom::Update(float deltaTime)
{
    if (!active || encased) return;

    // Simple horizontal movement
    velocity.x = stats.speed * direction;
    position.x += velocity.x * deltaTime;

    // Apply gravity
    if (!onPlatform)
    {
        velocity.y += gravity * deltaTime;
        position.y += velocity.y * deltaTime;
    }

    // Update sprite position
    sprite.setPosition(position);
    hitbox.left = position.x;
    hitbox.top = position.y;
}

void Botom::Draw(sf::RenderWindow& window)
{
    if (active)
    {
        window.draw(sprite);
    }
}

bool Botom::TakeDamage(int damage)
{
    return Enemy::TakeDamage(damage);
}
