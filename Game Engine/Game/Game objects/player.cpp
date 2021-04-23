// visibility
#define PLAYER_SIZE 0.5
#define PLAYER_DELTA_DRAW_POS dot(-30.5, 41) * PLAYER_SIZE

// physics
#define PLAYER_DDP_SPEED 500
#define PLAYER_STATICPOS_COOLDOWN 5
#define PLAYER_COLLISION_RADIUS 3

#define PLAYER_MAX_COMBO 3
#define PLAYER_ATTACK_COOLDOWN 1.3
#define PLAYER_RECOVERY_COOLDOWN 1

struct Player {

	// движение и позиция
	dot pos;
	dot dp;

	// info
	s32 max_hp = 200;
	s32 hp = max_hp;
	s32 damage = 70;
	s32 exp = 0;

	// settings
	point_t paralyzed_cooldown_acc = PLAYER_STATICPOS_COOLDOWN;
	point_t time_between_attack = PLAYER_ATTACK_COOLDOWN;
	point_t time_recovery_hp = 0;
	s32 cnt_combo = 0;

	bool is_attack = false;
	bool now_is_attached = false;

	bool is_paralyzed = false;

	// дерево анимаций игрока
	struct Player_anim_tree {

		std::vector<animation> Anims = {
			animation(SP_PLAYER, 0, 6, 0.1, 64),  // run right
			animation(SP_PLAYER, 6, 6, 0.1, 64),  // run up
			animation(SP_PLAYER, 12, 6, 0.1, 64), // run left
			animation(SP_PLAYER, 18, 6, 0.1, 64), // run down

			animation(SP_PLAYER, 24, 4, 0.12, 64), // attack right
			animation(SP_PLAYER, 28, 4, 0.12, 64), // attack up
			animation(SP_PLAYER, 32, 4, 0.12, 64), // attack left
			animation(SP_PLAYER, 36, 4, 0.12, 64), // attack down

			animation(SP_PLAYER, 40, 5, 0.1, 64), // roll right
			animation(SP_PLAYER, 45, 5, 0.1, 64), // roll up
			animation(SP_PLAYER, 50, 5, 0.1, 64), // roll left
			animation(SP_PLAYER, 55, 5, 0.1, 64), // roll down

			animation(SP_PLAYER, 0, 1, 0.1, 64),  // idle right
			animation(SP_PLAYER, 6, 1, 0.1, 64),  // idle up
			animation(SP_PLAYER, 12, 1, 0.1, 64), // idle left
			animation(SP_PLAYER, 18, 1, 0.1, 64), // idle down
		};

		enum anim_t {
			NONE,
			RUN,
			ATTACK,
			ROLL,
			IDLE,

			COUNT,
		};

		struct anim_and_dir_t {
			anim_t anim = NONE;
			direction_t dir = direction_t::NONE;

			u32 get_num() const {
				return (static_cast<u32>(anim) - 1) * 4 + (static_cast<u32>(dir) - 1);
			}
		};

		// возвращает анимацию
		static anim_and_dir_t get_anim(dot dir, anim_and_dir_t type) {
			// дефолтная анимация
			if (type.anim == NONE) {
				return { IDLE, direction_t::RIGHT };
			}
			// run
			else if (dir.x < 0) {
				return { RUN, direction_t::LEFT };
			}
			else if (dir.x > 0) {
				return { RUN, direction_t::RIGHT };
			}
			else if (dir.y < 0) {
				return { RUN, direction_t::DOWN };
			}
			else if (dir.y > 0) {
				return { RUN, direction_t::UP };
			}
			// idle
			else {
				return { IDLE, type.dir };
			}
		}
	} player_anim_tree;

	// предыдущая анимация
	Player_anim_tree::anim_and_dir_t anim_type;

	Player() {}

	Player(dot pos) {
		this->pos = pos;
	}

	void draw() const {
		draw_sprite(pos + dot(-7, 4) * PLAYER_SIZE, PLAYER_SIZE, SP_MEDIUM_SHADOW);

		player_anim_tree.Anims[anim_type.get_num()].draw(pos + PLAYER_DELTA_DRAW_POS, PLAYER_SIZE);

		draw_collision_obj(*this);
	}

