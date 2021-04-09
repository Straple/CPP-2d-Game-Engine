
// функции внутри game objects
// simulate - обновить объект
// draw - рисовать объект

struct Bush {
	dot pos;
	point_t size;

	Bush(dot pos, point_t size) {
		this->pos = pos + dot(7.7, -10);
		this->size = size;
	}

	void simulate(dot* player_pos) {
		collision_circle coll_circle{ Circle(pos, 8) };

		coll_circle.bubble(player_pos);
	}

	void draw() {
		draw_sprite(pos - dot(7.7, -10), size, SP_large_shadow, 128);
		draw_sprite(pos - dot(7.7, -10), size, SP_bush);

		if (debug_mode) {
			collision_circle coll_circle{ Circle(pos, 8) };

			dot pos = coll_circle.circle.pos;
			static_pos_update(pos, !camera_mod);

			draw_circle(Circle(pos, coll_circle.circle.radius), Color(0xffffff, 50));
		}
	}
};

struct Player {

	// движение и позици€
	dot pos;
	dot dp;

	point_t size;

	// дерево анимаций игрока
	struct Player_anim_tree {
		enum player_anim_t {
			run_right,
			run_up,
			run_left,
			run_down,

			attack_right,
			attack_up,
			attack_left,
			attack_down,

			roll_right,
			roll_up,
			roll_left,
			roll_down,

			idle_right,
			idle_up,
			idle_left,
			idle_down,

			none,
		};

		std::vector<animation> Anims = {
			animation(SP_player, 0, 6, 0.1, 64),  // right
			animation(SP_player, 6, 6, 0.1, 64),  // up
			animation(SP_player, 12, 6, 0.1, 64), // left
			animation(SP_player, 18, 6, 0.1, 64), // down

			animation(SP_player, 24, 4, 0.1, 64), // attack right
			animation(SP_player, 28, 4, 0.1, 64), // attack up
			animation(SP_player, 32, 4, 0.1, 64), // attack left
			animation(SP_player, 36, 4, 0.1, 64), // attack down

			animation(SP_player, 40, 5, 0.1, 64), // roll right
			animation(SP_player, 45, 5, 0.1, 64), // roll up
			animation(SP_player, 50, 5, 0.1, 64), // roll left
			animation(SP_player, 55, 5, 0.1, 64), // roll down

			animation(SP_player, 0, 1, 0.1, 64),  // idle right
			animation(SP_player, 6, 1, 0.1, 64),  // idle up
			animation(SP_player, 12, 1, 0.1, 64), // idle left
			animation(SP_player, 18, 1, 0.1, 64), // idle down
		};

		// возвращает анимацию
		player_anim_t get_anim(dot dir, player_anim_t prevAnim) {
			// дефолтна€ анимаци€
			if (prevAnim == none) {
				return idle_right;
			}
			// run
			else if (dir.x < 0) {
				return run_left;
			}
			else if (dir.x > 0) {
				return run_right;
			}
			else if (dir.y < 0) {
				return run_down;
			}
			else if (dir.y > 0) {
				return run_up;
			}
			// idle
			else if (prevAnim == run_left) {
				return idle_left;
			}
			else if (prevAnim == run_right) {
				return idle_right;
			}
			else if (prevAnim == run_down) {
				return idle_down;
			}
			else if (prevAnim == run_up) {
				return idle_up;
			}
			else {
				return prevAnim;
			}
		}
	} player_anim_tree;

	// предыдуща€ анимаци€
	Player_anim_tree::player_anim_t prev_anim = Player_anim_tree::idle_right;

	Player(dot pos, point_t size) {
		this->pos = pos;
		this->size = size;
	}

	void draw() {
		draw_sprite(pos - dot(size, -size * 1.3) * 32, size, SP_medium_shadow, 128);
		player_anim_tree.Anims[prev_anim].draw(pos - dot(size, -size * 1.3) * 32, size);

		if (debug_mode) {
			dot p = pos;
			static_pos_update(p, !camera_mod);

			draw_circle(Circle(p, 0.6), Color(0xff0000, 128));
		}
	}

