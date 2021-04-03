#define is_down(b)  (input.buttons[b].is_down)
#define pressed(b)  (is_down(b) && input.buttons[b].changed)
#define released(b) (!is_down(b) && input.buttons[b].changed)

#include "game_objects.cpp"

Mouse mouse(SP_cursor, SP_focus_cursor, 0.09);

Player player(dot(0, 0), 0.5);

std::vector<Bush> Bushes = {
	Bush(dot(-10, -10), 0.5),
	Bush(dot(-60, 10), 0.5),
	Bush(dot(20, -70), 0.5),
	Bush(dot(35, -10), 0.5),
	Bush(dot(-50, -40), 0.5),
	Bush(dot(-40, -10), 0.5),
	Bush(dot(8, 50), 0.5),
	Bush(dot(-20, 10), 0.5),
	Bush(dot(-39, 25), 0.5),
	Bush(dot(40, 50), 0.5),
};

collision_box box(dot(-10, 10), dot(10, -10));

button btn("click", dot(), 1, 0xffffffff, 0xffff0000, false);

void simulate_physics(const Input& input, point_t delta_time) {

	// simulate player
	{
		// накопление вектора движения
		auto accum_ddp = [&](point_t ddp_speed) -> dot {
			return dot(is_down(BUTTON_D) - is_down(BUTTON_A), is_down(BUTTON_W) - is_down(BUTTON_S)) * ddp_speed;
		};

		player.simulate(delta_time, accum_ddp(500));

		box.bubble(&player.pos);

		for (auto& bush : Bushes) {
			bush.simulate(&player.pos);
		}
	}

	camera.simulate(player.pos, delta_time);

	mouse.simulate(input);

	btn.simulate(&mouse);
}

void render_game(const Input& input) {

	clear_screen(0xff4d4d4d);

	draw_texture(dot(-arena_half_size.x, arena_half_size.y), 4, 2, 0.5, SP_grass_background);

	// draw player and bushes
	{
		for (auto& bush : Bushes) {
			if (bush.pos.y > player.pos.y) {
				bush.draw();
			}
		}

		player.draw();

		for (auto& bush : Bushes) {
			if (bush.pos.y <= player.pos.y) {
				bush.draw();
			}
		}
	}
	
	draw_rect2(dot(box.p0.x, box.p1.y) - camera.pos, dot(box.p1.x, box.p0.y) - camera.pos, 0x30ffffff);

	mouse.draw();

	// draw button and button count
	{
		static u32 cnt = 0;
		if (btn.coll.trigger(mouse.pos) && pressed(BUTTON_MOUSE_L)) {
			cnt++;
		}

		btn.draw();

		draw_object(cnt, dot(20, 20), 1, 0xffffffff);
	}


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

	if (pressed(BUTTON_ESC)) {
		running = false;
	}

	if (pressed(BUTTON_ENTER)) {
		fullscreen_mod_is_changed = true;
	}

	
	simulate_physics(input, delta_time);

	render_game(input);

	
}

#undef is_down
#undef pressed
#undef released
