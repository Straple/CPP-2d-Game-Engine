#define is_down(b)  (input.buttons[b].is_down)
#define pressed(b)  (is_down(b) && input.buttons[b].changed)
#define released(b) (!is_down(b) && input.buttons[b].changed)

#include "game_objects.cpp"

// global variables

Player player;

std::vector<Bush> Bushes = {
	Bush(dot(-10, -10)),
	Bush(dot(-60, 10)),
	Bush(dot(20, -70)),
	Bush(dot(35, -10)),
	Bush(dot(-50, -40)),
	Bush(dot(-40, -10)),
	Bush(dot(8, 50)),
	Bush(dot(-20, 10)),
	Bush(dot(-39, 25)),
	Bush(dot(40, 50)),
};

std::vector<Slime> Slimes = {
	Slime(dot(30, -12)),
	Slime(dot(-20, 7)),
};



Mouse mouse(SP_cursor, SP_focus_cursor, 0.09);

collision_box box(dot(-20, -30), dot(10, -40));

button btn("click", dot(), 1, 0xffffffff, 0xffff0000, false);


void simulate_physics(const Input& input, point_t delta_time) {

	// simulate player
	{
		// накопление вектора движения
		auto accum_ddp = [&input]() -> dot {
			return dot(is_down(BUTTON_D) - is_down(BUTTON_A), is_down(BUTTON_W) - is_down(BUTTON_S));
		};

		player.simulate(delta_time, accum_ddp());

		box.bubble(&player.pos);

		for (auto& bush : Bushes) {
			bush.get_collision().bubble(&player.pos);
		}
	}

	// simulate slimes
	{
		for (auto& slime : Slimes) {
			slime.simulate(player.pos, delta_time);
		}

		for (auto& slime1 : Slimes) {
			for (auto& slime2 : Slimes) {

				// slime1 выталкивает slime2

				// позиция атакующего слайма статична
				if (&slime1 != &slime2 && !slime2.is_attack) {
					collision_circle coll = Circle(slime1.pos, /*!!!*/ 2 * SLIME_COLLISION_RADIUS);
					coll.bubble(&slime2.pos);
				}
			}
		}

		// box
		for (auto& slime : Slimes) {
			if (!slime.is_attack) {
				box.bubble(&slime.pos);
			}
		}

		// bushes
		for (auto& bush : Bushes) {

			auto collision = bush.get_collision();
			collision.circle.radius += SLIME_COLLISION_RADIUS;

			for (auto& slime : Slimes) {
				if (!slime.is_attack) {
					collision.bubble(&slime.pos);
				}
			}
		}
	}

	camera.simulate(player.pos, delta_time);

	mouse.simulate(input);

	//btn.simulate(&mouse);
}

void render_game(const Input& input) {

	clear_screen(0xff4d4d4d);

	draw_texture(dot(-50, 20), 4, 2, 0.5, SP_grass_background);

	// draw player, slimes and bushes
	{
		// то что выше рисуем первым

		std::sort(Bushes.begin(), Bushes.end(), [](const Bush& Lhs, const Bush& Rhs) -> bool {
			return Lhs.pos.y > Rhs.pos.y;
		});

		std::sort(Slimes.begin(), Slimes.end(), [](const Slime& Lhs, const Slime& Rhs) -> bool {
			return Lhs.pos.y > Rhs.pos.y;
		});

		u32 bush_i, slime_i;
		bush_i = slime_i = 0;

		bool player_is_draw = false;

		while (bush_i < Bushes.size() && slime_i < Slimes.size()) {

			// игрок не был нарисован и он выше остальных
			if (!player_is_draw && 
				player.pos.y == std::max({ player.pos.y, Bushes[bush_i].pos.y, Slimes[slime_i].pos.y })) {
				
				player_is_draw = true;
				player.draw();
			}
			else if(Bushes[bush_i].pos.y > Slimes[slime_i].pos.y) {
				Bushes[bush_i].draw();
				bush_i++;
			}
			else {
				Slimes[slime_i].draw();
				slime_i++;
			}
		}
		
		while (bush_i < Bushes.size()) {
			if (!player_is_draw && player.pos.y > Bushes[bush_i].pos.y) {
				player_is_draw = true;
				player.draw();
			}
			else {
				Bushes[bush_i].draw();
				bush_i++;
			}
		}

		while (slime_i < Slimes.size()) {
			if (!player_is_draw && player.pos.y > Slimes[slime_i].pos.y) {
				player_is_draw = true;
				player.draw();
			}
			else {
				Slimes[slime_i].draw();
				slime_i++;
			}
		}
		
		if (!player_is_draw) {
			player.draw();
		}
	}
	
	// draw collision box
	{
		dot pos0(box.p0.x, box.p1.y);
		static_pos_update(pos0, !camera_mod);

		dot pos1(box.p1.x, box.p0.y);
		static_pos_update(pos1, !camera_mod);

		draw_rect2(pos0, pos1, 0x30ffffff);
	}
	
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

	/*draw_text_align("abcdefghijklmnopqrstuvwxyz", dot(0, -10), 0.6, 0xffffffffff);
	draw_text_align("1234567890 +-!?.", dot(0, -20), 0.6, 0xffffffffff);
	draw_text_align("ABCDEFGHIJKLMNOPQRSTUVWXYZ", dot(0, -30), 0.6, 0xffffffffff);*/

	/*draw_object(camera.pos.x, dot(), 1, 0xffffffff);
	draw_object(camera.pos.y, dot(0, 18), 1, 0xffffffff);*/
	
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


	if (pressed(BUTTON_ESC)) {
		running = false;
	}

	if (pressed(BUTTON_ENTER)) {
		fullscreen_mod_is_changed = true;
	}

	if (pressed(BUTTON_K)) {
		debug_mode = !debug_mode;
	}

	simulate_physics(input, delta_time);

	render_game(input);
}

#undef is_down
#undef pressed
#undef released
