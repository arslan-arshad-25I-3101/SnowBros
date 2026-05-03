#pragma once
#include "../Enemy.h"
#include "../../Levels/Tiles.h"
#include "../Projectiles/Snowball.h"
#include "../../Systems/DatabaseManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <string>

class Player;

//=============================================================================
// MogeraChild — small spawn released by Mogera boss
//=============================================================================
class MogeraChild {
protected:
    sf::Sprite* spawn;
    sf::Texture text[7];
    int currentFrame;
    float frametime;
    sf::Clock animClock;
    bool active;
    bool isJump;
    bool onGround;
    float velocityY;
    float gravity;

    bool checkCollision(Tiles* tiles, int tileCount);

public:
    MogeraChild();
    MogeraChild(const MogeraChild& other);
    MogeraChild& operator=(const MogeraChild& other);
    ~MogeraChild();

    void setPos(float x, float y, float w = 186.f, float h = 170.f);
    void applyGravity(Tiles* tiles, int tileCount);
    void movement();
    bool isActive();
    void Draw(sf::RenderWindow& window);
    sf::FloatRect getBounds();
};

void vectora(MogeraChild*& a, int* n, MogeraChild v);

//=============================================================================
// Mogera — main boss
//=============================================================================
class Mogera {
protected:
    sf::Sprite* bossChest;
    sf::Sprite* bossLeg;
    sf::Texture bodyText[3];
    sf::Texture legText[3];
    int bodyFrames;
    int legFrames;
    float frametime;
    sf::Clock clock;

    MogeraChild* spawns;
    int spawnCount;
    sf::Clock spawnClock;
    float spawnInterval;
    bool isDead;

public:
    static int bossHp;

    Mogera();
    virtual ~Mogera();

    bool movement(Tiles* tiles, int tileCount, Player& play,
                  DatabaseManager& db, std::string& username,
                  int& lives, int& gems, int& score, int& levelNo);

    void spawnEnemy();
    void cleanupInactiveSpawns();
    void setPos(float x, float y, float xx, float yy);
    void Draw(sf::RenderWindow& window, Tiles* tiles, int tileCount);
    sf::FloatRect boun();
    void setCol();
    void setNor();
    void checkdead();
    bool getDeath();
    void setDead(bool x = false);
};

//=============================================================================
// Bomb — projectile spawned by Gamakichi
//=============================================================================
class Bomb : public MogeraChild {
protected:
    sf::Texture texts[4];
    sf::Sprite* spawns;
    sf::Clock lifeClock;
    float explosionTime;
    bool hasExploded;

    bool checkCollision(Tiles* tiles, int tileCount);

public:
    Bomb();
    Bomb(const Bomb& other);
    Bomb& operator=(const Bomb& other);
    ~Bomb();

    void setPos(float x, float y, float w = 220.f, float h = 250.f);
    void applyGravity(Tiles* tiles, int tileCount);
    void movement();
    void explode();
    void Draw(sf::RenderWindow& window);
    bool isActive();
    sf::FloatRect getBound();
};

void vectory(Bomb*& a, int* n, Bomb v);

//=============================================================================
// Gamakichi — second boss
//=============================================================================
class Gamakichi : public Mogera {
protected:
    sf::Sprite* boss;
    sf::Texture text[2];
    sf::Texture smoke[4];
    sf::Sprite* smokey[4];
    float speed;
    Bomb* spawnss;
    bool mv1;
    float pausef;
    bool isSmoke;
    bool smokeStart;
    sf::Clock smokeClock;
    float smokeDuration;

public:
    static int bossHp;

    Gamakichi();
    ~Gamakichi();

    bool movements(Tiles* tiles, int tileCount, Player& play,
                   DatabaseManager& db, std::string& username,
                   int& lives, int& gems, int& score, int& levelNo);

    void spawnEnemy();
    void cleanupInactiveSpawns();
    void setPos(float x, float y, float xx, float yy);
    void Draw(sf::RenderWindow& window, Tiles* tiles, int tileCount);
    sf::FloatRect boun();
    void setCol();
    void setNor();
};
