#include "ShopManager.h"

ShopManager::ShopManager() {}

bool ShopManager::buyExtraLife(int& gems, int& lives) {
    if (gems >= LIFE_COST) {
        gems -= LIFE_COST;
        lives++;
        return true;
    }
    std::cout << "Not enough gems for Extra Life!" << std::endl;
    return false;
}

bool ShopManager::buySpeedBoost(int& gems, bool& hasSpeedBoost) {
    if (gems >= SPEED_COST && !hasSpeedBoost) {
        gems -= SPEED_COST;
        hasSpeedBoost = true;
        return true;
    }
    return false;
}

bool ShopManager::buySnowballPower(int& gems, bool& hasSnowballPower) {
    if (gems >= SNOWBALL_POWER_COST && !hasSnowballPower) {
        gems -= SNOWBALL_POWER_COST;
        hasSnowballPower = true;
        return true;
    }
    return false;
}

bool ShopManager::buyDistanceIncrease(int& gems, bool& hasDistanceIncrease) {
    if (gems >= DISTANCE_COST && !hasDistanceIncrease) {
        gems -= DISTANCE_COST;
        hasDistanceIncrease = true;
        return true;
    }
    return false;
}

bool ShopManager::buyBalloonMode(int& gems, bool& hasBalloonMode) {
    if (gems >= BALLOON_COST && !hasBalloonMode) {
        gems -= BALLOON_COST;
        hasBalloonMode = true;
        return true;
    }
    return false;
}
