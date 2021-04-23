// visibility
#define SLIME_ALPHA 210
#define SLIME_SIZE 0.4
#define SLIME_DELTA_DRAW_POS dot(-30, 38) * SLIME_SIZE
#define SLIME_FRAME_DURATION 1.0 / 7

// physics
#define SLIME_DDP_SPEED 250
#define SLIME_PUSH_DP 400
#define SLIME_DIST_JUMP 8

// radius
#define SLIME_COLLISION_RADIUS 3
#define SLIME_LOCATOR_RADIUS 30
#define SLIME_PERSEC_RADIUS 40 // преследование

// times
#define SLIME_ATTACK_COOLDOWN 3
#define SLIME_PARALYZED_COOLDOWN 0.3
#define SLIME_WALK 2
#define SLIME_WALK_SUM 3

// settings
#define SLIME_HP 150

#define SLIME_DAMAGE 100

struct Slime {

	// pos and move
	dot pos;
	dot ddp;
	dot dp;

	dot walk_to;

	// cooldowns
	point_t attack_cooldown_accum = SLIME_ATTACK_COOLDOWN;
	point_t paralyzed_cooldown_accum = SLIME_PARALYZED_COOLDOWN;
	point_t walk_accum = 0;

	// settings
	s32 hp = SLIME_HP;
	static const s32 exp_cnt = 5;
	s32 target = -1; // цели преследования нет


	// animation
	animation idle = animation(SP_SLIME, 0, 24, SLIME_FRAME_DURATION, 64);
	animation attack = animation(SP_SLIME, 25, 30, SLIME_FRAME_DURATION, 64);

	// boolean
	bool is_attack = false;

	Slime(){}
	Slime(const dot& p) {
		walk_to = pos = p;
		std::uniform_int_distribution<s64> random_range(1, 1000);
		walk_accum = (random_range(rnd) / 1000.0) * SLIME_WALK_SUM;
	}

	collision_circle get_collision() const {
		return Circle(pos, SLIME_COLLISION_RADIUS);
	}

	void simulate(point_t delta_time) {
		paralyzed_cooldown_accum += delta_time;
		attack_cooldown_accum += delta_time;

		if (paralyzed_cooldown_accum < SLIME_PARALYZED_COOLDOWN) {
			simulate_move2d(pos, dp, ddp, delta_time);
		}
		else {
			ddp = dot();

			if (is_attack) {

				Player& player = Players[target];

				u32 prev_frame_count = attack.frame_count;

				attack.frame_update(delta_time);

				// анимация атаки закончилась
				if (prev_frame_count > attack.frame_count) {
					// idle animation beginner

					player.is_paralyzed = is_attack = false; // позиция игрока не статична и не анимация атаки

					attack_cooldown_accum = 0; // cooldown reset

					idle.reset(); // новая анимация
				}

				// шарик лопнул
				if (attack.frame_count > 25 && player.is_paralyzed) {

					// push player
					player.dp = Circle(dot(), SLIME_PUSH_DP).get_random_dot();

					player.is_paralyzed = false; // у игрока не статическая позиция
					player.paralyzed_cooldown_acc = 0; // перезарядка

					player.hp -= SLIME_DAMAGE;

					add_hit_effect(player.pos + dot(-8, 16) * PLAYER_SIZE);
				}
			}
			else {
				idle.frame_update(delta_time);

				// цель потеряна
				if (target != -1 && (Players[target].is_paralyzed || (Players[target].pos - pos).getLen() > SLIME_PERSEC_RADIUS)) {
					target = -1;
					walk_accum = SLIME_WALK;
				}

				if (target == -1) { // цели нет

					// поищем

					// найдем самого близжайшего игрока
					for(s32 i = 0; i < Players.size(); i++) {
						if (Players[i].is_paralyzed) {
							continue;
						}

						if (target == -1 || (Players[target].pos - pos).getLen() > (Players[i].pos - pos).getLen()) {
							target = i;
						}
					}

					// не подходит. Слишком далеко
					if (target != -1 && (Players[target].pos - pos).getLen() > SLIME_LOCATOR_RADIUS) {
						target = -1;
					}
				}

				if (target != -1) {

					auto& player = Players[target];

					move_to2d(pos, player.pos, dp, (player.pos - pos).normalize() * SLIME_DDP_SPEED, delta_time);

					// игрока никто не ест и мы близко к игроку и
					if (!player.is_paralyzed && (player.pos - pos).getLen() <= SLIME_DIST_JUMP &&

						// и перезарядка атаки прошла и перезарядка игрока тоже
						attack_cooldown_accum >= SLIME_ATTACK_COOLDOWN && player.paralyzed_cooldown_acc >= PLAYER_STATICPOS_COOLDOWN) {

						// attack animation beginner

						player.is_paralyzed = is_attack = true; // игрок не может двигаться и у нас анимация атаки

						pos = player.pos; // прыгаем на игрока

						attack.reset(); // новая анимация
					}
				}
				else {
					// цели нет. Можем погулять

					walk_accum += delta_time;

					if (pos == walk_to) {
						walk_to = Circle(pos, 20).get_random_dot();
					}

					if (walk_accum > SLIME_WALK_SUM) {
						walk_accum = 0;
						walk_to = Circle(pos, 20).get_random_dot();
					}

					if (walk_accum < SLIME_WALK) {
						move_to2d(pos, walk_to, dp, (walk_to - pos).normalize() * SLIME_DDP_SPEED, delta_time);
					}
				}
			}
		}
	}

	void draw() const {

		if (is_attack) {
			if (attack.frame_count >= 9 && attack.frame_count <= 25) {
				draw_sprite(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SP_SLIME_LARGE_SHADOW, shadow_pixel_func);
			}
			else {
				draw_sprite(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func);
			}
			attack.draw(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, alpha_pixel_func<SLIME_ALPHA>);
		}
		else {
			draw_sprite(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func);

			idle.draw(pos + SLIME_DELTA_DRAW_POS, SLIME_SIZE, [&](Color color) {
				return paralyzed_cooldown_accum < SLIME_PARALYZED_COOLDOWN ? 
					WHITE :
					Color(color.operator u32(), SLIME_ALPHA);
			});
		}
		
		draw_collision_obj(*this);

		draw_hp(*this);

		if (show_locator) {
			dot p = pos;
			static_pos_update(p, !camera_mod);

			draw_circle(Circle(p, SLIME_PERSEC_RADIUS), Color(0xf000f0, 64));

			draw_circle(Circle(p, SLIME_LOCATOR_RADIUS), Color(0xfff000, 32));

			draw_circle(Circle(p, SLIME_DIST_JUMP), Color(0xff0000, 16));

			draw_object(walk_accum, p, 0.4, 0xffffffff);
			
			p = walk_to;
			static_pos_update(p, !camera_mod);

			draw_rect(p, dot(0.3, 0.3), 0xffff0000);
		}
	}

	void simulate_hit(const Player& player) {
		if (is_attack) {
			return;
		}

		add_hit_effect(pos + dot(-8, 8) * SLIME_SIZE);

		hp -= player.damage;

		if (hp <= 0) {
			add_death_effect(pos + dot(-15, 15) * SLIME_SIZE);
		}
		else {
			ddp += player.get_dir() * SLIME_DDP_SPEED * 1.5;
			paralyzed_cooldown_accum = 0;
		}
	}
};