	void simulate(point_t delta_time, dot ddp, bool is_attached) {

		if (is_paralyzed) {
			// игрок не может двигаться
			anim_type.anim = Player_anim_tree::IDLE;
			is_attack = now_is_attached = false;
		}
		else {

			time_recovery_hp += delta_time;

			if (time_recovery_hp >= PLAYER_RECOVERY_COOLDOWN) {
				time_recovery_hp = 0;
				hp = std::min(hp + 1, max_hp);
			}

			time_between_attack += delta_time;
			paralyzed_cooldown_acc += delta_time;
			
			// ddp - вектор направления игрока

			// нужно понять какую анимацию воспроизводить

			// игрок начал атаковать
			if (is_attached && !is_attack) {

				bool new_attack = false;

				if (time_between_attack <= 0.2 && cnt_combo < PLAYER_MAX_COMBO - 1) {
					new_attack = true;
					cnt_combo++;
				}
				else if (time_between_attack >= PLAYER_ATTACK_COOLDOWN) {
					new_attack = true;
					cnt_combo = 0;
				}

				if (new_attack) {
					is_attack = true;
					now_is_attached = true;

					anim_type.anim = Player_anim_tree::ATTACK;
					player_anim_tree.Anims[anim_type.get_num()].reset();

					dp = dot();
				}
			}

			if (is_attack) {
				u32 prev_frame = player_anim_tree.Anims[anim_type.get_num()].frame_count;

				player_anim_tree.Anims[anim_type.get_num()].frame_update(delta_time);

				// удар закончился
				if (player_anim_tree.Anims[anim_type.get_num()].frame_count < prev_frame) {
					now_is_attached = is_attack = false;
					anim_type.anim = Player_anim_tree::IDLE;
					time_between_attack = 0;
				}
			}
			else {

				// simulate move
				{
					ddp *= PLAYER_DDP_SPEED;

					simulate_move2d(pos, dp, ddp, delta_time);
				}

				// текущая анимация+
				auto current_anim = player_anim_tree.get_anim(ddp.normalize(), anim_type);

				if (anim_type.get_num() != current_anim.get_num()) { // у нас сменились анимации
					is_attack = false;

					anim_type = current_anim;

					player_anim_tree.Anims[current_anim.get_num()].reset();

					// начинаем с 1 кадра, чтобы мы сразу начинали движение
					player_anim_tree.Anims[current_anim.get_num()].frame_count = std::min<u32>(1, player_anim_tree.Anims[current_anim.get_num()].frame_size - 1);
				}

				player_anim_tree.Anims[anim_type.get_num()].frame_update(delta_time);
			}
		}
	}

	// возвращает направление анимации игрока
	dot get_dir() const {
		return get_direction(anim_type.dir);
	}

	collision_circle get_collision() const {
		return Circle(pos, PLAYER_COLLISION_RADIUS);
	}

	collision_box get_attack_collision() const {
		collision_box box;

		switch (anim_type.dir) {
			case direction_t::RIGHT: {
				box = collision_box(pos + dot(0, 14), pos + dot(15, -4));
			}break;
			case direction_t::LEFT: {
				box = collision_box(pos + dot(-15, 14), pos + dot(0, -4));
			}break;
			case direction_t::UP: {
				box = collision_box(pos + dot(-9, 20), pos + dot(9, 0));
			}break;
			case direction_t::DOWN: {
				box = collision_box(pos + dot(-9, 3), pos + dot(9, -14));
			}break;
		}

		// show
		if(debug_mode) {
			dot top_left = dot(box.p0.x, box.p1.y);
			static_pos_update(top_left, !camera_mod);

			dot bottom_right = dot(box.p1.x, box.p0.y);
			static_pos_update(bottom_right, !camera_mod);

			draw_rect2(top_left, bottom_right, now_is_attached ? Color(0xffffff, 100) : Color(0xff0000, 100));
		}

		return box;
	}

	bool attack_trigger(const dot& p) {

		return is_attack && now_is_attached && // attached
			player_anim_tree.Anims[anim_type.get_num()].frame_count >= 1 && // animation checked
			get_attack_collision().trigger(p); // collision checked
	}

	// вернет правду, если атака кого-то зацепила
	template<typename container_t>
	bool simulate_attack(container_t& Enemies) {

		bool is_attack = false;

		for (u32 i = 0; i < Enemies.size(); i++) {
			if (attack_trigger(Enemies[i].pos)) {
				is_attack = true;

				Enemies[i].simulate_hit(*this);
			}
		}

		for (u32 i = 0; i < Enemies.size(); i++) {

			if (Enemies[i].hp <= 0) {
				exp += Enemies[i].exp_cnt;
				Enemies.erase(Enemies.begin() + i);
				i--;
			}
		}

		return is_attack;
	}
};

std::vector<Player> Players = {
	Player(), // you
	// other players
	//Player(dot(30, 25)),
};
