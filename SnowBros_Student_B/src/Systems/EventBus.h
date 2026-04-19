/**
 * @file EventBus.h
 * @brief Observer pattern implementation for decoupled event communication
 * 
 * The EventBus allows different game systems to communicate without tight coupling.
 * Example: Enemy death event fires, UI and scoring systems listen independently.
 */

#pragma once
#include <functional>
#include <vector>
#include <memory>

/// Event types fired by game systems
enum class GameEvent
{
    EnemyKilled,            ///< Enemy defeated - data: (score, gems)
    LevelComplete,          ///< All enemies cleared - data: (levelScore, gemsCollected)
    PowerUpCollected,       ///< Player collected power-up - data: (powerUpType, duration)
    PlayerDeath,            ///< Player touched enemy and lost life - data: (livesRemaining)
    BossDefeated,           ///< Boss defeated - data: (gemReward)
    GameOver,               ///< Game ended - data: (finalScore, levelReached)
    GamePaused,             ///< Game paused
    GameResumed             ///< Game resumed
};

/// Callback function type for events
using EventCallback = std::function<void(int, int)>;  // (param1, param2)

class EventBus
{
public:
    /**
     * @brief Get singleton instance
     * @return Reference to EventBus instance
     */
    static EventBus& GetInstance();

    /**
     * @brief Subscribe to an event
     * @param event Event type to listen for
     * @param callback Function to call when event fires
     * @return Subscription ID (for unsubscribing)
     */
    int Subscribe(GameEvent event, EventCallback callback);

    /**
     * @brief Unsubscribe from an event
     * @param event Event type
     * @param subscriptionId ID returned from Subscribe()
     */
    void Unsubscribe(GameEvent event, int subscriptionId);

    /**
     * @brief Fire an event
     * @param event Event type
     * @param param1 First parameter (usage depends on event)
     * @param param2 Second parameter (usage depends on event)
     */
    void Fire(GameEvent event, int param1 = 0, int param2 = 0);

    /**
     * @brief Clear all subscriptions (useful for scene changes)
     */
    void Clear();

private:
    EventBus() = default;
    
    struct Subscription
    {
        int id;
        EventCallback callback;
    };

    std::vector<Subscription> subscriptions[8];  // One vector per event type
    int nextSubscriptionId = 0;
};
