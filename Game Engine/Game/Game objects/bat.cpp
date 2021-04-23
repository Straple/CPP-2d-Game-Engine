// visibility
#define BAT_SIZE 0.4
#define BAT_DELTA_DRAW_POS dot(-8, 26) * BAT_SIZE
#define BAT_FRAME_DURATION 1.0 / 7

// physics
#define BAT_DDP_SPEED 300
#define BAT_DIST_JUMP 8

// radius
#define BAT_COLLISION_RADIUS 3.5
#define BAT_LOCATOR_RADIUS 40
#define BAT_PERSEC_RADIUS 50

// times
#define BAT_PARALYZED_COOLDOWN 0.3
#define BAT_ATTACK_COOLDOWN 5
#define BAT_WALK 3
#define BAT_WALK_SUM 4.5

// settings
#define BAT_HP 100
#define BAT_DAMAGE 50

struct Bat {

	// pos and move
	dot pos;
	dot dp;
	dot ddp;

	dot walk_to;

	// cooldowns
	point_t attack_cooldown_accum = BAT_ATTACK_COOLDOWN;
	point_t paralyzed_cooldown_accum = BAT_PARALYZED_COOLDOWN;
	point_t walk_accum = 0;

	// settings
	s32 hp = BAT_HP;
	static const s32 exp_cnt = 3;
	s32 target = -1; // цели преследования нет

	// animation
	animation anim = animation(SP_BAT, 0, 5, BAT_FRAME_DURATION, 16);

	Bat(){}
	Bat(const dot& p) {
		walk_to = pos = p;
		std::uniform_int_distribution<s64> random_range(1, 1000);
		walk_accum = (random_range(rnd) / 1000.0) * BAT_WALK_SUM;
	}

	collision_circle get_collision() const {
		return Circle(pos, BAT_COLLISION_RADIUS);
	}

	void simulate(point_t delta_time) {

		attack_cooldown_accum += delta_time;
		paralyzed_cooldown_accum += delta_time;

		if (paralyzed_cooldown_accum < BAT_PARALYZED_COOLDOWN) {
			simulate_move2d(pos, dp, ddp, delta_time);
		}
		else {

			ddp = dot();

			anim.frame_update(delta_time);

			if (dp.x < 0) {
				anim.sprite_sheet = SP_BAT_INVERSE;
			}
			else {
				anim.sprite_sheet = SP_BAT;
			}
			
			// цель потеряна
			if (target != -1 && (Players[target].pos - pos).getLen() > BAT_PERSEC_RADIUS) {
				target = -1;
				walk_to = pos;
				walk_accum = BAT_WALK;
			}

			if (target == -1) { // цели нет

				// поищем

				// найдем самого близжайшего игрока
				for (s32 i = 0; i < Players.size(); i++) {

					if (target == -1 || (Players[target].pos - pos).getLen() > (Players[i].pos - pos).getLen()) {
						target = i;
					}
				}

				// не подходит. Слишком далеко
				if (target != -1 && (Players[target].pos - pos).getLen() > BAT_LOCATOR_RADIUS) {
					target = -1;
				}
			}

			if (target != -1) {

				auto& player = Players[target];

				move_to2d(pos, player.pos, dp, (player.pos - pos).normalize() * BAT_DDP_SPEED, delta_time);

				// мы близко к игроку и
				if ((player.pos - pos).getLen() <= BAT_DIST_JUMP &&
					attack_cooldown_accum >= BAT_ATTACK_COOLDOWN) {

					// hit

					attack_cooldown_accum = 0;

					pos = player.pos; // прыгаем на игрока

					player.hp -= BAT_DAMAGE;
					add_hit_effect(player.pos + dot(-8, 16) * PLAYER_SIZE);
				}
			}
			else {
				// цели нет. Можем погулять

				walk_accum += delta_time;

				if (pos == walk_to) {
					walk_to = Circle(pos, 20).get_random_dot();
				}

				if (walk_accum > BAT_WALK_SUM) {
					walk_accum = 0;
					walk_to = Circle(pos, 20).get_random_dot();
				}

				if (walk_accum < BAT_WALK) {
					move_to2d(pos, walk_to, dp, (walk_to - pos).normalize() * BAT_DDP_SPEED, delta_time);
				}
			}
		}
	}

	void draw() const {

		draw_sprite(pos + dot(-5, 0) * BAT_SIZE, BAT_SIZE, SP_SMALL_SHADOW);

		anim.draw(pos + BAT_DELTA_DRAW_POS, BAT_SIZE, [&](const Color& color) {
			return paralyzed_cooldown_accum < BAT_PARALYZED_COOLDOWN ?
				WHITE :
				color;
		});

		draw_collision_obj(*this);

		draw_hp(*this);

		if (show_locator) {
			dot p = pos;
			static_pos_update(p, !camera_mod);

			draw_circle(Circle(p, BAT_PERSEC_RADIUS), Color(0xf000f0, 64));

			draw_circle(Circle(p, BAT_LOCATOR_RADIUS), Color(0xfff000, 32));

			draw_circle(Circle(p, BAT_DIST_JUMP), Color(0xff0000, 16));

			draw_object(walk_accum, p, 0.4, 0xffffffff);

			p = walk_to;
			static_pos_update(p, !camera_mod);

			draw_rect(p, dot(0.3, 0.3), 0xffff0000);
		}
	}

	// вернет правду, если враг уничтожен
	void simulate_hit(const Player& player) {
		add_hit_effect(pos + BAT_DELTA_DRAW_POS);

		hp -= player.damage;

		if (hp <= 0) {
			add_death_effect(pos + dot(-16, 22) * BAT_SIZE);
		}
		else {
			ddp += player.get_dir() * BAT_DDP_SPEED * 1.5;
			paralyzed_cooldown_accum = 0;
		}
	}
};
