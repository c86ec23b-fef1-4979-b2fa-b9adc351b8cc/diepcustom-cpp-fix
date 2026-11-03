#pragma once

#include "Entity/Shape/AbstractShape.h"

class Crasher : public AbstractShape {
public:
    std::unique_ptr<AI> ai;
    bool isLarge;

    explicit Crasher(GameServer* game, bool large = false);

    void tick(int tick) override;

private:
    double targettingSpeed;
};