	void simulate(point_t delta_time, dot ddp) {

		simulate_move2d(pos, dp, ddp, delta_time);

		// simulate animation
		{
			// ddp - вектор направлени€ игрока

			// нужно пон€ть какую анимацию воспроизводить

			// текуща€ анимаци€+
			auto current_anim = player_anim_tree.get_anim(ddp.normalize(), prev_anim);

			if (prev_anim != current_anim) { // у нас сменились анимации
				prev_anim = current_anim;

				player_anim_tree.Anims[current_anim].frame_time_accum = 0;
				// начинаем с 1 кадра, чтобы мы сразу начинали движение
				player_anim_tree.Anims[current_anim].frame_count = std::min<u32>(1, player_anim_tree.Anims[current_anim].frame_size - 1);
			}

			player_anim_tree.Anims[current_anim].frame_update(delta_time);
		}
	}

};

struct Mouse {

	sprite_t sprite;
	sprite_t focus_sprite;

	point_t size;

	dot pos;
	dot focus_pos; // позици€ точки, когда мы нажали на мышку и удерживали

	bool focus = false;
	bool is_down = false;

	Mouse(sprite_t sprite, sprite_t focus_sprite, point_t size) {
		this->sprite = sprite;
		this->focus_sprite = focus_sprite;
		this->size = size;
	}

	void simulate(const Input& input) {
		if (pressed(BUTTON_MOUSE_L)) {
			focus_pos = pos;
		}
		is_down = is_down(BUTTON_MOUSE_L);
		focus = false;
	}

	void draw() {
		if (is_down) {
			dot pos0(std::min(focus_pos.x, pos.x), std::min(focus_pos.y, pos.y));
			dot pos1(std::max(focus_pos.x, pos.x), std::max(focus_pos.y, pos.y));
			draw_rect2(pos0, pos1, Color(0xffffff, 64));

			draw_object(Line(focus_pos, pos).get_a(), dot(), 0.3, 0xffffffff);
			draw_object(Line(focus_pos, pos).get_b(), dot(0, 10), 0.3, 0xffffffff);
			draw_object(Line(focus_pos, pos).get_c(), dot(0, 20), 0.3, 0xffffffff);

			draw_line(Line(focus_pos, pos), 0.3, 0xffffffff);
		}

		if (focus) {
			draw_sprite(pos - dot(16, 0) * size, size, focus_sprite, 0xff, true);
		}
		else {
			draw_sprite(pos, size, sprite, 0xff, true);
		}
		

		if (debug_mode) {
			draw_rect(pos, dot(0.35, 0.35), 0xffffffff);
			draw_object<s64>(pos.x, dot(65, 45), 0.5, 0xffffffff);
			draw_object<s64>(pos.y, dot(80, 45), 0.5, 0xffffffff);
		}
	}
};

// текст с колизией
struct button { // кнопка
	text_t text;
	dot pos; // центр текста по x
	point_t size;
	collision_box coll;
	Color color, // usual color
		focus_color; // color when the button doesn't focus
	bool is_align, is_focus;

	button() {}

	button(text_t _text, dot _pos, point_t _size, Color _color, Color _focus_color, bool align = false) {
		is_align = align;
		is_focus = false;
		text = _text;
		pos.x = _pos.x + 0.5f * _size;
		pos.y = _pos.y - 0.5f * _size;
		size = _size;
		color = _color;
		focus_color = _focus_color;

		// !создать коллизию!
		{
			int len = text_len(_text);

			coll.p0 = dot(_pos.x + (align ? -(size * len) * 0.5 : 0), _pos.y);
			coll.p1.x = _pos.x + size * len - size + (align ? -(size * len) * 0.5 : 0);
			coll.p1.y = _pos.y - size * 6 - size;
		}
	}

	void draw() {
		if (is_align) {
			draw_text_align(text, pos, size, (is_focus ? focus_color : color));
		}
		else {
			draw_text(text, pos, size, (is_focus ? focus_color : color));
		}

		if (debug_mode) {
			draw_rect(coll.p0, dot(0.5, 0.5), 0xff00ff00);
			draw_rect(coll.p1, dot(0.5, 0.5), 0xff00ff00);
		}
	}

	// обновит состо€ние фокуса мыши
	void simulate(Mouse* mouse) {
		if (coll.trigger(mouse->pos)) {
			mouse->focus = true;
			is_focus = true;
		}
		else {
			is_focus = false;
		}
	}
};
