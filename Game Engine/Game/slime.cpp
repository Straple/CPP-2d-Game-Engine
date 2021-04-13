// visibility
#define SLIME_ALPHA 210
#define SLIME_SIZE 0.4
#define SLIME_FRAME_DURATION 1.0 / 7

// fixeds
#define SLIME_DELTA_DRAW_POS dot(-30, 38) * SLIME_SIZE

// physics
#define SLIME_DDP_SPEED 300
#define SLIME_COLLISION_RADIUS 3

// cooldown
#define SLIME_ATTACK_COOLDOWN 3


struct Slime {

	dot pos;
	dot dp;

	point_t attack_cooldown_accum = 3;

	animation idle = animation(SP_slime, 0, 24, SLIME_FRAME_DURATION, 64);
	animation attack = animation(SP_slime, 25, 30, SLIME_FRAME_DURATION, 64);

	bool is_attack = false;

	Slime(){}

	Slime(const dot& p) {
		pos = p;
	}

	collision_circle get_collision() const {
		return Circle(pos, SLIME_COLLISION_RADIUS);
	}

	void simulate(const dot& player_pos, point_t delta_time) {
		if (is_attack) {

			u32 prev_frame_count = attack.frame_count;

			attack.frame_update(delta_time);

			// анимация атаки закончилась
			if (prev_frame_count > attack.frame_count) {
				// idle animation beginner

				player_pos_is_static = is_attack = false; // позиция игрока не статична и не анимация атаки

				attack_cooldown_accum = 0; // cooldown reset

				idle.reset(); // новая анимация
			}

			if (attack.frame_count > 25) {
				player_pos_is_static = false;
				player_static_pos_cooldown_acc = 0;
			}
		}
		else {
			idle.frame_update(delta_time);

			attack_cooldown_accum += delta_time;

			// move to player

			move_to2d(pos, player_pos, dp, (player_pos - pos).normalize() * SLIME_DDP_SPEED, delta_time);

			// игрока никто не ест и мы близко к игроку и
			if (!player_pos_is_static && (player_pos - pos).getLen() <= 5 &&

				// и перезарядка атаки прошла и перезарядка игрока тоже
				attack_cooldown_accum > SLIME_ATTACK_COOLDOWN && player_static_pos_cooldown_acc > PLAYER_STATICPOS_COOLDOWN) {

				// attack animation beginner

				player_pos_is_static = is_attack = true; // игрок не может двигаться и у нас анимация атаки

				pos = player_pos; // прыгаем на игрока

				attack.reset(); // новая анимация
			}
		}
	}

	void draw() {

		if (is_attack) {
			if (attack.frame_count >= 9 && attack.frame_count <= 25) {
				draw_sprite(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SP_slime_large_shadow, 128);
			}
			else {
				draw_sprite(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SP_slime_small_shadow, 128);
			}
			attack.draw(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SLIME_ALPHA);
		}
		else {
			draw_sprite(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SP_slime_small_shadow, 128);
			idle.draw(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SLIME_ALPHA);
		}

		if (debug_mode) {
			auto circle = get_collision().circle;
			static_pos_update(circle.pos, !camera_mod);

			draw_circle(circle, Color(0xffffff, 128));
		}
	}
};
