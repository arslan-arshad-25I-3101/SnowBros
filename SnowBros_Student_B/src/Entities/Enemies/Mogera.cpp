#include "Mogera.h"
#include "../Player.h"

int Mogera::bossHp = 900;
int Gamakichi::bossHp = 1500;

//=============================================================================
// MogeraChild
//=============================================================================

bool MogeraChild::checkCollision(Tiles* tiles, int tileCount) {
    auto bounds = spawn->getGlobalBounds();
    sf::FloatRect hitbox({ bounds.position.x + 6.f, bounds.position.y + 6.f },
        { bounds.size.x - 6.f, bounds.size.y - 12.f });
    for (int i = 0; i < tileCount; i++)
        if (hitbox.findIntersection(tiles[i].boun())) return true;
    return false;
}

MogeraChild::MogeraChild() : currentFrame(0), frametime(0.1f), active(true),
                isJump(false), onGround(false), velocityY(0.f), gravity(0.5f) {
    text[0].loadFromFile("spawn/spawn_01.png");
    text[1].loadFromFile("spawn/spawn_02.png");
    text[2].loadFromFile("spawn/spawn_03.png");
    text[3].loadFromFile("spawn/spawn_04.png");
    text[4].loadFromFile("spawn/spawn_05.png");
    text[5].loadFromFile("spawn/spawn_06.png");
    text[6].loadFromFile("spawn/spawn_07.png");
    spawn = new sf::Sprite(text[0]);
}

MogeraChild::MogeraChild(const MogeraChild& other) {
    for (int i = 0; i < 7; i++) text[i] = other.text[i];
    spawn = new sf::Sprite(text[other.currentFrame]);
    spawn->setPosition(other.spawn->getPosition());
    spawn->setOrigin(other.spawn->getOrigin());
    spawn->setScale(other.spawn->getScale());
    currentFrame = other.currentFrame;
    frametime    = other.frametime;
    active       = other.active;
    animClock    = other.animClock;
    velocityY    = other.velocityY;
    gravity      = other.gravity;
    onGround     = other.onGround;
    isJump       = other.isJump;
}

MogeraChild& MogeraChild::operator=(const MogeraChild& other) {
    if (this != &other) {
        delete spawn;
        for (int i = 0; i < 7; i++) text[i] = other.text[i];
        spawn = new sf::Sprite(text[other.currentFrame]);
        spawn->setPosition(other.spawn->getPosition());
        spawn->setOrigin(other.spawn->getOrigin());
        spawn->setScale(other.spawn->getScale());
        currentFrame = other.currentFrame;
        frametime    = other.frametime;
        active       = other.active;
        animClock    = other.animClock;
        velocityY    = other.velocityY;
        gravity      = other.gravity;
        onGround     = other.onGround;
        isJump       = other.isJump;
    }
    return *this;
}

MogeraChild::~MogeraChild() { delete spawn; spawn = nullptr; }

void MogeraChild::setPos(float x, float y, float w, float h) {
    spawn->setOrigin({ w, h });
    spawn->setPosition({ x, y });
}

void MogeraChild::applyGravity(Tiles* tiles, int tileCount) {
    velocityY += gravity;
    float limit = spawn->getPosition().y + velocityY;
    if (limit < 15.f) velocityY = 15.f - spawn->getPosition().y;
    spawn->move({ 0.f, velocityY });

    if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
        spawn->move({ 0.f, -velocityY });
        velocityY = 0.f;
        onGround = true;
        isJump = false;
    } else {
        onGround = false;
        if (animClock.getElapsedTime().asSeconds() > frametime) {
            currentFrame++;
            if (currentFrame >= 7) currentFrame = 5;
            spawn->setTexture(text[currentFrame]);
            animClock.restart();
        }
    }
}

void MogeraChild::movement() {
    spawn->setScale({ 186.f / 725.f, 170.f / 690.f });
    spawn->move({ -1.5f, 0.0f });
    if (animClock.getElapsedTime().asSeconds() > frametime) {
        currentFrame++;
        if (currentFrame >= 7) currentFrame = 0;
        spawn->setTexture(text[currentFrame]);
        animClock.restart();
    }
    if (spawn->getPosition().x > 750.f || spawn->getPosition().y > 550.f || spawn->getPosition().x <= 80.f)
        active = false;
}

bool MogeraChild::isActive() { return active; }

void MogeraChild::Draw(sf::RenderWindow& window) {
    if (active) window.draw(*spawn);
}

sf::FloatRect MogeraChild::getBounds() { return spawn->getGlobalBounds(); }

void vectora(MogeraChild*& a, int* n, MogeraChild v) {
    *n += 1;
    MogeraChild* arr = new MogeraChild[*n];
    for (int i = 0; i < *n; i++) {
        if (i < *n - 1) arr[i] = a[i];
        else            arr[i] = v;
    }
    delete[] a;
    a = arr;
}

