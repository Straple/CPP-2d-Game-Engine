#pragma once

#include "utils.h"
#include "Objects/dot.h"


void simulate_move(point_t& p, point_t& dp, point_t ddp, point_t dt) {
	ddp -= dp * 15;
	p = p + dp * dt + ddp * dt * dt * 0.5;
	dp = dp + ddp * dt;
}

void simulate_move2d(dot& p, dot& dp, dot ddp, point_t dt) {
	simulate_move(p.x, dp.x, ddp.x, dt);
	simulate_move(p.y, dp.y, ddp.y, dt);
}
