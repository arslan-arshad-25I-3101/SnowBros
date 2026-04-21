#pragma once

#include "src/Enemies/Botom.h"

using namespace sf;

class FlyingEnemy : public Botom
{
public:
    FlyingEnemy(const Vector2f& position);

    void Update(float deltaTime) override;

    void Draw(RenderWindow& window) override;

    bool TakeDamage(int damage) override;



};