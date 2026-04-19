/**
 * @file Entity.cpp
 * @brief Implementation of base Entity class
 */

#include "Entity.h"

Entity::Entity(const sf::Vector2f& position)
    : position(position), velocity(0.f, 0.f), active(true)
{
    hitbox = sf::FloatRect(position, sf::Vector2f(0.f, 0.f));
}

sf::Vector2f Entity::GetPosition() const
{
    return position;
}

void Entity::SetPosition(const sf::Vector2f& pos)
{
    position = pos;
    hitbox.position = position;
}

sf::FloatRect Entity::GetHitBox() const
{
    return hitbox;
}

bool Entity::IsActive() const
{
    return active;
}

void Entity::Deactivate()
{
    active = false;
}
