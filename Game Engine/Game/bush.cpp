// visibility
#define BUSH_SIZE 0.5

// fixeds
#define BUSH_DELTA_DRAW_POS dot(-15, 20) * BUSH_SIZE

// physics
#define BUSH_COLLISION_RADIUS 8

struct Bush {
	dot pos;

	Bush(dot pos) {
		this->pos = pos - BUSH_DELTA_DRAW_POS;
	}

	collision_circle get_collision() const {
		return Circle(pos, BUSH_COLLISION_RADIUS);
	}

	void draw() {
		draw_sprite(pos + BUSH_DELTA_DRAW_POS, BUSH_SIZE, SP_large_shadow, 128);
		draw_sprite(pos + BUSH_DELTA_DRAW_POS, BUSH_SIZE, SP_bush);

		if (debug_mode) {
			Circle circle = get_collision().circle;

			static_pos_update(circle.pos, !camera_mod);

			draw_circle(circle, Color(0xffffff, 50));
		}
	}
};
