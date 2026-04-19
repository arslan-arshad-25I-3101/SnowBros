/**
 * @file EntityTemplate.h
 * @brief TEMPLATE - Use this as a guide for creating new entity classes
 * 
 * This is a template showing how to properly extend Entity or Enemy.
 * Copy and modify this for new classes.
 */

#pragma once
#include "Entity.h"  // or Entity.h if not an enemy

/**
 * @brief Template Enemy Class
 * 
 * Replace "TemplateEnemy" with your actual class name
 * Replace this description with your enemy's behavior
 */
class TemplateEnemy : public Enemy  // Change to inherit from Enemy or Entity
{
public:
    /**
     * @brief Constructor
     * @param position Starting position in the world
     */
    explicit TemplateEnemy(const sf::Vector2f& position);

    /**
     * @brief Destructor
     */
    virtual ~TemplateEnemy() = default;

    /**
     * @brief Update entity logic each frame
     * @param deltaTime Time elapsed since last frame (seconds)
     */
    void Update(float deltaTime) override;

    /**
     * @brief Draw entity to the window
     * @param window Target render window
     */
    void Draw(sf::RenderWindow& window) override;

    /**
     * @brief Apply damage from snowball hit
     * @param damage Amount of damage
     * @return true if fully encased, false if only partially damaged
     */
    bool TakeDamage(int damage) override;

private:
    // Private member variables
    sf::CircleShape sprite;         // Visual representation (placeholder)
    int direction;                  // -1 for left, 1 for right
    float gravity;                  // Physics constant
    bool onPlatform;                // Whether touching a platform
    
    // Add more members specific to your enemy:
    // float flightTimer;           // For flying enemies
    // bool attackCooldown;         // For attacking enemies
    // etc.
};

/*
============================================================================
IMPLEMENTATION NOTES:

1. CONSTRUCTOR:
   - Call parent constructor with stats
   - Example: Enemy(position, EnemyStats{.health = 2, .speed = 50, ...})
   - Initialize your sprite shape/texture
   - Set hitbox dimensions

2. UPDATE:
   - Check if active before updating
   - Check if encased (if so, apply minimal physics)
   - Update position based on velocity
   - Handle collisions with platforms/walls
   - Apply gravity if needed

3. DRAW:
   - Only draw if active
   - Update sprite position to match entity position
   - window.draw(sprite) or window.draw(texture)

4. TAKEDAMAGE:
   - Call parent TakeDamage first
   - Return whether fully encased
   - Can add special behavior (e.g., knock back, temporary invulnerability)

5. STATS:
   - Adjust EnemyStats in constructor for difficulty
   - health: hits needed to fully encase (1-3 for color variants)
   - speed: pixels per second
   - scoreMin/scoreMax: random score range
   - gemDropChance: 0-100 percentage

6. ENCASED STATE:
   - When fully encased, don't update normally
   - Check IsEncased() to know state
   - Can still apply gravity while encased
   - Remove visual details (show snow ball instead)

============================================================================
EXAMPLE: FlyingEnemy (extends Botom or Enemy)

class FlyingEnemy : public Enemy
{
public:
    explicit FlyingEnemy(const sf::Vector2f& position);
    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow& window) override;
    bool TakeDamage(int damage) override;

private:
    float flightTimer;
    float flightDuration;
    bool inFlight;
    
    // Inherit position, velocity, sprite from Entity/Enemy
    // Just add flight-specific logic
};

In FlyingEnemy::Update():
- Start with inherited movement
- If flightTimer exceeds flightDuration:
  - Enter flight mode
  - Move freely without gravity
  - flightTimer = 0
- If in flight, move in random directions
- If flightTimer expires, land and resume normal movement

============================================================================
HIERARCHY DIAGRAM:

Entity (abstract)
  └─ Enemy (abstract)
      ├─ Botom
      │  └─ FlyingEnemy
      │     ├─ FlyingFooga
      │     └─ Tornado
      ├─ Boss (abstract)
      │  ├─ Mogera
      │  └─ Gamakichi
      └─ ColorVariant (decorator pattern)

============================================================================
TIPS:

1. Test each enemy thoroughly in isolation before adding to levels
2. Use hitbox debug toggle (F1) to verify collision areas
3. Keep movement smooth - use deltaTime, not fixed steps
4. Fire events via EventBus when important things happen
5. Comment your non-obvious logic
6. Reuse code - don't duplicate update/draw patterns
7. Keep enemy-specific behavior in overrides, common in base class

============================================================================
*/
