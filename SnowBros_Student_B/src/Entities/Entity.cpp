/**
 * @file Entity.cpp
 * @brief Implementation of base Entity class
 */

#include "Entity.h"

Entity::Entity(const sf::Vector2f& position)
    : position(position), velocity(0, 0), active(true)
{
    hitbox = sf::FloatRect(position.x, position.y, 0, 0);
}

sf::Vector2f Entity::GetPosition() const
{
    return position;
}

void Entity::SetPosition(const sf::Vector2f& pos)
{
    position = pos;
    hitbox.left = position.x;
    hitbox.top = position.y;
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