//=============================================================================
// Mogera
//=============================================================================

Mogera::Mogera() : bodyFrames(0), legFrames(0), frametime(0.15f),
           spawns(nullptr), spawnCount(0), spawnInterval(3.0f), isDead(true) {
    bodyText[0].loadFromFile("mogera/mogera_01.png");
    bodyText[1].loadFromFile("mogera/mogera_02.png");
    bodyText[2].loadFromFile("mogera/mogera_03.png");
    legText[0].loadFromFile("mogera/mogera_leg_01.png");
    legText[1].loadFromFile("mogera/mogera_leg_02.png");
    legText[2].loadFromFile("mogera/mogera_leg_03.png");
    bossChest = new sf::Sprite(bodyText[0]);
    bossLeg   = new sf::Sprite(legText[0]);
}

Mogera::~Mogera() {
    delete bossChest; bossChest = nullptr;
    delete bossLeg;   bossLeg   = nullptr;
    delete[] spawns;  spawns    = nullptr;
}

bool Mogera::movement(Tiles* tiles, int tileCount, Player& play,
              DatabaseManager& db, std::string& username,
              int& lives, int& gems, int& score, int& levelNo) {
    int x = rand() % 10;
    if (x == 1) {
        bossChest->setScale({ 572.f / 1050.f, 460.f / 990.f });
        bossLeg->setScale({ 504.f / 1000.f, 118.f / 254.f });
        bossChest->move({ 0.f, 0.f });
        bossLeg->move({ 0.f, 0.f });
        if (clock.getElapsedTime().asSeconds() > frametime) {
            bodyFrames++;
            if (bodyFrames >= 3) bodyFrames = 0;
            bossChest->setTexture(bodyText[bodyFrames]);
            bossLeg->setTexture(legText[bodyFrames]);
            clock.restart();
        }
    }

    if (bossLeg->getPosition().y >= 550.f) {
        bossChest->setScale({ 572.f / 1050.f, 460.f / 990.f });
        bossLeg->setScale({ 504.f / 1000.f, 118.f / 254.f });
        bossChest->move({ 0.f, 0.f });
        bossLeg->move({ 0.f, 0.f });
        if (clock.getElapsedTime().asSeconds() > frametime) {
            bodyFrames++;
            if (bodyFrames >= 3) bodyFrames = 0;
            bossChest->setTexture(bodyText[bodyFrames]);
            bossLeg->setTexture(legText[bodyFrames]);
            clock.restart();
        }
    }

    if (spawnClock.getElapsedTime().asSeconds() > spawnInterval) {
        spawnEnemy();
        spawnClock.restart();
    }

    for (int i = 0; i < spawnCount; i++) {
        spawns[i].applyGravity(tiles, tileCount);
        spawns[i].movement();
        if (play.boun().findIntersection(spawns[i].getBounds())) {
            db.saveProgress(username, levelNo, lives - 1, gems, score);
            cleanupInactiveSpawns();
            return true;
        }
    }
    cleanupInactiveSpawns();
    return false;
}

void Mogera::spawnEnemy() {
    MogeraChild newSpawn;
    float y = 0.f;
    int r = rand() % 3;
    if (r == 1) y += 42.85f * 2;
    if (r == 2) y += 42.85f * 4;
    newSpawn.setPos(bossChest->getPosition().x, bossChest->getPosition().y + y);
    vectora(spawns, &spawnCount, newSpawn);
}

void Mogera::cleanupInactiveSpawns() {
    int activeCount = 0;
    for (int i = 0; i < spawnCount; i++)
        if (spawns[i].isActive()) activeCount++;
    if (activeCount == spawnCount) return;
    MogeraChild* newSpawns = new MogeraChild[activeCount];
    int index = 0;
    for (int i = 0; i < spawnCount; i++)
        if (spawns[i].isActive()) newSpawns[index++] = spawns[i];
    delete[] spawns;
    spawns = newSpawns;
    spawnCount = activeCount;
}

void Mogera::setPos(float x, float y, float xx, float yy) {
    bossChest->setOrigin({ xx, yy });
    bossLeg->setOrigin({ xx, yy });
    bossChest->setPosition({ x, y });
    bossLeg->setPosition({ x, y + 205.7f });
}

void Mogera::Draw(sf::RenderWindow& window, Tiles* tiles, int tileCount) {
    window.draw(*bossChest);
    window.draw(*bossLeg);
    for (int i = 0; i < spawnCount; i++) spawns[i].Draw(window);
}

sf::FloatRect Mogera::boun() { return bossChest->getGlobalBounds(); }
void Mogera::setCol()  { bossChest->setColor(sf::Color::Red);   }
void Mogera::setNor()  { bossChest->setColor(sf::Color::White); }
void Mogera::checkdead() { if (bossHp <= 0) isDead = true; }
bool Mogera::getDeath()  { return isDead; }
void Mogera::setDead(bool x) { isDead = x; }

