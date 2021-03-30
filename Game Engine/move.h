#pragma once

#include "utils.h"
#include "Objects/dot.h"


void simulate_move(point_t& p, point_t& dp, point_t ddp, point_t dt) {
	ddp -= dp * 15;
	p = p + dp * dt + ddp * quare(dt) * 0.5;
	dp = dp + ddp * dt;
}

void simulate_move2d(dot& p, dot& dp, dot ddp, point_t dt) {
	simulate_move(p.x, dp.x, ddp.x, dt);
	simulate_move(p.y, dp.y, ddp.y, dt);
}

void move_to(point_t& p, point_t& p_to, point_t& dp, point_t ddp, point_t dt) {

    point_t to = p;

    simulate_move(to, dp, ddp, dt);

    auto is_spoiled = [](point_t num) -> bool {
        return isnan(num) || isinf(num);
    };

    if (is_spoiled(to) || is_spoiled(dp)) {
        to = p_to;
        dp = 0;
    }
    // p----p_to---to
    // or
    // to---p_to---p
    else if((p <= p_to && p_to <= to) || (to <= p_to && p_to <= p)) {
        to = p_to;
        dp = 0;
    }

    p = to;
}

void move_to2d(dot& p, dot p_to, dot& dp, dot ddp, point_t dt) {
    move_to(p.x, p_to.x, dp.x, ddp.x, dt);
    move_to(p.y, p_to.y, dp.y, ddp.y, dt);
}
