#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
class Entity
{
public:
    virtual ~Entity() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;
    virtual sf::Vector2f GetPosition() const;
    virtual void SetPosition(const sf::Vector2f& pos);

    virtual sf::FloatRect GetHitBox() const;

 
    bool IsActive() const;

    void Deactivate();

protected:
    Entity(const sf::Vector2f& position = sf::Vector2f(0, 0));

    sf::Vector2f position;      
    sf::Vector2f velocity;      
    sf::FloatRect hitbox;       
    bool active;                
};