//=============================================================================
// Bomb
//=============================================================================

bool Bomb::checkCollision(Tiles* tiles, int tileCount) {
    auto bounds = spawn->getGlobalBounds();
    sf::FloatRect hitbox({ bounds.position.x + 6.f, bounds.position.y + 6.f },
        { bounds.size.x - 6.f, bounds.size.y - 12.f });
    for (int i = 0; i < tileCount; i++)
        if (hitbox.findIntersection(tiles[i].boun())) return true;
    return false;
}

Bomb::Bomb() : explosionTime(2.0f), hasExploded(false) {
    texts[0].loadFromFile("bomb/bomb_01.png");
    texts[1].loadFromFile("bomb/bomb_02.png");
    texts[2].loadFromFile("bomb/bomb_03.png");
    texts[3].loadFromFile("bomb/bomb_04.png");
    spawns = new sf::Sprite(texts[0]);
    lifeClock.restart();
}

Bomb::Bomb(const Bomb& other) {
    for (int i = 0; i < 4; i++) texts[i] = other.texts[i];
    spawns = new sf::Sprite(texts[other.currentFrame]);
    spawns->setPosition(other.spawns->getPosition());
    spawns->setOrigin(other.spawns->getOrigin());
    spawns->setScale(other.spawns->getScale());
    currentFrame  = other.currentFrame;
    frametime     = other.frametime;
    active        = other.active;
    animClock     = other.animClock;
    explosionTime = other.explosionTime;
    hasExploded   = other.hasExploded;
}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        delete spawns;
        for (int i = 0; i < 4; i++) texts[i] = other.texts[i];
        spawns = new sf::Sprite(texts[other.currentFrame]);
        spawns->setPosition(other.spawns->getPosition());
        spawns->setOrigin(other.spawns->getOrigin());
        spawns->setScale(other.spawns->getScale());
        currentFrame  = other.currentFrame;
        frametime     = other.frametime;
        active        = other.active;
        animClock     = other.animClock;
        explosionTime = other.explosionTime;
        hasExploded   = other.hasExploded;
    }
    return *this;
}

Bomb::~Bomb() { delete spawns; spawns = nullptr; }

void Bomb::setPos(float x, float y, float w, float h) {
    spawns->setOrigin({ w, h });
    spawns->setPosition({ x, y });
}

void Bomb::applyGravity(Tiles* tiles, int tileCount) {
    velocityY += gravity;
    float limit = spawns->getPosition().y + velocityY;
    if (limit < 15.f) velocityY = 15.f - spawns->getPosition().y;
    spawns->move({ 0.f, velocityY });
    if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
        spawns->move({ 0.f, -velocityY });
        velocityY = 0.f;
        onGround = true;
    } else {
        onGround = false;
        if (animClock.getElapsedTime().asSeconds() > frametime) {
            currentFrame++;
            if (currentFrame >= 4) currentFrame = 0;
            spawns->setTexture(texts[currentFrame]);
            animClock.restart();
        }
    }
}

void Bomb::movement() {
    if (!hasExploded && lifeClock.getElapsedTime().asSeconds() >= explosionTime) {
        explode(); return;
    }
    spawns->setScale({ 220.f / 995.f, 250.f / 1090.f });
    spawns->move({ -1.5f, 0.0f });
    if (animClock.getElapsedTime().asSeconds() > frametime) {
        currentFrame++;
        if (currentFrame >= 4) currentFrame = 0;
        spawns->setTexture(texts[currentFrame]);
        animClock.restart();
    }
    if (spawns->getPosition().x > 750.f || spawns->getPosition().y > 550.f || spawns->getPosition().x <= 80.f)
        active = false;
}

void Bomb::explode() { hasExploded = true; active = false; }
void Bomb::Draw(sf::RenderWindow& window) { if (active) window.draw(*spawns); }
bool Bomb::isActive() { return active; }
sf::FloatRect Bomb::getBound() { return spawns->getGlobalBounds(); }

void vectory(Bomb*& a, int* n, Bomb v) {
    *n += 1;
    Bomb* arr = new Bomb[*n];
    for (int i = 0; i < *n; i++) {
        if (i < *n - 1) arr[i] = a[i];
        else            arr[i] = v;
    }
    delete[] a;
    a = arr;
}

//=============================================================================
// Gamakichi
//=============================================================================

