
void static_pos_update(dot& pos, bool is_static) {
	if (!is_static) {
		pos -= camera.pos;
	}
}

// проверяет пересечение экрана и этого прямоугольника
bool arena_query(point_t left, point_t right, point_t top, point_t bottom) {
	return !(right < -arena_half_size.x || arena_half_size.x < left || // x
		     top < -arena_half_size.y || arena_half_size.y < bottom); // y
}

#define simulate_draw_pixel(color, pos, half_size)\
if (is_draw(color)) {\
	draw_rect(pos, half_size, func(color));\
}\
else if (debug_mode) {\
	draw_rect(pos, half_size, Color(0xffffff, 60));\
}

Color standart_pixel_func(const Color& color) {
	return color;
}
template<u8 alpha>
Color alpha_pixel_func(const Color& color) {
	return Color(color.r, color.g, color.b, alpha);
}

#define shadow_pixel_func alpha_pixel_func<100>

// рисует спрайт
template<typename func_t = Color(const Color& color)>
void draw_sprite_static(dot pos, point_t size, sprite_t sprite, func_t&& func = standart_pixel_func) {

	point_t original_x = pos.x;
	dot half_size = dot(0.5, 0.5) * size;

	auto& pixels = Sprites[sprite].picture;

	if (arena_query(pos.x - half_size.x, pos.x + pixels.getColLen() * size + half_size.x,
		pos.y + half_size.y, pos.y - pixels.getRowLen() * size - half_size.y)) {

		for (u32 i = 0; i < pixels.getRowLen(); i++) {
			for (u32 j = 0; j < pixels.getColLen(); j++, pos.x += size) {

				simulate_draw_pixel(pixels[i][j], pos, half_size);
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}
}

template<typename func_t = Color(const Color& color)>
void draw_sprite(dot pos, point_t size, sprite_t sprite, func_t&& func = standart_pixel_func) {

	static_pos_update(pos, !camera_mod);

	draw_sprite_static(pos, size, sprite, func);
}


// WARGNING NOT ARENA QUERY
// рисует текстуру
// x_cnt, y_cnt - колво спрайтов по координатам
template<typename func_t = Color(const Color& color)>
void draw_texture(dot pos, u32 x_cnt, u32 y_cnt, point_t size, sprite_t texture, func_t&& func = standart_pixel_func) {

	const point_t original_x = pos.x;

	auto& pixels = Sprites[texture].picture;

	const point_t x_summary = pixels.getColLen() * size;
	const point_t y_summary = pixels.getRowLen() * size;

	for (u32 i = 0; i < y_cnt; i++) {
		for (u32 j = 0; j < x_cnt; j++, pos.x += x_summary) {
			draw_sprite(pos, size, texture, func);
		}
		pos.x = original_x;
		pos.y -= y_summary;
	}
}

// рисует спрайт из листа спрайтов
// len_x - длина спрайта по x
// sprite_count - идентификатор спрайта
template<typename func_t = Color(const Color& color)>
void draw_spritesheet(dot pos, point_t size, sprite_t spritesheet, u32 len_x, u32 sprite_count, func_t&& func = standart_pixel_func) {

	static_pos_update(pos, !camera_mod);

	point_t original_x = pos.x;
	dot half_size = dot(0.5, 0.5) * size;

	auto& pixels = Sprites[spritesheet].picture;

	u32 begin_x = len_x * sprite_count;
	u32 end_x = begin_x + len_x;

	if (arena_query(pos.x - half_size.x, pos.x + len_x * size + half_size.x,
		pos.y + half_size.y, pos.y - pixels.getRowLen() * size - half_size.y)) {

		for (u32 i = 0; i < pixels.getRowLen(); i++) {

			for (u32 j = begin_x; j < end_x; j++, pos.x += size) {

				simulate_draw_pixel(pixels[i][j], pos, half_size);
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}
}
