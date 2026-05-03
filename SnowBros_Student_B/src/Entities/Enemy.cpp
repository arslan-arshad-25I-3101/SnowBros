#include "Enemy.h"
#include "../Levels/Tiles.h"

sf::Texture Enemy::frozenSharedTexture;

Enemy::Enemy() : Entity(), frozen(false), frozenIndex(0), rolling(false), 
          rollingVelocityX(0.f), alive(true), dying(false), enemyScale(1.f), frozenOverlay(nullptr) {}

Enemy::~Enemy() {
    if (frozenOverlay) delete frozenOverlay;
}

bool Enemy::isAlive() const { return alive && !dying; }
bool Enemy::isDying() const { return dying; }
bool Enemy::isFrozen() const { return frozen; }
bool Enemy::isRolling() const { return rolling; }
bool Enemy::isFullyFrozen() const { return frozen && frozenIndex == 3; }

void Enemy::setPos(float i) {
    sprite->setPosition({ 350.f, 65.f + i * 65.f });
}

void Enemy::setPos(int row, int col) {
    sprite->setPosition({ col * Level::TILE_W + Level::TILE_W / 2.f,
                         row * Level::TILE_H + Level::TILE_H / 2.f });
}

void Enemy::setPos(float x, float y, float originX, float originY) {
    sprite->setOrigin({ originX, originY });
    sprite->setPosition({ x, y });
}

void Enemy::setOrigin(float x, float y) {
    sprite->setOrigin({ x, y });
}

void Enemy::reset() {
    alive = true;
    dying = false;
    frozen = false;
    rolling = false;
    frozenIndex = 0;
    velocityY = 0.f;
    rollingVelocityX = 0.f;
}

void Enemy::movement(class Tiles* tiles, int tileCount) {}

sf::Sprite& Enemy::getSprite() { return *sprite; }

sf::Sprite& Enemy::getFrozenOverlay() { 
    if (!frozenOverlay) {
        frozenOverlay = new sf::Sprite(frozenSharedTexture);
    }
    frozenOverlay->setTextureRect(frozenFrames[frozenIndex]);
    if (sprite) {
        frozenOverlay->setPosition(sprite->getPosition());
        // Keep native size (1.0 scale) as requested, to avoid it being too small
        frozenOverlay->setScale({ 1.f, 1.f });
    }
    
    sf::FloatRect bounds = frozenOverlay->getLocalBounds();
    frozenOverlay->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    
    return *frozenOverlay; 
}
