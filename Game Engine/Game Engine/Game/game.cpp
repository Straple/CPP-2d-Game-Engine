#define is_down(b)  (input.buttons[b].is_down)
#define pressed(b)  (is_down(b) && input.buttons[b].changed)
#define released(b) (!is_down(b) && input.buttons[b].changed)

#include "game_objects.cpp"

Mouse mouse(SP_cursor, SP_focus_cursor, 0.08);

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

void simulate_game(Input& input, point_t delta_time) {
	clear_screen(0x4d4d4d);

	if (pressed(BUTTON_ESC)) {
		running = false;
	}


	draw_texture(dot(-50, 40), 2, 2, 0.5, SP_grass_background);

	draw_texture(dot(-40, 40), 2, 2, 0.5, SP_grass_background);

	draw_texture(dot(70, 40), 2, 2, 0.5, SP_grass_background);

	draw_texture(dot(20, 40), 2, 2, 0.5, SP_grass_background);

	for (auto& bush : Bushes) {
		bush.draw();
	}

	// накопление вектора движения
	auto accum_ddp = [&](point_t ddp_speed) -> dot {
		return dot(is_down(BUTTON_D) - is_down(BUTTON_A), is_down(BUTTON_W) - is_down(BUTTON_S)) * ddp_speed;
	};

	player.simulate(delta_time, accum_ddp(500));
	player.draw();

	draw_rect(dot(), dot(5, 5), Color(0x00ff00, 58));

	mouse.update(input);
	mouse.draw();

	camera.simulate(player.pos, delta_time);

	if (is_down(BUTTON_MOUSE_L)) {
		draw_circle(mouse.pos, 50, Color(0xff0000, 128));
	}

	if (is_down(BUTTON_MOUSE_R)) {
		draw_rect(mouse.pos, dot(0.5, 0.5), Color(0x00ff00, 128));
	}
}

#undef is_down
#undef pressed
#undef released