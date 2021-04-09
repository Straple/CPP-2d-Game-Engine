#pragma once

#include "Geometry/geometry.h"


/*  p0---+
*	|	 |
*	|    |
*	|    |
*	+----p1
*/
struct collision_box {
	dot p0, p1;

	collision_box(){}

	collision_box(const dot& p0, const dot& p1) {
		ASSERT(p0.x <= p1.x && p1.y <= p0.y, "bad points");

		this->p0 = p0;
		this->p1 = p1;
	}

	bool trigger(const dot& pos) const {
		return is_between(p0.x, pos.x, p1.x) && is_between(p1.y, pos.y, p0.y);
	}

	void bubble(dot* pos) const {
		if (trigger(*pos)) {

			point_t best_dist = std::min({
				abs(p0.x - pos->x),
				abs(p1.x - pos->x),

				abs(p0.y - pos->y),
				abs(p1.y - pos->y),
			});
			
			if (best_dist == abs(p0.x - pos->x)) {
				pos->x = p0.x;
			}
			else if (best_dist == abs(p1.x - pos->x)) {
				pos->x = p1.x;
			}
			else if (best_dist == abs(p0.y - pos->y)) {
				pos->y = p0.y;
			}
			else {
				pos->y = p1.y;
			}

		}
	}
};

struct collision_circle {
	Circle circle;

	collision_circle(){}

	collision_circle(const Circle& circle) {
		this->circle = circle;
	}

	// выталкивает pos за пределы круга
	void bubble(dot* pos) const {
		if (circle.isin(*pos)) {
			dot vector = (*pos - circle.pos).normalize() * circle.radius;
			*pos = circle.pos + vector;
		}
	}
};

