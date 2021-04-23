#define is_down(b)  (input.buttons[b].is_down)
#define pressed(b)  (is_down(b) && input.buttons[b].changed)
#define released(b) (!is_down(b) && input.buttons[b].changed)

#include "game_utils.cpp"

#include "UI Objects/ui_objects.cpp"

#include "Game objects/game_objects.cpp"

// game objects

std::vector<Bush> Bushes;

std::vector<Slime> Slimes;

std::vector<Tree> Trees;

std::vector<Bat> Bats;

std::vector<Fireplace> Fireplaces = {
	dot(30, -30),
};

dot world_half_size(500, 500);

#include "game_collision.cpp"

void build_world() {
	std::uniform_int_distribution<s32> random_size(200, 200);
	std::uniform_int_distribution<s32> random_size_enemies(20, 20);
	std::uniform_int_distribution<s32> random_x(-world_half_size.x, world_half_size.x);
	std::uniform_int_distribution<s32> random_y(-world_half_size.y, world_half_size.y);

	auto random_dot = [&]() -> dot {
		return dot(random_x(rnd), random_y(rnd));
	};

	auto build = [&](auto& Objects, u32 size) {
		Objects.resize(size);

		for (auto& obj : Objects) {
			obj = random_dot();
		}
	};

	build(Bushes, random_size(rnd));
	build(Trees, random_size(rnd));

	auto get_coll = [&](auto obj) -> collision_circle {
		auto res = collision_circle(obj.get_collision());
		res.circle.radius += 5;
		return res;
	};

	for (auto& tree1 : Trees) {
		for (auto& tree2 : Trees) {
			if (verify_others_obj(tree1, tree2)) {

				update_collision(tree1, get_coll(tree2));
			}
		}
	}

	for (auto& bush1 : Bushes) {
		for (auto& bush2 : Bushes) {
			if (verify_others_obj(bush1, bush2)) {

				update_collision(bush1, get_coll(bush2));
			}
		}
	}

	for (auto& tree : Trees) {
		for (auto& bush : Bushes) {
			if (verify_others_obj(tree, bush)) {

				update_collision(tree, get_coll(bush));
			}
		}
	}
	for (auto& bush : Bushes) {
		for (auto& tree : Trees) {
			if (verify_others_obj(bush, tree)) {

				update_collision(bush, get_coll(tree));
			}
		}
	}

	for (s32 i = 0; i < Bushes.size(); i++) {
		for (s32 j = i + 1; j < Bushes.size(); j++) {
			if (Bushes[i].get_collision().trigger(get_coll(Bushes[j]))) {
				Bushes.erase(Bushes.begin() + i);
				i--;
				break;
			}
		}
	}

	for (s32 i = 0; i < Trees.size(); i++) {
		for (s32 j = i + 1; j < Trees.size(); j++) {
			if (Trees[i].get_collision().trigger(get_coll(Trees[j]))) {
				Trees.erase(Trees.begin() + i);
				i--;
				break;
			}
		}
	}

	build(Slimes, random_size_enemies(rnd));
	build(Bats, random_size_enemies(rnd));


}

// UI objects

Mouse mouse(SP_CURSOR, SP_FOCUS_CURSOR, 0.09);

collision_box box(dot(-20, -30), dot(10, -40));

button btn("click", dot(), 1, 0xffffffff, 0xffff0000, false);


void simulate_physics(const Input& input, point_t delta_time) {

	// simulate player
	{
		// накопление вектора движения
		auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> dot {
			return dot(is_down(right) - is_down(left), is_down(top) - is_down(bottom));
		};

		Players[0].simulate(delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S), is_down(BUTTON_J));

		//Players[1].simulate(delta_time, accum_ddp(BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN), is_down(BUTTON_K));

		simulate_game_collision(Players);
	}

	// simulate logs
	{
		for (auto& log : Logs) {
			log.simulate(delta_time);
		}
		simulate_game_collision(Logs);
	}

	// simulate slimes
	{
		for (auto& slime : Slimes) {
			slime.simulate(delta_time);
		}

		simulate_game_collision(Slimes);
	}

	// simulate bats
	{
		for (auto& bat : Bats) {
			bat.simulate(delta_time);
		}

		simulate_game_collision(Bats);
	}

	// simulate fireplaces
	{
		for (auto& fireplace : Fireplaces) {
			fireplace.simulate(delta_time);
		}

		if (Fireplaces[0].time <= 0) {
			// game over
		}
	}

	// simulate effects
	{
		for (u32 i = 0; i < Effects.size(); i++) {
			if (Effects[i].simulate(delta_time)) {
				Effects.erase(Effects.begin() + i);
				i--;
			}
		}
	}

	rain.simulate(delta_time);

	camera.simulate(Players[0].pos, delta_time);

	mouse.simulate(input);

	//btn.simulate(&mouse);
}

