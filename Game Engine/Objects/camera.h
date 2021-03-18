#pragma once

#include "../utils.h"
#include "../move.h"

struct Camera {
    dot pos; // позиция
    dot dp;
    const point_t speed = 20; // скорость камеры

    // симуляция (позиция игрока, время между кадрами)
    void simulate(dot player_pos, point_t delta_time) {
        simulate_move2d(pos, dp, (player_pos - pos) * speed, delta_time);
    }
};