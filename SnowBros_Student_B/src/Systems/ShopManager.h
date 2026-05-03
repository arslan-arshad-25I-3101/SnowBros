#pragma once
#include <iostream>
#include <string>

class ShopManager {
public:
    ShopManager();

    // Returns true if purchase was successful
    bool buyExtraLife(int& gems, int& lives);
    bool buySpeedBoost(int& gems, bool& hasSpeedBoost);
    bool buySnowballPower(int& gems, bool& hasSnowballPower);
    bool buyDistanceIncrease(int& gems, bool& hasDistanceIncrease);
    bool buyBalloonMode(int& gems, bool& hasBalloonMode);

private:
    const int LIFE_COST = 50;
    const int SPEED_COST = 20;
    const int SNOWBALL_POWER_COST = 30;
    const int DISTANCE_COST = 25;
    const int BALLOON_COST = 35;
};
