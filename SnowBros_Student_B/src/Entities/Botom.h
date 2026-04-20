#pragma once
#include "Enemy.h"

class Botom : public Enemy
{
public:
   
    explicit Botom(const sf::Vector2f& position);

  
    void Update(float deltaTime) override;

    void Draw(sf::RenderWindow& window) override;

   
    bool TakeDamage(int damage) override;

protected:
    sf::CircleShape sprite;     
    int direction;              
    float gravity;              
    bool onPlatform;            
};
