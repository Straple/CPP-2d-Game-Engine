﻿
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

// рисует спрайт
void draw_sprite(dot pos, point_t size, sprite_t sprite, u8 alpha = 0xff, bool is_static = !camera_mod) {

	static_pos_update(pos, is_static);

	point_t original_x = pos.x;
	dot half_size = dot(0.5, 0.5) * size;

	auto& pixels = Sprites[sprite].picture;

	if (arena_query(pos.x - half_size.x, pos.x + pixels.getColLen() * size + half_size.x,
		pos.y + half_size.y, pos.y - pixels.getRowLen() * size - half_size.y)) {


		for (u32 i = 0; i < pixels.getRowLen(); i++) {
			for (u32 j = 0; j < pixels.getColLen(); j++) {
				if (pixels[i][j].is_draw) {
					draw_rect(pos, half_size, Color(pixels[i][j].color, alpha));
				}
				pos.x += size;
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}
}

// рисует текстуру
// x_cnt, y_cnt - колво спрайтов по координатам
void draw_texture(dot pos, u32 x_cnt, u32 y_cnt, point_t size, sprite_t texture, u8 alpha = 0xff) {

	static_pos_update(pos, !camera_mod);

	const point_t original_x = pos.x;

	auto& pixels = Sprites[texture].picture;

	const point_t x_summary = pixels.getColLen() * size;
	const point_t y_summary = pixels.getRowLen() * size;

	for (u32 i = 0; i < y_cnt; i++) {
		for (u32 j = 0; j < x_cnt; j++) {
			draw_sprite(pos, size, texture, alpha, true);
			pos.x += x_summary;
		}
		pos.x = original_x;
		pos.y -= y_summary;
	}
}

// рисует спрайт из листа спрайтов
// len_x - длина спрайта по x
// sprite_count - идентификатор спрайта
void draw_spritesheet(dot pos, point_t size, sprite_t spritesheet, u32 len_x, u32 sprite_count, u8 alpha = 0xff) {

	static_pos_update(pos, !camera_mod);

	point_t original_x = pos.x;
	dot half_size = dot(0.5, 0.5) * size;

	auto& pixels = Sprites[spritesheet].picture;

	u32 begin_x = len_x * sprite_count;
	u32 end_x = begin_x + len_x;

	if (arena_query(pos.x - half_size.x, pos.x + len_x * size + half_size.x,
		pos.y + half_size.y, pos.y - pixels.getRowLen() * size - half_size.y)) {

		for (u32 i = 0; i < pixels.getRowLen(); i++) {
			for (u32 j = begin_x; j < end_x; j++) {
				if (pixels[i][j].is_draw) {
					draw_rect(pos, half_size, Color(pixels[i][j].color, alpha));
				}

				pos.x += size;
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}
}
