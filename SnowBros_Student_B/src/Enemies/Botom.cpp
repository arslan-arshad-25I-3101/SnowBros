///**
// * @file Botom.cpp
// * @brief Implementation of Botom enemy
// */
//
//#include "Botom.h"
//
//using namespace sf;
//
//Botom::Botom(const Vector2f& position)
//    : Enemy(position, EnemyStats{
//        .health = 2,
//        .speed = 50,
//        .scoreMin = 100,
//        .scoreMax = 500,
//        .gemDropChance = 30
//    }),
//    direction(1),
//    gravity(300.0f),
//    onPlatform(true)
//{
//    sprite.setRadius(15.0f);
//    sprite.setFillColor(Color::Blue);
//    sprite.setPosition(position);
//
//    hitbox.size = Vector2f(30.0f, 30.0f);
//    hitbox.position = position;
//}
//
//void Botom::Update(float deltaTime)
//{
//    if (!active || encased)
//    {
//        return;
//    }
//
//    velocity.x = static_cast<float>(stats.speed * direction);
//    position.x += velocity.x * deltaTime;
//
//    if (!onPlatform)
//    {
//        velocity.y += gravity * deltaTime;
//        position.y += velocity.y * deltaTime;
//    }
//
//    sprite.setPosition(position);
//    hitbox.position = position;
//}
//
//void Botom::Draw(RenderWindow& window)
//{
//    if (active)
//    {
//        window.draw(sprite);
//    }
//}
//
//bool Botom::TakeDamage(int damage)
//{
//    return Enemy::TakeDamage(damage);
//}