Gamakichi::Gamakichi() : speed(1.25f), spawnss(nullptr), mv1(false), pausef(0.f),
              isSmoke(false), smokeStart(false), smokeDuration(3.0f) {
    text[0].loadFromFile("gamakichi/gamakichi_01.png");
    text[1].loadFromFile("gamakichi/gamakichi_02.png");
    boss = new sf::Sprite(text[0]);
    smoke[0].loadFromFile("gamakichi/smoke_01.png");
    smoke[1].loadFromFile("gamakichi/smoke_02.png");
    smoke[2].loadFromFile("gamakichi/smoke_03.png");
    smoke[3].loadFromFile("gamakichi/smoke_04.png");
    for (int i = 0; i < 4; i++) smokey[i] = new sf::Sprite(smoke[0]);
    spawns = nullptr; spawnCount = 0;
}

Gamakichi::~Gamakichi() {
    for (int i = 0; i < 4; i++) delete smokey[i];
    delete boss; boss = nullptr;
    delete[] spawnss; spawnss = nullptr;
}

bool Gamakichi::movements(Tiles* tiles, int tileCount, Player& play,
               DatabaseManager& db, std::string& username,
               int& lives, int& gems, int& score, int& levelNo) {
    boss->setScale({ 1128.f / 3500.f, 622.f / 1200.f });

    if (isSmoke && smokeClock.getElapsedTime().asSeconds() >= smokeDuration)
        isSmoke = false;

    if (!isSmoke) {
        if (!mv1) {
            boss->move({ 0.f, +speed });
            if (clock.getElapsedTime().asSeconds() > frametime) {
                bodyFrames++;
                if (bodyFrames >= 2) bodyFrames = 0;
                boss->setTexture(text[bodyFrames]);
                clock.restart();
            }
        } else {
            boss->move({ 0.f, -speed });
            if (clock.getElapsedTime().asSeconds() > frametime) {
                bodyFrames++;
                if (bodyFrames >= 2) bodyFrames = 0;
                boss->setTexture(text[bodyFrames]);
                clock.restart();
            }
        }
    }

    if (boss->getPosition().y >= 1050 && boss->getPosition().y <= 1100 && !smokeStart) {
        isSmoke = true; smokeStart = true; mv1 = true;
        boss->setTexture(text[1]); pausef = 60;
        smokeClock.restart();
    }
    if (isSmoke) {
        if (clock.getElapsedTime().asSeconds() > frametime) {
            bodyFrames++;
            if (bodyFrames >= 4) bodyFrames = 0;
            for (int i = 0; i < 4; i++) smokey[i]->setTexture(smoke[bodyFrames]);
            clock.restart();
        }
    }
    if (boss->getPosition().y >= 0 && boss->getPosition().y <= 50) {
        mv1 = false; smokeStart = false;
        boss->setTexture(text[1]); pausef = 60;
    }

    if (spawnClock.getElapsedTime().asSeconds() > spawnInterval) {
        spawnEnemy();
        spawnClock.restart();
    }

    for (int i = 0; i < spawnCount; i++) {
        spawnss[i].applyGravity(tiles, tileCount);
        spawnss[i].movement();
        if (play.boun().findIntersection(spawnss[i].getBound())) {
            db.saveProgress(username, levelNo, lives - 1, gems, score);
            cleanupInactiveSpawns();
            return true;
        }
    }
    cleanupInactiveSpawns();
    return false;
}

void Gamakichi::spawnEnemy() {
    Bomb newSpawn;
    newSpawn.setPos(boss->getPosition().x, boss->getPosition().y);
    vectory(spawnss, &spawnCount, newSpawn);
}

void Gamakichi::cleanupInactiveSpawns() {
    int activeCount = 0;
    for (int i = 0; i < spawnCount; i++)
        if (spawnss[i].isActive()) activeCount++;
    if (activeCount == spawnCount) return;
    Bomb* newSpawns = new Bomb[activeCount];
    int index = 0;
    for (int i = 0; i < spawnCount; i++)
        if (spawnss[i].isActive()) newSpawns[index++] = spawnss[i];
    delete[] spawnss;
    spawnss = newSpawns;
    spawnCount = activeCount;
}

void Gamakichi::setPos(float x, float y, float xx, float yy) {
    boss->setOrigin({ xx, yy });
    boss->setPosition({ x, y });
}

void Gamakichi::Draw(sf::RenderWindow& window, Tiles* tiles, int tileCount) {
    if (!isSmoke) window.draw(*boss);
    for (int i = 0; i < spawnCount; i++) spawnss[i].Draw(window);
    if (isSmoke) {
        for (int i = 0; i < 4; i++) {
            smokey[i]->setScale({ 330.f / 870.f, 190.f / 500.f });
            smokey[i]->setPosition({ 175.f + i * 100.f, 540.f });
            window.draw(*smokey[i]);
        }
    }
}

sf::FloatRect Gamakichi::boun() { return boss->getGlobalBounds(); }
void Gamakichi::setCol() { boss->setColor(sf::Color::Red);   }
void Gamakichi::setNor() { boss->setColor(sf::Color::White); }