void simulate_logics(const Input& input) {
	if (pressed(BUTTON_ESC)) {
		running = false;
	}

	/*if (pressed(BUTTON_ENTER)) {
		fullscreen_mod_is_changed = true;
	}*/

	if (pressed(BUTTON_ENTER)) {
		debug_mode = !debug_mode;
	}

	if (pressed(BUTTON_K)) {
		show_locator = !show_locator;
	}
}

void render_game(const Input& input) {
	clear_screen(0xff000000);

	{
		dot p;
		static_pos_update(p, !camera_mod);
		draw_rect(p, world_half_size, 0xff4d4d4d);
	}

	draw_texture(dot(-world_half_size.x, world_half_size.y), 32, 32, 0.5, SP_GRASS_BACKGROUND);

	// draw players, slimes, bats, 
	// bushes, trees, fireplaces,
	// logs
	{
		// top sort

		struct top_sort_object {

			enum type_object {
				TO_PLAYER,
				TO_BUSH,
				TO_SLIME,
				TO_TREE,
				TO_LOG,
				TO_BAT,
				TO_FIREPLACE,

				TO_UNDEFIND,
			};

			type_object type;
			const void* ptr;

			top_sort_object() {
				type = TO_UNDEFIND;
				ptr = nullptr;
			}

			top_sort_object(const Player& player) {
				type = TO_PLAYER;
				ptr = reinterpret_cast<const void*>(&player);
			}
			top_sort_object(const Fireplace& fireplace) {
				type = TO_FIREPLACE;
				ptr = reinterpret_cast<const void*>(&fireplace);
			}
			top_sort_object(const Bat& player) {
				type = TO_BAT;
				ptr = reinterpret_cast<const void*>(&player);
			}
			top_sort_object(const Log& log) {
				type = TO_LOG;
				ptr = reinterpret_cast<const void*>(&log);
			}
			top_sort_object(const Bush& bush) {
				type = TO_BUSH;
				ptr = reinterpret_cast<const void*>(&bush);
			}
			top_sort_object(const Slime& slime) {
				type = TO_SLIME;
				ptr = reinterpret_cast<const void*>(&slime);
			}
			top_sort_object(const Tree& tree) {
				type = TO_TREE;
				ptr = reinterpret_cast<const void*>(&tree);
			}

			point_t get_y() const {
				switch (type) {
				case TO_PLAYER: {
					return reinterpret_cast<const Player*>(ptr)->pos.y;
				}break;
				case TO_BUSH: {
					return reinterpret_cast<const Bush*>(ptr)->pos.y;
				}break;
				case TO_SLIME: {
					return reinterpret_cast<const Slime*>(ptr)->pos.y;
				}break;
				case TO_TREE: {
					return reinterpret_cast<const Tree*>(ptr)->pos.y;
				}break;
				case TO_LOG: {
					return reinterpret_cast<const Log*>(ptr)->pos.y;
				}break;
				case TO_BAT: {
					return reinterpret_cast<const Bat*>(ptr)->pos.y;
				}break;
				case TO_FIREPLACE: {
					return reinterpret_cast<const Fireplace*>(ptr)->pos.y;
				}break;
				case TO_UNDEFIND: {
					ASSERT(false, "undefind object type");
				}break;
				}
			}

			void draw() {
				switch (type) {
				case TO_PLAYER: {
					reinterpret_cast<const Player*>(ptr)->draw();
				}break;
				case TO_BUSH: {
					reinterpret_cast<const Bush*>(ptr)->draw();
				}break;
				case TO_SLIME: {
					reinterpret_cast<const Slime*>(ptr)->draw();
				}break;
				case TO_TREE: {
					reinterpret_cast<const Tree*>(ptr)->draw();
				}break;
				case TO_LOG: {
					reinterpret_cast<const Log*>(ptr)->draw();
				}break;
				case TO_BAT: {
					reinterpret_cast<const Bat*>(ptr)->draw();
				}break;
				case TO_FIREPLACE: {
					reinterpret_cast<const Fireplace*>(ptr)->draw();
				}break;
				case TO_UNDEFIND: {
					ASSERT(false, "undefind object type");
				}break;
				}
			}

			bool operator < (const top_sort_object& Rhs) const {
				return get_y() > Rhs.get_y();
			}
		};

		std::vector<top_sort_object> Objects;
		for (auto& player : Players) {
			Objects.push_back(player);
		}
		for (auto& fireplace : Fireplaces) {
			Objects.push_back(fireplace);
		}
		for (auto& slime : Slimes) {
			Objects.push_back(slime);
		}
		for (auto& bush : Bushes) {
			Objects.push_back(bush);
		}
		for (auto& tree : Trees) {
			Objects.push_back(tree);
		}
		for (auto& log : Logs) {
			Objects.push_back(log);
		}
		for (auto& bat : Bats) {
			Objects.push_back(bat);
		}

		std::stable_sort(Objects.begin(), Objects.end());

		for (auto& obj : Objects) {
			obj.draw();
		}
	}

	// draw effects
	{
		for (u32 i = 0; i < Effects.size(); i++) {
			Effects[i].draw();
		}
	}

	rain.draw();
	
	// draw collision box
	/*{
		dot pos0(box.p0.x, box.p1.y);
		static_pos_update(pos0, !camera_mod);

		dot pos1(box.p1.x, box.p0.y);
		static_pos_update(pos1, !camera_mod);

		draw_rect2(pos0, pos1, 0x30ffffff);
	}*/
	
	mouse.draw();

	// draw button and button count
	/*{
		static u32 cnt = 0;
		if (btn.coll.trigger(mouse.pos) && pressed(BUTTON_MOUSE_L)) {
			cnt++;
		}

		btn.draw();

		draw_object(cnt, dot(20, 20), 1, 0xffffffff);
	}*/


	if (is_down(BUTTON_MOUSE_L)) {
		draw_circle(Circle(mouse.pos, 40), Color(0xff00ff, 100));
	}

	if (is_down(BUTTON_MOUSE_R)) {
		draw_circle(Circle(mouse.pos, 5), Color(0x00ff00, 100));
	}

	/*draw_text_align("\
+---------------+\n\
|1234567890+-!?.|\n\
+---------------+\n\
", dot(0, 0), 0.6, 0xffffffffff);
	draw_text_align("\
+--------------------------+\n\
|ABCDEFGHIJKLMNOPQRSTUVWXYZ|\n\
|abcdefghijklmnopqrstuvwxyz|\n\
+--------------------------+\n\
", dot(0, -20), 0.6, 0xffffffffff);*/
}

void simulate_game(const Input& input, point_t delta_time) {

	// update render_scale
	{
		if (is_down(BUTTON_UP)) {

			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale *= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}

		if (is_down(BUTTON_DOWN)) {
			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale /= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}
	}

	simulate_physics(input, delta_time);

	simulate_logics(input);

	render_game(input);

	draw_object(Players[0].exp, dot(5 - arena_half_size.x, -5 + arena_half_size.y), 0.6, Color(0xfffff000));

	draw_object(Players[0].hp, dot(5 - arena_half_size.x, -10 + arena_half_size.y), 0.6, Color(0xffff0000));

	// player attack
	{
		if (Players[0].simulate_attack(Logs) | Players[0].simulate_attack(Slimes) | Players[0].simulate_attack(Bats) | Players[0].simulate_attack(Trees)) {
			Players[0].now_is_attached = false;
		}
	}
}

#undef is_down
#undef pressed
#undef released
