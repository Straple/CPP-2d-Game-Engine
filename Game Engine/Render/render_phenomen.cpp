
void draw_rain(u32 count_water) {
	clear_screen(Color(0, 0, 255, 16));

	std::uniform_int_distribution<s32> random_range_x(0, render_state.width);
	std::uniform_int_distribution<s32> random_range_y(0, render_state.height);

	for (u32 i = 0; i < count_water; i++) {

		s32 x = random_range_x(rnd);
		s32 y = random_range_y(rnd);

		draw_rect_in_pixels(x, y, x + 2, y + 30, Color(0, 0, 255, 24));
	}
}
