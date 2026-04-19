/**
 * @file EventBus.cpp
 * @brief Implementation of EventBus
 */

#include "EventBus.h"

EventBus& EventBus::GetInstance()
{
    static EventBus instance;
    return instance;
}

int EventBus::Subscribe(GameEvent event, EventCallback callback)
{
    int subscriptionId = nextSubscriptionId++;
    subscriptions[static_cast<int>(event)].push_back({ subscriptionId, callback });
    return subscriptionId;
}

void EventBus::Unsubscribe(GameEvent event, int subscriptionId)
{
    auto& subs = subscriptions[static_cast<int>(event)];
    subs.erase(
        std::remove_if(subs.begin(), subs.end(),
            [subscriptionId](const Subscription& s) { return s.id == subscriptionId; }),
        subs.end()
    );
}

void EventBus::Fire(GameEvent event, int param1, int param2)
{
    auto& subs = subscriptions[static_cast<int>(event)];
    for (const auto& sub : subs)
    {
        sub.callback(param1, param2);
    }
}

void EventBus::Clear()
{
    for (int i = 0; i < 8; ++i)
    {
        subscriptions[i].clear();
    }
}
