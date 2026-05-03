#include "src/Game.h"
#include <cstdlib>
#include <ctime>
#include<cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "src/Levels/LevelData.h"
#include "src/Systems/DatabaseManager.h"
#include "src/Systems/ShopManager.h"


using namespace std;
using namespace sf;

// Global Power-up states
bool hasSpeedBoost = false;
bool hasSnowballPower = false;
bool hasDistanceIncrease = false;
bool hasBalloonMode = false;

// Forward declaration for Tiles
//class Tiles;

class Tiles {
private:
    RectangleShape tile;
    FloatRect bounds;

public:
    Tiles() {
        tile.setSize({ Level::TILE_W, Level::TILE_H });
        tile.setFillColor(Color::Transparent);
        //tile.setFillColor(Color(255, 0, 0, 191));
        //tile.setOutlineThickness(0.f);
    }

    void setpost(float x, float y) {
        tile.setPosition({ x, y });
    }

    void Draw(RenderWindow& window) {
        window.draw(tile);
    }
    float getX() {
        return tile.getPosition().x;
    }
    float getY() {
        return tile.getPosition().y;
    }
    FloatRect boun() {
        return tile.getGlobalBounds();
    }
};

class Botom {
protected:
    bool mv1 = false;
    Texture tex[6];
    Texture spriteSheet;
    IntRect walkFrames[3];
    Clock clocks;
    int frames = 0;
    float frametime = 0.15f;
    Sprite* enemy;
    FloatRect bounds;
    float velocityY = 0.f;      // current vertical speed
    float gravity = 0.5f;     // pulls player down each frame
    float jumpForce = -10.f;    // negative = upward in SFML
    bool onGround = false;    // can only jump if standing on something
    int pausef = 0;
    bool isJump;
    float playerScale = 1.f;
    float speed = 2.25f;
    bool onUpper;
    bool anti;
    bool frozen = false;    // whether encased in snowball
    Clock freezeClock;       // timer for snowball encasement
    IntRect frozenFrames[4]; // 4 stages of ice overlay
    int frozenIndex = 0;    // current frame (0-3)
    Sprite frozenOverlay;   // overlay sprite drawn on top of enemy
    static Texture frozenSharedTexture;  // ONE shared texture for all enemies
    float enemyScale = 1.f;  // scale for rendering

    // Rolling state — after player kicks a fully-frozen enemy
    bool rolling = false;
    float rollingVelocityX = 0.f;
    Clock rollingClock;          // rolling timeout — dies after a few seconds

    // Death state
    bool alive = true;           // dead enemies are skipped entirely
    bool dying = false;          // brief shrink animation before truly dead
    Clock dyingClock;            // timer for death animation

    //for checking gravity like falling
    bool checkCollision(Tiles* tiles, int tileCount) {
        auto bounds = enemy->getGlobalBounds();
        FloatRect hitbox({ bounds.position.x + 6.f, bounds.position.y + 6.f },
            { bounds.size.x - 12.f, bounds.size.y - 12.f });

        for (int i = 0; i < tileCount; i++) {
            if (hitbox.findIntersection(tiles[i].boun())) {
                return true;
            }
        }
        return false;
    }
    //for checking gravity like jumping
    bool upCheckCollision(Tiles* tiles, int tileCount) {
        auto bounds = enemy->getGlobalBounds();
        FloatRect hitbox({ bounds.position.x + 6.f, bounds.position.y + 6.f },
            { bounds.size.x - 12.f, bounds.size.y - 12.f });

        for (int i = 0; i < tileCount; i++) {
            if (hitbox.findIntersection(tiles[i].boun())) {
                return true;
            }
        }
        return false;
    }

public:
    Botom() : enemy(nullptr), frozenOverlay(frozenSharedTexture) {
        tex[0].loadFromFile("botom_orange/botom_orange_walk_11.png");
        tex[1].loadFromFile("botom_orange/botom_orange_walk_12.png");
        tex[2].loadFromFile("botom_orange/botom_orange_walk_13.png");
        tex[3].loadFromFile("botom_orange/Botom_bruh.png");
        tex[4].loadFromFile("botom_orange/Botom_orange_fall.png");
        tex[5].loadFromFile("botom_orange/botom_jump.png");
        bool loaded = spriteSheet.loadFromFile("Sprite Sheet/Botom_Orange.png");

        //enemy = new Sprite(spriteSheet);
        enemy = new Sprite(tex[0]);
        walkFrames[0] = IntRect({ 96, 360 }, { 94, 95 });
        walkFrames[1] = IntRect({ 190, 395 }, { 102, 96 });
        walkFrames[2] = IntRect({ 7, 469 }, { 91, 107 });

        //enemy->setTextureRect(walkFrames[0]);

        float sx = Level::TILE_W / (float)walkFrames[0].size.x;
        float sy = Level::TILE_H / (float)walkFrames[0].size.y;
        enemyScale = (sx < sy) ? sx : sy;  // Scale based on frame size, not full sheet
        enemy->setScale({ enemyScale, enemyScale });
        enemy->setOrigin({ walkFrames[0].size.x / 2.f, walkFrames[0].size.y / 2.f });

        // Load frozen texture once (shared across all enemies)
        if (frozenSharedTexture.getSize().x == 0 || frozenSharedTexture.getSize().y == 0) {
            frozenSharedTexture.loadFromFile("Sprite Sheet/Player_Blue.png");
        }

        // 4-stage ice overlay animation frames
        frozenFrames[0] = IntRect({ 13, 919 }, { 57, 49 });
        frozenFrames[1] = IntRect({ 83, 903 }, { 65, 65 });
        frozenFrames[2] = IntRect({ 176, 899 }, { 72, 70 });
        frozenFrames[3] = IntRect({ 260, 883 }, { 69, 86 });

        frozenIndex = 0;
    }

