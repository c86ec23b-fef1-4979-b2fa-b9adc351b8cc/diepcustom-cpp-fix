#pragma once

#include "Entity/Tank/TankBody.h"
#include "Entity/AI.h"

class ArenaCloser : public TankBody {
public:
    static constexpr double BASE_SIZE = 175;

    std::unique_ptr<AI> ai;

    explicit ArenaCloser(GameServer* game);

    void tick(int tick) override;
};