    float distanceChecker(Tiles* tiles, int tileCount) {
        auto bounds = enemy->getGlobalBounds();
        FloatRect hitbox({ bounds.position.x + 6.f, bounds.position.y + 6.f },
            { bounds.size.x - 12.f, bounds.size.y - 12.f });
        for (int i = 0; i < tileCount; i++) {
            if (hitbox.findIntersection(tiles[i].boun())) {
                for (int j = 0; j < tileCount; j++) {
                    if (tiles[i].getX() == tiles[j].getX() && tiles[j].getY() < tiles[i].getY()) {
                        double distance = hypot(tiles[j].getX() - enemy->getPosition().x,
                            tiles[j].getY() - enemy->getPosition().y);
                        return distance;
                    }
                }
            }
        }
        return 0.f;
    }
    void updateFrozen() {
        if (!frozen) return;

        float elapsed = freezeClock.getElapsedTime().asSeconds();

        // Every 1.5s, melt one stage (decrement frozenIndex)
        if (elapsed > 1.0f) {
            frozenIndex--;
            if (frozenIndex < 0) {
                // Fully melted — enemy is free again
                frozen = false;
                frozenIndex = 0;
            }
            freezeClock.restart(); // restart timer for next melt stage
        }
    }
    void freeze(bool full = false) {
        if (full) {
            frozenIndex = 3; // Max ice stage
            frozen = true;
            freezeClock.restart();
            return;
        }

        if (frozen) {
            // Already frozen — stack more ice (max frame 3)
            if (frozenIndex < 3) {
                frozenIndex++;
            }
        }
        else {
            // First hit — start at frame 0 (lightest ice)
            frozenIndex = 0;
        }
        frozen = true;
        freezeClock.restart(); // reset melt timer on each hit
    }
    void kick(float directionX) {
        rolling = true;
        rollingVelocityX = directionX * 8.f;  // fast horizontal speed
        rollingClock.restart();                // start the timeout
    }
    void kill() {
        // Don't die instantly — enter the "dying" state first
        // This gives us time to play a shrink/flash animation
        dying = true;
        dyingClock.restart();
        // Stop all other behavior
        frozen = false;
        rolling = false;
    }
    void updateDying() {
        if (!dying) return;

        float elapsed = dyingClock.getElapsedTime().asSeconds();
        float duration = 0.4f;  // total death animation time

        if (elapsed < duration) {
            // Shrink from full size → 0 over 0.4 seconds
            //   "elapsed / duration" goes from 0.0 → 1.0
            //   "1.0 - that" goes from 1.0 → 0.0  (shrinking)
            float t = 1.f - (elapsed / duration);
            enemy->setScale({ enemyScale * t, enemyScale * t });
        }
        else {
            // Animation done — truly dead now
            alive = false;
            dying = false;
            enemy->setPosition({ -999.f, -999.f });
        }
    }
    Sprite getFrozenOverlay() {
        // Return the overlay sprite positioned at enemy's position
        // Call this to draw frozen effect on top
        if (!frozen) {
            // Return a sprite that won't draw (we check in main loop)
            Sprite empty(frozenSharedTexture);
            empty.setPosition({ -999.f, -999.f });  // offscreen
            return empty;
        }

        frozenOverlay.setTextureRect(frozenFrames[frozenIndex]);
        frozenOverlay.setPosition(enemy->getPosition());
        frozenOverlay.setOrigin({ frozenFrames[frozenIndex].size.x / 2.f,
                                  frozenFrames[frozenIndex].size.y / 2.f });
        return frozenOverlay;
    }
    enum GravityState { NORMAL, JUMPING_UP };
    GravityState currentState = NORMAL;
    void setGravity(float g) { gravity = g; }
    void applyGravity(Tiles* tiles, int tileCount) {
        velocityY += gravity;
        float limit = enemy->getPosition().y + velocityY;
        if (limit < 15.f) {
            velocityY = 15.f - enemy->getPosition().y;
        }
        enemy->move({ 0.f, velocityY });

        if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
            enemy->move({ 0.f, -velocityY });
            velocityY = 0.f;
            onGround = true;
            isJump = false;
        }
        else {
            onGround = false;
            if (velocityY < 0)
                enemy->setTexture(tex[4]);
            else
                enemy->setTexture(tex[4]);
        }
    }
    void antiGravity(Tiles* tiles, int tileCount) {
        if (currentState == NORMAL && upCheckCollision(tiles, tileCount)) {
            // Start the jump
            float distance = distanceChecker(tiles, tileCount);
            velocityY = -5.0f;  // Initial jump speed
            currentState = JUMPING_UP;
            enemy->setTexture(tex[5]);
        }

        if (currentState == JUMPING_UP) {
            velocityY += gravity * 0.5f;  // Slower gravity while jumping up
            enemy->move({ 0.f, velocityY });

            // Check if reached upper tile
            if (upCheckCollision(tiles, tileCount) && velocityY >= 0) {
                enemy->move({ 0.0f, -velocityY });
                velocityY = 0.0f;
                currentState = NORMAL;  // Reset
                onUpper = true;
            }
        }
    }
    bool getAnti() {
        return anti;
    }

    bool checkTileBelow(Tiles* tiles, int tileCount) {
        auto bounds = enemy->getGlobalBounds();
        // Check a bit below the enemy's feet
        FloatRect checkArea({ bounds.position.x + 6.f, bounds.position.y + bounds.size.y },
            { bounds.size.x - 20.f, 4.f }); // 5 pixels below

        for (int i = 0; i < tileCount; i++) {
            if (checkArea.findIntersection(tiles[i].boun())) {
                return true;
            }
        }
        return false;
    }
    void movement(Tiles* tiles, int tileCount) {
        if (!alive || dying) return;  // dead/dying enemies do nothing
        if (enemy->getPosition().y > 650.f) {
            kill();
            return;
        }
        // --- ROLLING STATE: fast horizontal movement with gravity ---
        if (rolling) {
            // Timeout: after 3 seconds of rolling, the snowball breaks
            if (rollingClock.getElapsedTime().asSeconds() > 3.f) {
                kill();
                return;
            }

            // Gravity still applies while rolling
            velocityY += gravity;
            enemy->move({ 0.f, velocityY });

            if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
                enemy->move({ 0.f, -velocityY });
                velocityY = 0.f;
            }

            // Move horizontally at high speed
            enemy->move({ rollingVelocityX, 0.f });

            // Screen wrap or break (classic Snow Bros behavior)
            auto pos = enemy->getPosition();
            if (pos.y > 500.f) {
                // On the bottom floor, the rolling snowball breaks when going off-screen
                if (pos.x > 800.f || pos.x < 0.f) {
                    kill();
                }
            }
            else {
                // On higher floors, it wraps around
                if (pos.x > 800.f) enemy->setPosition({ 0.f, pos.y });
                if (pos.x < 0.f)   enemy->setPosition({ 800.f, pos.y });
            }

            return; // skip normal walking logic
        }
        if (frozen) return;

        //functiosn related to botom movement start from here
        if (mv1 == true) {
            enemy->move({ -speed, -0.0f });
            enemy->setScale({ 89.f / 185.f,97.f / 185.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                //enemy->setTextureRect(walkFrames[frames]);
                enemy->setTexture(tex[frames]);
                clocks.restart();

            }
        }
        else if (mv1 == false) {
            enemy->move({ +speed, -0.0f });
            enemy->setScale({ -89.f / 185.f,97.f / 185.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                //enemy->setTextureRect(walkFrames[frames]);
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }


        if (enemy->getPosition().x >= 750 && enemy->getPosition().x <= 800) {
            mv1 = true;
            //enemy->setTextureRect(walkFrames[2]);
            enemy->setTexture(tex[3]);
            pausef = 60;
        }
        if (enemy->getPosition().x >= 0 && enemy->getPosition().x <= 50) {
            mv1 = false;
            //enemy->setTextureRect(walkFrames[2]);
            enemy->setTexture(tex[3]);
            pausef = 60;
        }

    }
    Sprite getEnemy() {
        return *enemy;
    }
    void setPos(float i) {
        enemy->setPosition({ 350.f, 65.f + i * 65.f });
    }
    void setPos(int row, int col) {
        enemy->setPosition({ col * Level::TILE_W + Level::TILE_W / 2.f,
                             row * Level::TILE_H + Level::TILE_H / 2.f });
    }
    void setPos(float x, float y, float xx, float yy) {
        enemy->setOrigin({ xx,yy });
        enemy->setPosition({ x,y });
    }
    void setOrigin(float x, float y) {
        enemy->setOrigin({ x,y });
    }
    void setmv1(bool x) {
        mv1 = x;
    }
    bool getmv1() {
        return mv1;
    }
    void setScale(float x, float y) {
        enemy->setScale({ 97.f / x,89.f / y });
    }
    bool isFrozen() {
        return frozen;
    }
    bool isFullyFrozen() {
        return frozen && frozenIndex == 3;
    }
    bool isRolling() {
        return rolling;
    }
    bool isDying() {
        return dying;
    }
    bool isAlive() {
        return alive && !dying;  // dying enemies count as "not alive" for gameplay
    }
    FloatRect boun() {
        return enemy->getGlobalBounds();
    }
    void reset() {
        alive = true;
        dying = false;
        frozen = false;
        rolling = false;
        frozenIndex = 0;
        velocityY = 0.f;
        rollingVelocityX = 0.f;
    }
    ~Botom() {
        delete enemy;
        enemy = nullptr;
    }
};

class Fooga : public Botom {
protected:
    bool isFlying;
    float velocity_X = 0.125f;
public:
    Fooga() {
        tex[0].loadFromFile("fooga/flying_01.png");
        tex[1].loadFromFile("fooga/flying_02.png");
        tex[2].loadFromFile("fooga/flying_03.png");
        tex[3].loadFromFile("fooga/flying_01.png");
        tex[4].loadFromFile("fooga/flying_01.png");
        enemy = new Sprite(tex[0]);
    }
    void Axis_move() {
        enemy->move({ speed + velocity_X, speed + velocityY });
    }
    void Ariel_move() {
        enemy->move({ speed - velocity_X, speed - velocityY });
    }
    void movement(Tiles* tiles, int tileCount) {
        if (!alive || dying) return;
        if (rolling) {
            // Timeout: after 3 seconds of rolling, the snowball breaks
            if (rollingClock.getElapsedTime().asSeconds() > 3.f) {
                kill();
                return;
            }

            // Gravity still applies while rolling
            velocityY += gravity;
            enemy->move({ 0.f, velocityY });

            if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
                enemy->move({ 0.f, -velocityY });
                velocityY = 0.f;
            }

            // Move horizontally at high speed
            enemy->move({ rollingVelocityX, 0.f });

            // Screen wrap or break (classic Snow Bros behavior)
            auto pos = enemy->getPosition();
            if (pos.y > 500.f) {
                // On the bottom floor, the rolling snowball breaks when going off-screen
                if (pos.x > 800.f || pos.x < 0.f) {
                    kill();
                }
            }
            else {
                // On higher floors, it wraps around
                if (pos.x > 800.f) enemy->setPosition({ 0.f, pos.y });
                if (pos.x < 0.f)   enemy->setPosition({ 800.f, pos.y });
            }

            return; // skip normal walking logic
        }
        if (frozen) return;
        //things related to fooga flying start from here
        if (mv1 == true && (enemy->getPosition().y <= 550 && enemy->getPosition().y >= 35)) {
            velocity_X -= 0.0625f;
            velocityY = velocityY;
            enemy->move({ -speed, -velocityY + speed });
            enemy->setScale({ 177.f / 455.f,180.f / 455.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();

            }
        }
        else if (mv1 == false && (enemy->getPosition().y <= 550 && enemy->getPosition().y >= 35)) {
            velocity_X -= 0.0625f;
            velocityY = velocityY;
            enemy->move({ +speed, +velocityY });
            enemy->setScale({ -177.f / 455.f,180.f / 455.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }
        else if (enemy->getPosition().y < 25.f) {
            velocity_X += 0.0625f;
            velocityY += 0.75f;
            enemy->move({ speed, velocityY });
            enemy->setScale({ -177.f / 455.f,180.f / 455.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }

        else {
            velocity_X -= 0.0625f;
            velocityY -= 0.5f;
            enemy->move({ speed, velocityY });
            enemy->setScale({ -177.f / 455.f,180.f / 455.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }
        if (enemy->getPosition().x >= 725 && enemy->getPosition().x <= 800) {
            mv1 = true;
            enemy->setTexture(tex[3]);
            pausef = 60;
            velocity_X -= 0.0625f;
            velocityY = -velocityY;


        }
        if (enemy->getPosition().x >= 0 && enemy->getPosition().x <= 50) {
            mv1 = false;
            enemy->setTexture(tex[3]);
            pausef = 60;
            velocity_X += 0.0625f;
            velocityY = -velocityY;
        }
        if (enemy->getPosition().y >= 0 && enemy->getPosition().y <= 15) {
            mv1 = false;
            enemy->setTexture(tex[1]);
            pausef = 60;
            velocity_X += 0.0625;
            velocityY = velocityY;
        }
        if (enemy->getPosition().y >= 550 && enemy->getPosition().y <= 600) {
            mv1 = false;
            enemy->setTexture(tex[1]);
            pausef = 60;
            velocity_X -= 0.25f;
            velocityY -= 0.5f;
        }
        if (enemy->getPosition().x <= 0 && enemy->getPosition().y <= 0) {
            mv1 = false;
            enemy->setTexture(tex[1]);
            pausef = 60;
            velocity_X += 0.0625;
            velocityY = -velocityY;
        }
    }


};

class Tornado : public Fooga {
protected:
Texture spin[8];
int spinFrames = 0;
public:
    Tornado() {
    tex[0].loadFromFile("tornado/tornado_blue_idle.png");
    tex[1].loadFromFile("tornado/tornado_blue_01.png");
    tex[2].loadFromFile("tornado/tornado_blue_02.png");
    tex[3].loadFromFile("tornado/tornado_blue_03.png");
    tex[4].loadFromFile("tornado/tornado_blue_03.png");
    tex[5].loadFromFile("tornado/tornado_blue_03.png");
    enemy->setTexture(tex[0]);
    spin[0].loadFromFile("tornado/tornado_blue_spin_01.png");
    spin[1].loadFromFile("tornado/tornado_blue_spin_02.png");
    spin[2].loadFromFile("tornado/tornado_blue_spin_03.png");
    spin[3].loadFromFile("tornado/tornado_blue_spin_04.png");
    spin[4].loadFromFile("tornado/tornado_blue_spin_05.png");
    spin[5].loadFromFile("tornado/tornado_blue_spin_06.png");
    spin[6].loadFromFile("tornado/tornado_blue_spin_07.png");
    spin[7].loadFromFile("tornado/tornado_blue_spin_08.png");

   
}
    //enum MovingState { NORMAL, SPINNING };
    //MovingState currentState = SPINNING;
    void movement_spin(Tiles* tiles, int tileCount) {
        if (!alive || dying) return;
        if (rolling) {
            // Timeout: after 3 seconds of rolling, the snowball breaks
            if (rollingClock.getElapsedTime().asSeconds() > 3.f) {
                kill();
                return;
            }

            // Gravity still applies while rolling
            velocityY += gravity;
            enemy->move({ 0.f, velocityY });

            if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
                enemy->move({ 0.f, -velocityY });
                velocityY = 0.f;
            }

            // Move horizontally at high speed
            enemy->move({ rollingVelocityX, 0.f });

            // Screen wrap or break (classic Snow Bros behavior)
            auto pos = enemy->getPosition();
            if (pos.y > 500.f) {
                // On the bottom floor, the rolling snowball breaks when going off-screen
                if (pos.x > 800.f || pos.x < 0.f) {
                    kill();
                }
            }
            else {
                // On higher floors, it wraps around
                if (pos.x > 800.f) enemy->setPosition({ 0.f, pos.y });
                if (pos.x < 0.f)   enemy->setPosition({ 800.f, pos.y });
            }

            return; // skip normal walking logic
        }
        if (frozen) return;
      
        if (mv1 == true && (enemy->getPosition().y <= 550 && enemy->getPosition().y >= 35)) {
            velocity_X -= 0.0625f;
            velocityY = velocityY;
            enemy->move({ -speed, -velocityY + speed });
            enemy->setScale({ 131.f / 325.f,122.f / 325.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (spinFrames)++;
                if (spinFrames >= 8)
                    spinFrames = 4;
                enemy->setTexture(spin[spinFrames]);
                clocks.restart();
                //currentState = NORMAL;
            }
        }
        else if (mv1 == false && (enemy->getPosition().y <= 550 && enemy->getPosition().y >= 35)) {
            velocity_X -= 0.0625f;
            velocityY = velocityY;
            enemy->move({ +speed, +velocityY });
            enemy->setScale({ -131.f / 325.f,122.f / 325.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (spinFrames)++;
                if (spinFrames >= 8)
                    spinFrames = 4;
                enemy->setTexture(spin[spinFrames]);
                clocks.restart();
               // currentState = NORMAL;
            }
        }
        else if (enemy->getPosition().y < 25.f) {
            velocity_X += 0.0625f;
            velocityY += 0.75f;
            enemy->move({ speed, velocityY });
            enemy->setScale({ -131.f / 325.f,122.f / 325.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (spinFrames)++;
                if (spinFrames >= 8)
                    spinFrames = 4;
                enemy->setTexture(spin[spinFrames]);
                clocks.restart();
                //currentState = NORMAL;
            }
        }

        else {
            velocity_X -= 0.0625f;
            velocityY -= 0.5f;
            enemy->move({ speed, velocityY });
            enemy->setScale({ -131.f / 325.f,122.f / 325.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (spinFrames)++;
                if (spinFrames >= 8)
                    spinFrames = 4;
                enemy->setTexture(spin[spinFrames]);
                clocks.restart();
                //currentState = NORMAL;
            }
        }
        if (enemy->getPosition().x >= 725 && enemy->getPosition().x <= 800) {
            mv1 = true;
            enemy->setTexture(tex[3]);
            pausef = 60;
            velocity_X -= 0.0625f;
            velocityY = -velocityY;


        }
        if (enemy->getPosition().x >= 0 && enemy->getPosition().x <= 50) {
            mv1 = false;
            enemy->setTexture(tex[3]);
            pausef = 60;
            velocity_X += 0.0625f;
            velocityY = -velocityY;
        }
        if (enemy->getPosition().y >= 0 && enemy->getPosition().y <= 15) {
            mv1 = false;
            enemy->setTexture(tex[1]);
            pausef = 60;
            velocity_X += 0.0625;
            velocityY = velocityY;
        }
        if (enemy->getPosition().y >= 550 && enemy->getPosition().y <= 600) {
            mv1 = false;
            enemy->setTexture(tex[1]);
            pausef = 60;
            velocity_X -= 0.25f;
            velocityY -= 0.5f;
        }
        if (enemy->getPosition().x <= 0 && enemy->getPosition().y <= 0) {
            mv1 = false;
            enemy->setTexture(tex[1]);
            pausef = 60;
            velocity_X += 0.0625;
            velocityY = -velocityY;
        }
    }
    void movement_normal(Tiles* tiles, int tileCount) {
        if (!alive || dying) return;  // dead/dying enemies do nothing
        if (enemy->getPosition().y > 650.f) {
            kill();
            return;
        }
        // --- ROLLING STATE: fast horizontal movement with gravity ---
        if (rolling) {
            // Timeout: after 3 seconds of rolling, the snowball breaks
            if (rollingClock.getElapsedTime().asSeconds() > 3.f) {
                kill();
                return;
            }

            // Gravity still applies while rolling
            velocityY += gravity;
            enemy->move({ 0.f, velocityY });

            if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
                enemy->move({ 0.f, -velocityY });
                velocityY = 0.f;
            }

            // Move horizontally at high speed
            enemy->move({ rollingVelocityX, 0.f });

            // Screen wrap or break (classic Snow Bros behavior)
            auto pos = enemy->getPosition();
            if (pos.y > 500.f) {
                // On the bottom floor, the rolling snowball breaks when going off-screen
                if (pos.x > 800.f || pos.x < 0.f) {
                    kill();
                }
            }
            else {
                // On higher floors, it wraps around
                if (pos.x > 800.f) enemy->setPosition({ 0.f, pos.y });
                if (pos.x < 0.f)   enemy->setPosition({ 800.f, pos.y });
            }

            return; // skip normal walking logic
        }
        if (frozen) return;

        //functiosn related to botom movement start from here
        if (mv1 == true) {
            enemy->move({ -speed, -0.0f });
            enemy->setScale({ 131.f / 325.f,122.f / 325.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 4)
                    frames = 1;
                //enemy->setTextureRect(walkFrames[frames]);
                enemy->setTexture(tex[frames]);
                clocks.restart();
                //currentState = SPINNING;
            }
        }
        else if (mv1 == false) {
            enemy->move({ +speed, -0.0f });
            enemy->setScale({ -131.f / 325.f,122.f / 325.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 4)
                    frames = 1;
                //enemy->setTextureRect(walkFrames[frames]);
                enemy->setTexture(tex[frames]);
                clocks.restart();
                //currentState = SPINNING;
            }
        }


        if (enemy->getPosition().x >= 750 && enemy->getPosition().x <= 800) {
            mv1 = true;
            //enemy->setTextureRect(walkFrames[2]);
            enemy->setTexture(tex[3]);
            pausef = 60;
        }
        if (enemy->getPosition().x >= 0 && enemy->getPosition().x <= 50) {
            mv1 = false;
            //enemy->setTextureRect(walkFrames[2]);
            enemy->setTexture(tex[3]);
            pausef = 60;
        }

    }
};

void mover(int n, Botom* other, Tiles* tiles, int tileCount) {
    for (int i = 0; i < n; i++) {
        if (!other[i].isAlive()) continue;  // skip dead
        other[i].movement(tiles, tileCount);
    }
}

void Draw(int n, Botom* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        if (!other[i].isAlive() && !other[i].isDying()) continue;  // skip fully dead, but draw dying (shrink anim)
        window.draw(other[i].getEnemy());
    }
}

void mover(int n, Fooga* other, Tiles* tiles, int tileCount) {
    for (int i = 0; i < n; i++)
        other[i].movement(tiles, tileCount);
}

void Draw(int n, Fooga* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        if (!other[i].isAlive() && !other[i].isDying()) continue;  // skip fully dead, but draw dying (shrink anim)
        window.draw(other[i].getEnemy());
    }
}

void Draw(int n, Tornado* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        if (!other[i].isAlive() && !other[i].isDying()) continue;  // skip fully dead, but draw dying (shrink anim)
        window.draw(other[i].getEnemy());
    }
}

void mover(int n, Tornado* other, Tiles* tiles, int tileCount) {
    for (int i = 0; i < n; i++) {
        int r = rand()%50;
        if(r != 1)
        other[i].movement_normal(tiles, tileCount);
        else if( r == 1)
        other[i].movement_spin(tiles, tileCount);
    }
}

void setPos(float x, float y, int n, Botom* other) {
    for (int i = 0; i < n; i++) {
        other[i].setOrigin(x, y);
        other[i].setPos(i);
        if (i % 2 == 1)
            other[i].setmv1(false);
        else
            other[i].setmv1(true);
    }
}

void Gravity(int n, Botom* other, Tiles* tiles, int c) {
    for (int i = 0; i < n; i++) {
        if (other[i].currentState == Botom::JUMPING_UP) {
            other[i].antiGravity(tiles, c);
        }
        else {
            bool isTile = other[i].checkTileBelow(tiles, c);
            if (!isTile) {
                other[i].applyGravity(tiles, c);
            }
            else {
                int x = rand() % 50;
                if (x == 1)
                    other[i].antiGravity(tiles, c);
                else if (x == 2)
                    other[i].applyGravity(tiles, c);

            }
        }
    }
}

void Gravity(int n, Fooga* other, Tiles* tiles, int c) {
    for (int i = 0; i < n; i++) {
        if (other[i].isFullyFrozen() || other[i].isFrozen()) {
            bool isTile = other[i].checkTileBelow(tiles, c);
            if (!isTile) {
                other[i].applyGravity(tiles, c);
            }
        }
    }
}

void Gravity(Tornado* other, int n, Tiles* tiles, int c) {
    for (int i = 0; i < n; i++) {
        if (other[i].currentState == Tornado::JUMPING_UP) {
            other[i].antiGravity(tiles, c);
        }
        else {
            bool isTile = other[i].checkTileBelow(tiles, c);
            if (!isTile) {
                other[i].applyGravity(tiles, c);
            }
            else {
                int x = rand() % 50;
                if (x == 1)
                    other[i].antiGravity(tiles, c);
                else if(x == 2)
                    other[i].applyGravity(tiles, c);

            }
        }
    }
}

//////////////////////////////// SNOWBALL CLASS //////////////////////////////////////

class Snowball {
public:
    static Texture sharedTexture;
    Sprite snowball;
    IntRect projFrames[2]; // [0] flying snowball, [1] burst on impact
    int frameIndex;
    float renderScale;
    float velocityX;
    float velocityY;
    float gravity;
    bool active;    // state 1
    bool bursting; // state 2
    Clock clock;
    Clock burstClock;

    Snowball() : snowball(sharedTexture) {
        if (sharedTexture.getSize().x == 0 || sharedTexture.getSize().y == 0) {
            sharedTexture.loadFromFile("Sprite Sheet/Items.png");
        }
        snowball.setTexture(sharedTexture);

        // Flying and burst frames
        projFrames[0] = IntRect({ 307, 1121 }, { 179, 152 }); // blue blob
        projFrames[1] = IntRect({ 487, 1115 }, { 161, 161 }); // orange burst

        frameIndex = 0;
        snowball.setTextureRect(projFrames[frameIndex]);
        snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

        float sx = (Level::TILE_W * 0.8f) / (float)projFrames[frameIndex].size.x;
        float sy = (Level::TILE_H * 0.8f) / (float)projFrames[frameIndex].size.y;
        renderScale = (sx < sy) ? sx : sy;
        snowball.setScale({ renderScale, renderScale });

        active = false;
        bursting = false;
        velocityX = 0.f;
        velocityY = 0.f;
        gravity = 0.3f;
    }

    void spawn(float x, float y, float directionX) {
        // Reset state for a fresh projectile
        bursting = false;

        float spawnX = x + directionX * (Level::TILE_W * 0.65f);
        float spawnY = y - Level::TILE_H * 0.15f;
        snowball.setPosition({ spawnX, spawnY });

        velocityX = directionX * 4.5f;
        velocityY = -3.0f;

        frameIndex = 0;
        snowball.setTextureRect(projFrames[frameIndex]);
        snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

        if (directionX > 0.f)
            snowball.setScale({ -renderScale, renderScale });
        else
            snowball.setScale({ renderScale, renderScale });

        active = true;
        clock.restart();
    }

    void update(Tiles* tiles, int tileCount) {
        if (!active) return;

        // If already bursting, show burst briefly then remove projectile.
        if (bursting) {
            if (burstClock.getElapsedTime().asSeconds() > 0.12f) {
                active = false;
                bursting = false;
            }
            return;
        }

        // Save previous-frame hitbox before movement.
        auto before = snowball.getGlobalBounds();
        float insetX = before.size.x * 0.22f;
        float insetY = before.size.y * 0.28f;
        FloatRect prevHitbox({ before.position.x + insetX, before.position.y + insetY },
            { before.size.x - insetX * 2.f, before.size.y - insetY * 2.f });

        // Projectile motion
        velocityY += gravity;
        snowball.move({ velocityX, velocityY });

        // Check collision with floor only while falling. 
        // This prevents "head hit" while moving upward.
        if (velocityY >= 0.f) {
            auto sb = snowball.getGlobalBounds();
            FloatRect hitbox({ sb.position.x + insetX, sb.position.y + insetY },
                { sb.size.x - insetX * 2.f, sb.size.y - insetY * 2.f });

            for (int i = 0; i < tileCount; i++) {
                FloatRect tile = tiles[i].boun();
                if (hitbox.findIntersection(tile)) {
                    float prevBottom = prevHitbox.position.y + prevHitbox.size.y;
                    float tileTop = tile.position.y;

                    // Top-surface hit only
                    if (prevBottom <= tileTop + 4.f) {
                        bursting = true;
                        velocityX = 0.f;
                        velocityY = 0.f;

                        frameIndex = 1;
                        snowball.setTextureRect(projFrames[frameIndex]);
                        snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

                        burstClock.restart();
                        return;
                    }
                }
            }
        }

        auto pos = snowball.getPosition();

        // Screen wrap horizontally
        if (pos.x > 800.f) snowball.setPosition({ 0.f, pos.y });
        if (pos.x < 0.f)   snowball.setPosition({ 800.f, pos.y });

        // End projectile after short range (or long range if power-up active)
        float maxTime = hasDistanceIncrease ? 2.5f : 0.6f;
        if (pos.y > 600.f || clock.getElapsedTime().asSeconds() > maxTime) {
            active = false;
        }
    }

    void draw(RenderWindow& window) {
        if (active) window.draw(snowball);
    }

    FloatRect getBounds() {
        return snowball.getGlobalBounds();
    }
};

bool snowballHitsEnemy(Snowball& snowball, Botom& enemy, bool hasPower) {
    if (!snowball.active) return false;

    if (snowball.getBounds().findIntersection(enemy.boun())) {
        enemy.freeze(hasPower);
        return true;
    }

    return false;
}

bool snowballHitsEnemy(Snowball& snowball, Fooga& enemy, Tiles* tiles, int tileCount, bool hasPower) {
    if (!snowball.active) return false;

    if (snowball.getBounds().findIntersection(enemy.boun())) {
        enemy.freeze(hasPower);
        enemy.applyGravity(tiles, tileCount);
        return true;
    }

    return false;
}



//////////////////////////////// SNOWBALL end //////////////////////////////////////

class Player : public Botom {
protected:
    float movementSpeed = 2.25f;

public:
    Player() {
        tex[0].loadFromFile("Characters/Enemies/nick_1.png");
        tex[1].loadFromFile("Characters/Enemies/nick_2.png");
        tex[2].loadFromFile("Characters/Enemies/nick_3.png");
        tex[3].loadFromFile("Characters/Enemies/nick_0.png"); // stationary 
        delete enemy;
        enemy = new Sprite(tex[0]);

        float sx = Level::TILE_W / (float)tex[0].getSize().x;
        float sy = Level::TILE_H / (float)tex[0].getSize().y;
        playerScale = (sx < sy) ? sx : sy;
        enemy->setScale({ playerScale, playerScale });
        enemy->setOrigin({ tex[0].getSize().x / 2.f, tex[0].getSize().y / 2.f });
    }

    void setSpeed(float s) { movementSpeed = s; }

    void applyGravity(Tiles* tiles, int tileCount) {
        velocityY += gravity;
        enemy->move({ 0.f, velocityY });

        if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
            enemy->move({ 0.f, -velocityY });
            velocityY = 0.f;
            onGround = true;

        }
        else {
            onGround = false;
            if (velocityY < 0)
                enemy->setTexture(tex[1]);
            else
                enemy->setTexture(tex[2]);
        }
    }

    void move(Keyboard::Key key, Tiles* tiles, int tileCount) {

        if (key == Keyboard::Key::A) {
            enemy->move({ -movementSpeed, -0.0f });
            if (checkCollision(tiles, tileCount)) {
                enemy->move({ movementSpeed, -0.0f }); // undo movement
            } else {
                enemy->setScale({ playerScale, playerScale });
                if (clocks.getElapsedTime().asSeconds() > frametime) {
                    frames++;
                    if (frames >= 3)
                        frames = 0;
                    enemy->setTexture(tex[frames]);
                    clocks.restart();
                }
            }
        }
        if (key == Keyboard::Key::D) {
            enemy->move({ +movementSpeed, -0.0f });
            if (checkCollision(tiles, tileCount)) {
                enemy->move({ -movementSpeed, -0.0f }); // undo movement
            } else {
                enemy->setScale({ -playerScale, playerScale });
                if (clocks.getElapsedTime().asSeconds() > frametime) {
                    frames++;
                    if (frames >= 3)
                        frames = 0;
                    enemy->setTexture(tex[frames]);
                    clocks.restart();
                }
            }
        }

        // W key just LAUNCHES the player upward
        if (key == Keyboard::Key::W) {
            if (onGround) {             // can't double jump
                velocityY = jumpForce;  // -10.f  shoots upward
                onGround = false;
            }
        }
    }

    float getDirectionX() {
        return (enemy->getScale().x > 0.f) ? -1.f : 1.f; // +scale is A (left), -scale is D (right)
    }

    sf::Vector2f getPosition() {
        return enemy->getPosition();
    }

    Sprite Draw() {
        return *enemy;
    }
    ~Player() {
    }
};

/////////////////// LOADING LEVELS //////////////////////////////

void LoadLevel(
    int levelNo,
    Level& level,
    Texture& bgTex,
    Sprite& background,
    Tiles*& tilt,
    int& count)
{
    // choose level
    if (levelNo == 1) SetupLevel1(level);
    else if (levelNo == 2) SetupLevel2(level);
    else if (levelNo == 3) SetupLevel3(level);
    else if (levelNo == 4) SetupLevel4(level);
    else if (levelNo == 5) SetupLevel5(level);
    else if (levelNo == 6) SetupLevel6(level);
    else if (levelNo == 7) SetupLevel7(level);
    else if (levelNo == 8) SetupLevel8(level);
    else if (levelNo == 9) SetupLevel9(level);
    else if (levelNo == 10) SetupLevel10(level);

    bgTex.loadFromFile(level.backgroundPath);
    background = Sprite(bgTex);

    // CHANGE LATER (check if the re-scale is needed)

    if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
        level.bgRect = IntRect({ 0, 0 }, { (int)bgTex.getSize().x, (int)bgTex.getSize().y });
        background.setTextureRect(level.bgRect);
        background.setScale({ 800.f / (float)bgTex.getSize().x, 600.f / (float)bgTex.getSize().y });
    }

    if (tilt != nullptr) {
        delete[] tilt;
        tilt = nullptr;
    }

    count = 0;
    for (int r = 0; r < Level::ROWS; r++)
        for (int c = 0; c < Level::COLS; c++)
            if (level.grid[r][c] != 0) count++;

    tilt = new Tiles[count];
    int idx = 0;
    for (int r = 0; r < Level::ROWS; r++) {
        for (int c = 0; c < Level::COLS; c++) {
            if (level.grid[r][c] != 0) {
                tilt[idx].setpost(c * Level::TILE_W, r * Level::TILE_H);
                idx++;
            }
        }
    }
}

///////////////////////////////////// LEVEL LOADING END ////////////////////////////////////

//---------------------------------MOGERAS SON JOHNs -----------------------------------

class MogeraChild {
protected:
    Sprite* spawn;
    Texture text[7];
    int currentFrame = 0;
    float frametime = 0.1f;
    Clock animClock;
    bool active = true;
    bool isJump;
    bool unUpper;
    bool onGround = false;
    float velocityY = 0.0f;
    float gravity = 0.5f;
    bool checkCollision(Tiles* tiles, int tileCount) {
        auto bounds = spawn->getGlobalBounds();
        FloatRect hitbox({ bounds.position.x + 6.f, bounds.position.y + 6.f },
            { bounds.size.x - 6.f, bounds.size.y - 12.f });

        for (int i = 0; i < tileCount; i++) {
            if (hitbox.findIntersection(tiles[i].boun())) {
                return true;
            }
        }
        return false;
    }
public:
    MogeraChild() {
        text[0].loadFromFile("spawn/spawn_01.png");
        text[1].loadFromFile("spawn/spawn_02.png");
        text[2].loadFromFile("spawn/spawn_03.png");
        text[3].loadFromFile("spawn/spawn_04.png");
        text[4].loadFromFile("spawn/spawn_05.png");
        text[5].loadFromFile("spawn/spawn_06.png");
        text[6].loadFromFile("spawn/spawn_07.png");
        spawn = new Sprite(text[0]);
    }
    MogeraChild(const MogeraChild& other) {
        for (int i = 0; i < 7; i++) {
            text[i] = other.text[i];
        }
        spawn = new Sprite(text[other.currentFrame]);
        spawn->setPosition(other.spawn->getPosition());
        spawn->setOrigin(other.spawn->getOrigin());
        spawn->setScale(other.spawn->getScale());

        currentFrame = other.currentFrame;
        frametime = other.frametime;
        active = other.active;
        animClock = other.animClock;
    }
    MogeraChild& operator=(const MogeraChild& other) {
        if (this != &other) {
            delete spawn;
            for (int i = 0; i < 7; i++) {
                text[i] = other.text[i];
            }
            spawn = new Sprite(text[other.currentFrame]);
            spawn->setPosition(other.spawn->getPosition());
            spawn->setOrigin(other.spawn->getOrigin());
            spawn->setScale(other.spawn->getScale());

            currentFrame = other.currentFrame;
            frametime = other.frametime;
            active = other.active;
            animClock = other.animClock;
        }
        return *this;
    }
    void setPos(float x, float y, float w = 186.f, float h = 170.f) {
        spawn->setOrigin({ w, h });
        spawn->setPosition({ x, y });
    }
    enum GravityState { WALKING, JUMPING_UP };
    GravityState currentState = WALKING;
    void applyGravity(Tiles* tiles, int tileCount) {
        velocityY += gravity;
        float limit = spawn->getPosition().y + velocityY;
        if (limit < 15.f) {
            velocityY = 15.f - spawn->getPosition().y;
        }
        spawn->move({ 0.f, velocityY });

        if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
            spawn->move({ 0.f, -velocityY });
            velocityY = 0.f;
            onGround = true;
            isJump = false;
        }
        else {
            onGround = false;
            if (velocityY < 0){
                if (animClock.getElapsedTime().asSeconds() > frametime) {
                    currentFrame++;
                    if (currentFrame >= 7)
                        currentFrame = 5;
                    spawn->setTexture(text[currentFrame]);
                    animClock.restart();
                }
              }
            else {
                if (animClock.getElapsedTime().asSeconds() > frametime) {
                    currentFrame++;
                    if (currentFrame >= 7)
                        currentFrame = 5;
                    spawn->setTexture(text[currentFrame]);
                    animClock.restart();
                }
            }
                
        }
    }
    void movement() {
    spawn->setScale({186.f/725.f, 170.f/690.f});
        spawn->move({ -1.5f, 0.0f });

        if (animClock.getElapsedTime().asSeconds() > frametime) {
            currentFrame++;
            if (currentFrame >= 7)
                currentFrame = 0;
            spawn->setTexture(text[currentFrame]);
            animClock.restart();
        }

        if (spawn->getPosition().x > 750.f || spawn->getPosition().y > 550.f || spawn->getPosition().x <= 80.f) {
            active = false;
        }
    }

    bool isActive() {
        return active;
    }

    void Draw(RenderWindow& window) {
        if (active) {
            window.draw(*spawn);
        }
    }

    ~MogeraChild() {
        delete spawn;
        spawn = nullptr;
    }
};


void vectora(MogeraChild*& a, int* n, MogeraChild v) {
    *n += 1;
    MogeraChild* arr = new MogeraChild[*n];
    for (int i = 0; i < *n; i++) {
        if (i < *n - 1)
            arr[i] = a[i];
        else
            arr[i] = v;
    }
    delete[] a;
    a = nullptr;
    a = arr;
}

//------------------------------------MOGERA BOSS-------------------------

class Mogera {
protected:
    Sprite* bossChest;
    Sprite* bossLeg;
    Texture bodyText[3];
    Texture legText[3];
    int bodyFrames = 0;
    int legFrames = 0;
    float frametime = 0.15f;
    Clock clock;

   
    MogeraChild* spawns;
    int spawnCount;
    Clock spawnClock;
    float spawnInterval = 3.0f;  
    bool isDead = false;

public:
static int bossHp;
    Mogera() {
        bodyText[0].loadFromFile("mogera/mogera_01.png");
        bodyText[1].loadFromFile("mogera/mogera_02.png");
        bodyText[2].loadFromFile("mogera/mogera_03.png");
        legText[0].loadFromFile("mogera/mogera_leg_01.png");
        legText[1].loadFromFile("mogera/mogera_leg_02.png");
        legText[2].loadFromFile("mogera/mogera_leg_03.png");
        bossChest = new Sprite(bodyText[0]);
        bossLeg = new Sprite(legText[0]);

        // Initialize spawn array
        spawns = nullptr;
        spawnCount = 0;
    }

    void movement(Tiles* tiles, int tileCount) {
        int x = rand() % 10;
        if (x == 1) {
            bossChest->setScale({ 572.f / 1050.f, 460.f / 990.f });
            bossLeg->setScale({ 504.f / 1000.f, 118.f / 254.f });
            bossChest->move({ 0.0f, 0.0f });
            bossLeg->move({ 0.0f, 0.0f });

            if (clock.getElapsedTime().asSeconds() > frametime) {
                bodyFrames++;
                if (bodyFrames >= 3)
                    bodyFrames = 0;
                bossChest->setTexture(bodyText[bodyFrames]);
                bossLeg->setTexture(legText[bodyFrames]);
                clock.restart();
            }
        }

        if (bossLeg->getPosition().y >= 550.f) {
            bossChest->setScale({ 572.f / 1050.f, 460.f / 990.f });
            bossLeg->setScale({ 504.f / 1000.f, 118.f / 254.f });
            bossChest->move({ 0.0f, 0.0f });
            bossLeg->move({ 0.0f, 0.0f });

            if (clock.getElapsedTime().asSeconds() > frametime) {
                bodyFrames++;
                if (bodyFrames >= 3)
                    bodyFrames = 0;
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
        }

       
        cleanupInactiveSpawns();
    }

    void spawnEnemy() {
        MogeraChild newSpawn;
        float y = 0.0f;
        int r = rand()%3;
        if(r == 0)
        y += 0.0f;
        if(r == 1)
        y += 42.85f*2;
        if(r == 2)
        y += 42.85f*4;
        newSpawn.setPos(bossChest->getPosition().x, bossChest->getPosition().y+y);
        vectora(spawns, &spawnCount, newSpawn);
    }

    void cleanupInactiveSpawns() {
        // Count active spawns
        int activeCount = 0;
        for (int i = 0; i < spawnCount; i++) {
            if (spawns[i].isActive()) {
                activeCount++;
            }
        }

        // If all are active, no cleanup needed
        if (activeCount == spawnCount) return;

        // Create new array with only active spawns
        MogeraChild* newSpawns = new MogeraChild[activeCount];
        int index = 0;
        for (int i = 0; i < spawnCount; i++) {
            if (spawns[i].isActive()) {
                newSpawns[index] = spawns[i];
                index++;
            }
        }

        delete[] spawns;
        spawns = newSpawns;
        spawnCount = activeCount;
    }

    void setPos(float x, float y, float xx, float yy) {
        bossChest->setOrigin({ xx, yy });
        bossLeg->setOrigin({ xx, yy });
        bossChest->setPosition({ x, y });
        bossLeg->setPosition({ x, y + 205.7f });
    }

    void Draw(RenderWindow& window, Tiles* tiles, int tileCount) {
        window.draw(*bossChest);
        window.draw(*bossLeg);

        // Draw all spawns
        for (int i = 0; i < spawnCount; i++) {
            spawns[i].Draw(window);
        }
    }

    FloatRect boun() {
        return bossChest->getGlobalBounds();
    }

    void setCol() {
        bossChest->setColor(Color::Red);
    }

    void setNor() {
        bossChest->setColor(Color::White);
    }

    void checkdead() {
        if(bossHp <= 0)
        isDead = true;
    }

    bool getDeath() {
        return isDead;
    }

    ~Mogera() {
        delete bossChest;
        delete bossLeg;
        bossChest = nullptr;
        bossLeg = nullptr;
        delete[] spawns;
        spawns = nullptr;
    }
};

int Mogera::bossHp = 900;

bool snowballHitsMogera(Snowball& snowball, Mogera& boss) {
    if (!snowball.active) return false;
    // No isFrozen() guard — additional hits stack more ice layers

    if (snowball.getBounds().findIntersection(boss.boun())) {
        boss.setCol();
        Mogera::bossHp--;
        return true;
    }

    return false;
}
//-----------------------------------MOGERA BOSS END--------------------------

struct EnemySpawnPoint { int row; int col; };

EnemySpawnPoint levelSpawns[10][6] = {
    // Level 1 — platforms at rows 4,5,7,9,11,13
    { {3,4}, {3,11}, {6,2}, {6,13}, {8,3}, {10,7}, },
    // Level 2 — platforms at rows 3,5,7,9,11,13
    { {2,5}, {2,11}, {4,7}, {6,5}, {8,7}, {10,5} },
    // Level 3 — platforms at rows 0,3,5,7,9,11,13
    { {2,6}, {2,10}, {4,5}, {4,11}, {6,5}, {10,4} },
    // Level 4 — platforms at rows 0,3,5,7,9,11,13
    { {2,7}, {4,7}, {6,4}, {6,10}, {8,4}, {10,5} },
    // Level 5 — platforms at rows 0,3,5,7,9,11,13
    { {2,5}, {2,11}, {4,8}, {6,7}, {8,7}, {10,7} },
    // Level 6 — platforms at rows 0,3,4,7,9,11,13
    { {2,5}, {2,10}, {3,7}, {6,6}, {10,5}, {10,10} },
    // Level 7 — platforms at rows 0,3,4,5,7,8,9,10,11,13
    { {2,3}, {2,12}, {4,8}, {6,5}, {8,4}, {10,2} },
    // Level 8 — platforms at rows 0,4,5,7,9,11,13
    { {3,7}, {4,8}, {6,4}, {6,11}, {8,4}, {10,6} },
    // Level 9 — platforms at rows 0,3,5,7,9,11,13
    { {2,6}, {2,10}, {4,5}, {6,4}, {8,5}, {10,4} },
    // Level 10 — platforms at rows 0,5,7,8,9,11,13
    { {4,3}, {4,10}, {6,4}, {8,4}, {10,4}, {10,10} }
};

void spawnFoogas(Fooga* foogas) {
    float fx = 177.f / 2.f;
    float fy = 180.f / 2.f;
    for (int i = 0; i < 4; i++) {
        foogas[i].reset();
    }
    foogas[0].setPos(155.f, 125.f, fx, fy);
    foogas[1].setPos(355.f, 125.f, fx, fy);
    foogas[2].setPos(155.f, 275.f, fx, fy);
    foogas[3].setPos(555.f, 275.f, fx, fy);
}

void spawnEnemies(Botom* enemies, int levelNo) {
    EnemySpawnPoint* spawns = levelSpawns[levelNo - 1];
    for (int i = 0; i < 6; i++) {
        enemies[i].reset();
        enemies[i].setPos(spawns[i].row, spawns[i].col);
        enemies[i].setmv1(rand() % 2 == 0);
    }
}

GameState currentState = GameState::FlashScreen;

int main()
{
string username, password;

cout<<"Do you want to login or register? (1 for login, 2 for register)" <<endl;
int choice;
cin>>choice;

cout<<"Enter your username: " <<endl;
cin>>username;
cout<<"Enter your password: " <<endl;
cin>>password;

    DatabaseManager db;
    ShopManager shop; // Our new shop manager!

    int lives = 2, gems = 100, score = 0, count = 0; // Starting with 100 gems for testing
    int levelNo = 1;

    // Timers for shop items
    Clock speedBoostTimer;
    Clock balloonTimer;
switch(choice){
    case 1:
        if(db.verifyLogin(username, password)){
            db.LoadUser(username, levelNo, lives, gems, score);
        }
        else {
            cout<<"Your username or password is incorrect or you are not registered.\n";
            //return 0;
        }
    break;
    
    case 2:
        db.registerUser(username, password);
    break;
    
    default:
        cout<<"Invalid choice. Please try again." <<endl;
    break;
}
    
    
    RenderWindow window(VideoMode({ 800u, 600u }), "HOPE");

    GameState previousState = GameState::MainMenu;
    Level currentLevel;
    Texture bgTex;
    Sprite background(bgTex);
    Tiles* tilt = nullptr;
    

    LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);

    Player play;
    const int MAX_ENEMIES = 6;
    Botom enemies[MAX_ENEMIES];

    spawnEnemies(enemies, levelNo);
    const int MAX_SNOWBALLS = 24;
    Snowball snowballs[MAX_SNOWBALLS];
    play.setPos(12, 5);


    //------------- Foogas ---------------
    int num = 4;
    Fooga fooga[4];
    spawnFoogas(fooga);
    window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("Fonts/snow-bros.ttf")) {
        // Fallback or error handling could go here
    }

    Mogera mogera;
    float vx = 572.f/2.f;
    float vy = 460.f/2.f;
    mogera.setPos(650.f, 300.f, vx, vy);

    Text title(font);  // Pass font here
Text prompt(font); // Pass font here


    title.setString("SNOW BROS");
    title.setCharacterSize(100);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    // Center title (approximate for 800x600)
    title.setPosition({ 150.f, 150.f });



    prompt.setString("Press ENTER to Start");
    prompt.setCharacterSize(30);
    prompt.setFillColor(Color::Yellow);
    prompt.setPosition({ 250.f, 400.f });

    //flash screen  
    Texture flashTexture;
    if (!flashTexture.loadFromFile("Images/Flash_Screen.png")) {
        cout<<" FLASH SCREEN WALI IMAGE NHI HAI NOOBn\n";
        return -1;
    }
    Sprite flashSprite(flashTexture);
    float scaleX = 800.f / flashTexture.getSize().x;
    float scaleY = 600.f / flashTexture.getSize().y;
    flashSprite.setScale({scaleX, scaleY});

    //main menu 
    Texture mainMenuTexture;
    if (!mainMenuTexture.loadFromFile("Images/main_menu.png")) {
        // This handles if the image is missing
        cout<<" MAIN MENU WALI IMAGE NHI HAI NOOB\n";
        return -1;
    }
    Sprite mainMenuSprite(mainMenuTexture);
    float mainMenuScaleX = 800.f / mainMenuTexture.getSize().x;
    float mainMenuScaleY = 600.f / mainMenuTexture.getSize().y;
    mainMenuSprite.setScale({mainMenuScaleX, mainMenuScaleY});

    /// SHOP Screen
    Texture shopTexture;
    if (!shopTexture.loadFromFile("Images/shop.png")) {

        cout<<" SHOP SCREEN WALI IMAGE NHI HAI NOOB\n";
        return -1;
    }
    Sprite shopSprite(shopTexture);
    float shopScaleX = 800.f / shopTexture.getSize().x;
    float shopScaleY = 600.f / shopTexture.getSize().y;
    shopSprite.setScale({shopScaleX, shopScaleY});
    while (window.isOpen())
    {
        switch (currentState) {
            case GameState::FlashScreen:
            {
                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value())
                        break;

                    if (event->is<Event::Closed>())
                        window.close();

                    // Transition on key press event (happens only once)
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Enter || keyPressed->code == Keyboard::Key::Space) {
                            currentState = GameState::MainMenu;
                        }
                    }
                }

                window.clear(Color::Black);
                window.draw(flashSprite);

                Text pressEnter(font);
                pressEnter.setString("PRESS ENTER TO START");
                pressEnter.setCharacterSize(20);
                pressEnter.setFillColor(Color::White);
                pressEnter.setPosition({ 280.f, 500.f });
                
                // Simple blink effect
                if ((int)(flashSprite.getColor().a) % 2 == 0) { // Just a placeholder for time-based logic
                     window.draw(pressEnter);
                }

                window.display();
            }
            break;
            case GameState::MainMenu:
            {
                // Corrected Mapping: 1 Play, 2 Multi, 3 Shop, 4 Lead, 5 Exit
                FloatRect btnPlay({ 336.f, 242.f }, { 129.f, 31.f });       
                FloatRect btnContinue({ 500.f, 242.f }, { 120.f, 31.f });
                FloatRect btnMulti({ 333.f, 279.f }, { 135.f, 29.f });     
                FloatRect btnShop({ 370.f, 316.f }, { 70.f, 29.f });       
                FloatRect btnLeader({ 333.f, 353.f }, { 135.f, 29.f });    
                FloatRect btnExit({ 328.f, 390.f }, { 144.f, 25.f });      

                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                int hoveredOption = 0; 

                if (btnPlay.contains(mousePos)) hoveredOption = 1;
                else if (btnMulti.contains(mousePos)) hoveredOption = 2;
                else if (btnShop.contains(mousePos)) hoveredOption = 3;
                else if (btnLeader.contains(mousePos)) hoveredOption = 4;
                else if (btnExit.contains(mousePos)) hoveredOption = 5;
                else if (levelNo > 1 && btnContinue.contains(mousePos)) hoveredOption = 6;

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();

                    if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                        if (mousePressed->button == Mouse::Button::Left) {
                            if (hoveredOption == 1) currentState = GameState::CharacterSelect;
                            if (hoveredOption == 2) { /* Multiplayer */ }
                            if (hoveredOption == 3) currentState = GameState::Shop;
                            if (hoveredOption == 4) currentState = GameState::Ranking;
                            if (hoveredOption == 5) window.close(); 
                            if (hoveredOption == 6) {
                                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                                spawnEnemies(enemies, levelNo);
                                spawnFoogas(fooga);
                                play.setPos(12, 5);
                                currentState = GameState::Playing;
                            }
                        }
                    }
                }

                window.clear(Color::Black);
                window.draw(mainMenuSprite);
                if (levelNo > 1) {
                    Text continueText(font);
                    continueText.setString("CONTINUE");
                    continueText.setCharacterSize(20);
                    continueText.setFillColor(hoveredOption == 6 ? Color::Cyan : Color::White);
                    continueText.setPosition({ 505.f, 245.f });
                    window.draw(continueText);
                }

                // Restore On-Hover Selectors (Little Snowballs)
                if (hoveredOption > 0) {
                    CircleShape selector(8.f);
                    selector.setFillColor(Color::White);
                    // Match the Y positions of our buttons
                    if (hoveredOption == 1) selector.setPosition({ 310.f, 250.f }); // Play
                    if (hoveredOption == 2) selector.setPosition({ 310.f, 287.f }); // Multi
                    if (hoveredOption == 3) selector.setPosition({ 345.f, 324.f }); // Shop
                    if (hoveredOption == 4) selector.setPosition({ 310.f, 361.f }); // Lead
                    if (hoveredOption == 5) selector.setPosition({ 305.f, 398.f }); // Exit
                    if (hoveredOption == 6) selector.setPosition({ 480.f, 250.f }); // Continue
                    window.draw(selector);
                }

                window.display();
            }
            break;

            case GameState::Ranking:
            {
                struct RankingEntry { string name; int level; int score; };
                RankingEntry topPlayers[100];
                int playerCount = 0;

                ifstream file("progress.txt");
                if (file.is_open()) {
                    string line;
                    while (getline(file, line) && playerCount < 100) {
                        if (line.empty()) continue;
                        stringstream ss(line);
                        string name, temp;
                        getline(ss, name, ',');
                        
                        int lvl = 0;
                        if (getline(ss, temp, ',')) lvl = atoi(temp.c_str());
                        getline(ss, temp, ','); // lives
                        getline(ss, temp, ','); // gems
                        int sc = 0;
                        if (getline(ss, temp, ',')) sc = atoi(temp.c_str());
                        
                        topPlayers[playerCount++] = { name, lvl, sc };
                    }
                    file.close();
                }

                // Bubble Sort (Descending)
                for (int i = 0; i < playerCount - 1; i++) {
                    for (int j = 0; j < playerCount - i - 1; j++) {
                        if (topPlayers[j].score < topPlayers[j + 1].score) {
                            RankingEntry t = topPlayers[j];
                            topPlayers[j] = topPlayers[j + 1];
                            topPlayers[j + 1] = t;
                        }
                    }
                }

                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Escape || keyPressed->code == Keyboard::Key::Enter) {
                            currentState = GameState::MainMenu;
                        }
                    }
                }

                window.clear(Color(10, 10, 30));

                // 1. Center Title
                Text leadTitle(font);
                leadTitle.setCharacterSize(50);
                leadTitle.setFillColor(Color::Yellow);
                leadTitle.setString("HALL OF FAME");
                leadTitle.setStyle(Text::Bold);
                FloatRect titleBounds = leadTitle.getGlobalBounds();
                leadTitle.setPosition({ (800.f - titleBounds.size.x) / 2.f, 30.f });
                window.draw(leadTitle);

                // 2. Table Header
                RectangleShape headerBar({ 640.f, 40.f });
                headerBar.setFillColor(Color(40, 40, 80));
                headerBar.setPosition({ 80.f, 110.f });
                window.draw(headerBar);

                Text headerText(font);
                headerText.setCharacterSize(22);
                headerText.setFillColor(Color::Cyan);
                headerText.setString("RANK   NAME   LEVEL   SCORE");
                headerText.setPosition({ 100.f, 115.f });
                window.draw(headerText);

                // 3. Draw Rows
                int show = (playerCount < 8) ? playerCount : 8; // Show top 8
                for (int i = 0; i < show; i++) {
                    // Row Background (alternating)
                    RectangleShape rowBg({ 640.f, 45.f });
                    rowBg.setFillColor(i % 2 == 0 ? Color(25, 25, 55) : Color(20, 20, 50));
                    rowBg.setPosition({ 80.f, 160.f + i * 48.f });
                    window.draw(rowBg);

                    // Rank
                    string rankStr = to_string(i + 1);
                    if (i == 0) rankStr = "1ST";
                    else if (i == 1) rankStr = "2ND";
                    else if (i == 2) rankStr = "3RD";
                    else rankStr += "TH";

                    Text rowText(font);
                    rowText.setCharacterSize(20);
                    rowText.setFillColor(i < 3 ? Color::Yellow : Color::White);
                    
                    // Column Alignment (Fixed X positions)
                    rowText.setString(rankStr);
                    rowText.setPosition({ 100.f, 170.f + i * 48.f });
                    window.draw(rowText);

                    string nameStr = topPlayers[i].name;
                    if (nameStr.length() > 12) nameStr = nameStr.substr(0, 12);
                    rowText.setString(nameStr);
                    rowText.setPosition({ 200.f, 170.f + i * 48.f });
                    window.draw(rowText);

                    rowText.setString(to_string(topPlayers[i].level));
                    rowText.setPosition({ 420.f, 170.f + i * 48.f });
                    window.draw(rowText);

                    rowText.setString(to_string(topPlayers[i].score));
                    rowText.setPosition({ 580.f, 170.f + i * 48.f });
                    window.draw(rowText);
                }

                // 4. Back Prompt
                Text backPrompt(font);
                backPrompt.setCharacterSize(18);
                backPrompt.setFillColor(Color(150, 150, 150));
                backPrompt.setString("PRESS ENTER TO RETURN TO MAIN MENU");
                FloatRect backBounds = backPrompt.getGlobalBounds();
                backPrompt.setPosition({ (800.f - backBounds.size.x) / 2.f, 540.f });
                window.draw(backPrompt);

                window.display();
            }
            break;

            case GameState::Shop:
            {
                // Define Hitboxes
                FloatRect item1({ 122.f, 378.f }, { 72.f, 25.f }); // width = 194-122, height = 403-378
                FloatRect item2({ 241.f, 378.f }, { 73.f, 25.f });
                FloatRect item3({ 365.f, 378.f }, { 70.f, 25.f });
                FloatRect item4({ 487.f, 378.f }, { 68.f, 25.f });
                FloatRect item5({ 607.f, 378.f }, { 71.f, 25.f });
                FloatRect shopExit({ 319.f, 536.f }, { 162.f, 46.f });

                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                int shopHover = 0;
                if (item1.contains(mousePos)) shopHover = 1;
                else if (item2.contains(mousePos)) shopHover = 2;
                else if (item3.contains(mousePos)) shopHover = 3;
                else if (item4.contains(mousePos)) shopHover = 4;
                else if (item5.contains(mousePos)) shopHover = 5;
                else if (shopExit.contains(mousePos)) shopHover = 6;

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value())
                        break;

                    if (event->is<Event::Closed>())
                        window.close();

                    if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                        if (mousePressed->button == Mouse::Button::Left) {
                            if (shopHover == 1) {
                                if (shop.buyExtraLife(gems, lives)) {
                                    cout << "Purchased Extra Life! Lives: " << lives << endl;
                                }
                            }
                            else if (shopHover == 2) {
                                if (shop.buySpeedBoost(gems, hasSpeedBoost)) {
                                    cout << "Speed Boost Activated!" << endl;
                                    play.setSpeed(3.5f); // Set high speed
                                    speedBoostTimer.restart(); // Start 30s timer
                                }
                            }
                            else if (shopHover == 3) {
                                if (shop.buySnowballPower(gems, hasSnowballPower)) {
                                    cout << "Snowball Power Activated!" << endl;
                                }
                            }
                            else if (shopHover == 4) {
                                if (shop.buyDistanceIncrease(gems, hasDistanceIncrease)) {
                                    cout << "Distance Increase Activated!" << endl;
                                }
                            }
                            else if (shopHover == 5) {
                                if (shop.buyBalloonMode(gems, hasBalloonMode)) {
                                    cout << "Balloon Mode Activated!" << endl;
                                    play.setGravity(0.25f); // Half gravity
                                    balloonTimer.restart();
                                }
                            }
                            if (shopHover == 6) currentState = previousState;
                        }
                    }

                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Escape) {
                            currentState = previousState;
                        }
                    }
                }

                // --- SHOP COORDINATE HELPER ---
                // cout << "SHOP Mouse X: " << mousePos.x << " | Mouse Y: " << mousePos.y << "\r" << flush;

                window.clear(Color::Black);
                window.draw(shopSprite);

                // Draw Gem Count
                Text gemText(font);
                gemText.setString("GEMS: " + to_string(gems));
                gemText.setCharacterSize(30);
                gemText.setFillColor(Color::Yellow);
                gemText.setPosition({ 581.f,388.f }); // Position based on your coords 581, 388
                window.draw(gemText);

                // Draw Selector snowball on hover
                if (shopHover > 0 && shopHover <= 5) {
                    CircleShape selector(5.f);
                    selector.setFillColor(Color::White);
                    // Just a simple visual for now
                    selector.setPosition({ mousePos.x - 15.f, mousePos.y });
                    window.draw(selector);
                }

                window.display();
            }
            break;

            case GameState::Playing:
            {
                // Check Power-up Timers
                if (hasSpeedBoost && speedBoostTimer.getElapsedTime().asSeconds() > 30.f) {
                    hasSpeedBoost = false;
                    play.setSpeed(2.25f); // Reset to normal speed
                    cout << "Speed Boost Expired!" << endl;
                }
                if (hasBalloonMode && balloonTimer.getElapsedTime().asSeconds() > 30.f) {
                    hasBalloonMode = false;
                    play.setGravity(0.5f); // Reset to normal gravity
                    cout << "Balloon Mode Expired!" << endl;
                }

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value())
                        break;

                    if (event->is<Event::Closed>())
                        window.close();
            
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                if (keyPressed->code == Keyboard::Key::P || keyPressed->code == Keyboard::Key::Escape) {
                    currentState = GameState::Paused;
                }
                if (keyPressed->code == Keyboard::Key::N) {
                    levelNo++;
                    if (levelNo > 10)
                        levelNo = 1;
                    LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                    play.setPos(12, 5);
                }
                if (keyPressed->code == Keyboard::Key::J) {
                    // Reuse first inactive snowball from fixed pool
                    for (int i = 0; i < MAX_SNOWBALLS; i++) {
                        if (!snowballs[i].active) {
                            snowballs[i].spawn(play.getPosition().x, play.getPosition().y, play.getDirectionX());
                            break;
                        }
                    }
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
            play.move(Keyboard::Key::A, tilt, count);
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
            play.move(Keyboard::Key::D, tilt, count);
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
            play.move(Keyboard::Key::W, tilt, count);
        }

        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            snowballs[i].update(tilt, count);
        }

        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            if (!snowballs[i].active){
            mogera.setNor();
                continue;
                }

            for (int j = 0; j < MAX_ENEMIES; j++) { // checking for collision between snowball and enemy
                if (!enemies[j].isAlive()) continue;
                if (snowballHitsEnemy(snowballs[i], enemies[j], hasSnowballPower)) {
                    snowballs[i].active = false; // consume the snowball on hit
                    break;
                }
            }
        }
        // foogas huh huh
        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            if (!snowballs[i].active)
                continue;
 
            for (int j = 0; j < 4; j++) { // checking for collision between snowball and enemy
                if (!fooga[j].isAlive()) continue;
                if (snowballHitsEnemy(snowballs[i], fooga[j], tilt, count, hasSnowballPower)) {
                    snowballs[i].active = false; // consume the snowball on hit
                    break;
                }
            }
        }

        //FOR MOGERA
        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            if(!snowballs[i].active)
            continue;
            snowballHitsMogera(snowballs[i], mogera);
        }
        // --- UPDATED LEVEL COMPLETION LOGIC IS BELOW IN THE DRAWING SECTION ---

        //        levelNo++;
        //        if (levelNo > 10)
        //           
        //            levelNo = 1;
        //        LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
        //        play.setPos(12, 5);
        //        spawnEnemies(fooga, levelNo);
        //    }
        //}
        //
        // --- KICK DETECTION ---
        // If player overlaps a fully-frozen enemy, kick it in the player's direction
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            if (!enemies[i].isFullyFrozen()) continue;
            if (enemies[i].isRolling()) continue;  // already rolling

            if (play.boun().findIntersection(enemies[i].boun())) {
                enemies[i].kick(play.getDirectionX());
                score += 100;
				gems += 10;
            }

        }
        //fooga idk
        for (int i = 0; i < 4; i++) {
            //for fooga gng
            if (!fooga[i].isAlive()) continue;
            if (!fooga[i].isFullyFrozen()) continue;
            if (play.boun().findIntersection(fooga[i].boun())) {
                fooga[i].kick(play.getDirectionX());
            }
        }
        // --- ROLLING KILLS ---
        // A rolling enemy kills any non-rolling enemy it overlaps
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            if (!enemies[i].isRolling()) continue;

            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (i == j) continue;  // don't collide with yourself
                if (!enemies[j].isAlive()) continue;
                if (enemies[j].isRolling()) continue;  // two rolling won't kill each other

                if (enemies[i].boun().findIntersection(enemies[j].boun())) {
                    enemies[j].kill();  // enemy in the path dies
                    score +=200;
                    gems+=10;
                    // The rolling snowball keeps going to kill others in its path
                }
            }
        }
        // FOOGAS!!!!!!!!!
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i == j) continue;
                if (!fooga[j].isAlive()) continue;
                if (fooga[j].isRolling()) continue;
                if (fooga[i].boun().findIntersection(fooga[j].boun())) {
                    fooga[j].kill();
                }
            }
        }

        // --- PLAYER DEATH DETECTION ---
        for (int i = 0; i < MAX_ENEMIES; i++) {
            // Only living, un-frozen enemies can kill the player
            if (!enemies[i].isAlive() || enemies[i].isFrozen() || enemies[i].isDying()) continue;

            if (play.boun().findIntersection(enemies[i].boun())) {
                db.saveProgress(username,levelNo,lives-1,gems,score);
                currentState = GameState::GameOver;
                break;
            }
        }

        // Move all enemies with gravity and tile collision
        mover(MAX_ENEMIES, enemies, tilt, count);
        mover(4, fooga, tilt, count);
        //mover(2, tornado, tilt, count);
        mogera.movement(tilt, count);
        //mover(opt,botom);
        Gravity(MAX_ENEMIES, enemies, tilt, count);
        Gravity(4, fooga, tilt, count);
        //Gravity(tornado, 2, tilt, count);
        //Gravity(&mogera, 1, tilt, count);
        // Update frozen state for all enemies (animate overlay, check unfreeze timer)
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            enemies[i].updateFrozen();
        }

        // Update dying animation (shrink effect) for all enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            enemies[i].updateDying();
        }

        //foogas
        for (int i = 0; i < 4; i++) {
            if (!fooga[i].isAlive()) continue;
            fooga[i].updateFrozen();
        }
        for (int i = 0; i < 4; i++) {
            fooga[i].updateDying();
        }
        play.applyGravity(tilt, count);

        window.clear(Color::Black);
        window.draw(background);
        for (int i = 0; i < count; i++) {
            tilt[i].Draw(window);
        }

        // Draw all enemies to screen.
        //Draw(MAX_ENEMIES, enemies, window);
        Draw(MAX_ENEMIES, enemies, window);
        Draw(4, fooga, window);
        //Draw(2, tornado, window);
        if(Mogera::bossHp > 0)
        mogera.Draw(window, tilt, count);
        //------------MOGERA DYING-------------------------------
        else {
           mogera.setPos(-999.f,-999.f,0.f,0.f);
        }
        // Draw frozen overlays on top of enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            if (enemies[i].isFrozen()) {
                window.draw(enemies[i].getFrozenOverlay());
            }
        }
        // foogas again ._.
        for (int i = 0; i < 4; i++) {
            if (!fooga[i].isAlive()) continue;
            if (fooga[i].isFrozen()) {
                window.draw(fooga[i].getFrozenOverlay());
            }
        }
        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            snowballs[i].draw(window);
        }
        window.draw(play.Draw());

        // --- HUD DRAWING ---
        // 1. Semi-transparent background bar
        RectangleShape hudBar({ 800.f, 40.f });
        hudBar.setFillColor(Color(0, 0, 0, 160));
        hudBar.setPosition({ 0.f, 0.f });
        window.draw(hudBar);

        Text hudText(font);
        hudText.setCharacterSize(22);
        hudText.setOutlineColor(Color::Black);
        hudText.setOutlineThickness(1.5f);

        // Score (Left)
        hudText.setFillColor(Color(255, 215, 0)); // Gold
        hudText.setString("SCORE: " + to_string(score));
        hudText.setPosition({ 20.f, 5.f });
        window.draw(hudText);

        // Level (Mid-Left)
        hudText.setFillColor(Color::White);
        hudText.setString("STAGE: " + to_string(levelNo));
        hudText.setPosition({ 220.f, 5.f });
        window.draw(hudText);

        // Lives (Center)
        hudText.setFillColor(Color(255, 60, 60)); // Arcade Red
        hudText.setString("LIVES: " + to_string(lives));
        hudText.setPosition({ 380.f, 5.f });
        window.draw(hudText);

        // Gems (Right)
        hudText.setFillColor(Color(0, 255, 255)); // Cyan
        hudText.setString("GEMS: " + to_string(gems));
        hudText.setPosition({ 620.f, 5.f });
        window.draw(hudText);

        // --- LEVEL COMPLETION CHECK ---
        bool allDead = true;
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].isAlive()) { allDead = false; break; }
        }
        if (allDead) {
            for (int i = 0; i < 4; i++) {
                if (fooga[i].isAlive()) { allDead = false; break; }
            }
        }

        if (allDead) {
            levelNo++;
            if (levelNo > 10) {
                // Game Won!
                currentState = GameState::Victory;
                levelNo = 1;
            } else {
                db.saveProgress(username, levelNo, lives, gems, score);
                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                spawnEnemies(enemies, levelNo);
                spawnFoogas(fooga);
                play.setPos(12, 5);
            }
        }

        window.display();
            }
            break;

            case GameState::CharacterSelect:
            {
                int selectedChar = 1; // 1 = Nick, 2 = Tom, 3 = SnowB
                if (Keyboard::isKeyPressed(Keyboard::Key::Left)) selectedChar = 1;
                if (Keyboard::isKeyPressed(Keyboard::Key::Down)) selectedChar = 2;
                if (Keyboard::isKeyPressed(Keyboard::Key::Right)) selectedChar = 3;

                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Enter) {
                            // Apply Stats based on character
                            levelNo = 1;
                            lives = 2;
                            score = 0;
                            gems = 100;
                            if (selectedChar == 1) { /* Nick - Default */ }
                            if (selectedChar == 2) { /* Tom - Faster */ play.setSpeed(3.0f); }
                            if (selectedChar == 3) { /* SnowB - Range */ }
                            
                            LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                            spawnEnemies(enemies, levelNo);
                            spawnFoogas(fooga);
                            play.setPos(12, 5);
                            currentState = GameState::Playing;
                        }
                        if (keyPressed->code == Keyboard::Key::Escape) currentState = GameState::MainMenu;
                    }
                }

                window.clear(Color(30, 30, 60));
                Text selectTitle(font);
                selectTitle.setCharacterSize(40);
                selectTitle.setFillColor(Color::Yellow);
                selectTitle.setString("SELECT YOUR SNOWMAN");
                selectTitle.setPosition({ 150.f, 50.f });
                window.draw(selectTitle);

                // Nick
                Text nick(font);
                nick.setString("NICK\nSPEED: 1.0\nRANGE: 1.0");
                nick.setPosition({ 100.f, 200.f });
                nick.setFillColor(selectedChar == 1 ? Color::Cyan : Color::White);
                window.draw(nick);

                // Tom
                Text tom(font);
                tom.setString("TOM\nSPEED: 1.5\nRANGE: 0.8");
                tom.setPosition({ 350.f, 200.f });
                tom.setFillColor(selectedChar == 2 ? Color::Cyan : Color::White);
                window.draw(tom);

                // SnowB
                Text snowb(font);
                snowb.setString("SNOWB\nSPEED: 0.8\nRANGE: 1.5");
                snowb.setPosition({ 600.f, 200.f });
                snowb.setFillColor(selectedChar == 3 ? Color::Cyan : Color::White);
                window.draw(snowb);

                Text prompt(font);
                prompt.setCharacterSize(18);
                prompt.setString("USE ARROWS TO NAVIGATE | ENTER TO START");
                prompt.setPosition({ 180.f, 500.f });
                window.draw(prompt);

                window.display();
            }
            break;

            case GameState::Paused:
            {
                static int hoveredOption = 1; // Persistent choice

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Up) hoveredOption = (hoveredOption == 1) ? 3 : hoveredOption - 1;
                        if (keyPressed->code == Keyboard::Key::Down) hoveredOption = (hoveredOption == 3) ? 1 : hoveredOption + 1;

                        if (keyPressed->code == Keyboard::Key::Enter) {
                            if (hoveredOption == 1) currentState = GameState::Playing;
                            else if (hoveredOption == 2) {
                                previousState = GameState::Paused; // Mark where we came from
                                currentState = GameState::Shop;
                            }
                            else currentState = GameState::MainMenu;
                        }
                        if (keyPressed->code == Keyboard::Key::P || keyPressed->code == Keyboard::Key::Escape) {
                            currentState = GameState::Playing;
                        }
                    }
                }

                // 1. Draw game state in background (frozen)
                window.clear(Color::Black);
                window.draw(background);
                for (int i = 0; i < count; i++) {
                    tilt[i].Draw(window);
                }
                Draw(MAX_ENEMIES, enemies, window);
                Draw(4, fooga, window);
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (enemies[i].isAlive() && enemies[i].isFrozen()) {
                        window.draw(enemies[i].getFrozenOverlay());
                    }
                }
                for (int i = 0; i < 4; i++) {
                    if (fooga[i].isAlive() && fooga[i].isFrozen()) {
                        window.draw(fooga[i].getFrozenOverlay());
                    }
                }
                for (int i = 0; i < MAX_SNOWBALLS; i++) {
                    snowballs[i].draw(window);
                }
                window.draw(play.Draw());

                // 2. Dim overlay (Full Screen)
                RectangleShape dim({ 800.f, 600.f });
                dim.setFillColor(Color(0, 0, 0, 180));
                window.draw(dim);

                // 3. Central Menu Box (Slightly taller for 3 options)
                RectangleShape menuBox({ 400.f, 350.f });
                menuBox.setFillColor(Color(20, 20, 50, 230));
                menuBox.setOutlineColor(Color::White);
                menuBox.setOutlineThickness(2.f);
                menuBox.setPosition({ 200.f, 125.f });
                window.draw(menuBox);

                Text pauseText(font);
                pauseText.setCharacterSize(50);
                pauseText.setFillColor(Color::Yellow);
                pauseText.setString("PAUSED");
                pauseText.setStyle(Text::Bold);
                FloatRect pBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - pBounds.size.x) / 2.f, 150.f });
                window.draw(pauseText);

                pauseText.setCharacterSize(25);
                pauseText.setStyle(Text::Regular);
                
                // Option 1: Resume
                pauseText.setString("RESUME");
                pauseText.setFillColor(hoveredOption == 1 ? Color::Cyan : Color::White);
                FloatRect rBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - rBounds.size.x) / 2.f, 240.f });
                window.draw(pauseText);

                // Option 2: Shop
                pauseText.setString("SHOP");
                pauseText.setFillColor(hoveredOption == 2 ? Color::Cyan : Color::White);
                FloatRect sBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - sBounds.size.x) / 2.f, 300.f });
                window.draw(pauseText);

                // Option 3: Main Menu
                pauseText.setString("MAIN MENU");
                pauseText.setFillColor(hoveredOption == 3 ? Color::Cyan : Color::White);
                FloatRect mBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - mBounds.size.x) / 2.f, 360.f });
                window.draw(pauseText);

                window.display();
            }
            break;

            case GameState::Victory:
            {
                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Enter || keyPressed->code == Keyboard::Key::Escape) {
                            currentState = GameState::MainMenu;
                        }
                    }
                }

                window.clear(Color(0, 60, 0)); // Dark Green for Victory
                Text winText(font);
                winText.setOutlineColor(Color::Black);
                winText.setOutlineThickness(2.f);

                winText.setCharacterSize(60);
                winText.setFillColor(Color::Yellow);
                winText.setString("CONGRATULATIONS!");
                FloatRect b1 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b1.size.x) / 2.f, 100.f });
                window.draw(winText);

                winText.setCharacterSize(30);
                winText.setFillColor(Color::White);
                winText.setString("YOU SAVED THE SNOW WORLD!");
                FloatRect b2 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b2.size.x) / 2.f, 220.f });
                window.draw(winText);

                winText.setString("FINAL SCORE: " + to_string(score));
                FloatRect b3 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b3.size.x) / 2.f, 320.f });
                window.draw(winText);

                winText.setCharacterSize(22);
                winText.setFillColor(Color(200, 200, 200));
                winText.setString("PRESS ENTER FOR MAIN MENU");
                FloatRect b4 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b4.size.x) / 2.f, 500.f });
                window.draw(winText);

                window.display();
            }
            break;

            case GameState::GameOver:
            {
                static int hoveredOption = 1; // Persistent selection

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Up) hoveredOption = 1;
                        if (keyPressed->code == Keyboard::Key::Down) hoveredOption = 2;

                        if (keyPressed->code == Keyboard::Key::Enter) {
                            if (hoveredOption == 1) {
                                // Reset to Level 1
                                levelNo = 1;
                                lives = 2;
                                score = 0;
                                gems = 100;
                                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                                spawnEnemies(enemies, levelNo);
                                spawnFoogas(fooga);
                                play.setPos(12, 5);
                                currentState = GameState::Playing;
                            } else {
                                currentState = GameState::MainMenu;
                            }
                        }
                    }
                }

                window.clear(Color(80, 0, 0)); // Dark Red
                Text loseText(font);
                loseText.setOutlineColor(Color::Black);
                loseText.setOutlineThickness(2.f);

                loseText.setCharacterSize(70);
                loseText.setFillColor(Color::White);
                loseText.setString("GAME OVER");
                FloatRect b1 = loseText.getGlobalBounds();
                loseText.setPosition({ (800.f - b1.size.x) / 2.f, 120.f });
                window.draw(loseText);

                loseText.setCharacterSize(35);
                loseText.setString("RETRY");
                loseText.setFillColor(hoveredOption == 1 ? Color::Yellow : Color(180, 180, 180));
                FloatRect b2 = loseText.getGlobalBounds();
                loseText.setPosition({ (800.f - b2.size.x) / 2.f, 320.f });
                window.draw(loseText);

                loseText.setString("MAIN MENU");
                loseText.setFillColor(hoveredOption == 2 ? Color::Yellow : Color(180, 180, 180));
                FloatRect b3 = loseText.getGlobalBounds();
                loseText.setPosition({ (800.f - b3.size.x) / 2.f, 390.f });
                window.draw(loseText);

                window.display();
            }
            break;
        }
    }

    delete[] tilt;
    tilt = nullptr;
    return 0;
}

Texture Snowball::sharedTexture;
Texture Botom::frozenSharedTexture;