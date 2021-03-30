
const point_t render_scale = 0.01; // масштаб рендеринга

point_t scale_factor = 0; // множитель масштабирования = render_state.height * render_scale

// рисует в пикселях
void draw_pixels(u32 x0, u32 y0, u32 x1, u32 y1, Color color) {
	if (x0 < x1 && y0 < y1) {

		Color* row = render_state.render_memory + static_cast<u64>(y0) * render_state.width;

		if (color.a == 255) {

			for (u32 y = y0; y < y1; y++) {
				fill(reinterpret_cast<u32*>(row + x0), static_cast<u32>(color), x1 - x0);

				row += render_state.width;
			}
		}
		else {
			for (u32 y = y0; y < y1; y++) {

				for (u32 x = x0; x < x1; x++) {

					row[x] = row[x].combine(color);
				}
				row += render_state.width;
			}
		}
	}
}

// рисует прямоугольник в пикселях с обработкой границ
void draw_rect_in_pixels(s32 x0, s32 y0, s32 x1, s32 y1, const Color& color) {
	x0 = clamp<s32>(0, x0, render_state.width);
	x1 = clamp<s32>(0, x1, render_state.width);

	y0 = clamp<s32>(0, y0, render_state.height);
	y1 = clamp<s32>(0, y1, render_state.height);

	draw_pixels(x0, y0, x1, y1, color);
}

// зарисовывает весь экран этим цветом
void clear_screen(const Color& color) {
	draw_pixels(0, 0, render_state.width, render_state.height, color);
}


// рисует прямоугольник
void draw_rect(dot pos, dot half_size, const Color& color) {
	pos += arena_half_size;

	// масштабирование
	pos *= scale_factor;
	half_size *= scale_factor;

	// change to pixels
	s32 x0 = static_cast<s32>(pos.x - half_size.x);
	s32 y0 = static_cast<s32>(pos.y - half_size.y);

	s32 x1 = static_cast<s32>(pos.x + half_size.x);
	s32 y1 = static_cast<s32>(pos.y + half_size.y);

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}

void draw_circle(dot pos, point_t radius, const Color& color) {

	s32 top_y = clamp<s32>(0, (pos.y + arena_half_size.y - radius) * scale_factor, render_state.height);
	s32 bottom_y = clamp<s32>(0, (pos.y + arena_half_size.y + radius) * scale_factor, render_state.height - 1);

	for (s32 y = top_y; y <= bottom_y; y++) {

		point_t h = std::min(radius, abs(y / scale_factor - arena_half_size.y - pos.y));
		
		point_t delta = sqrt(quare(radius) - quare(h));

		s32 left_x = clamp<s32>(0, (pos.x + arena_half_size.x - delta) * scale_factor, render_state.width);
		s32 right_x = clamp<s32>(-1, (pos.x + arena_half_size.x + delta) * scale_factor, render_state.width);

		draw_pixels(left_x, y, right_x + 1, y + 1, color);
	}
}

// рисует прямоугольник
void draw_rect2(dot pos0, dot pos1, const Color& color) {

	pos0 += arena_half_size;
	pos1 += arena_half_size;

	// масштабирование
	pos0 *= scale_factor;
	pos1 *= scale_factor;

	// change to pixels
	s32 x0 = static_cast<s32>(pos0.x);
	s32 y0 = static_cast<s32>(pos0.y);

	s32 x1 = static_cast<s32>(pos1.x);
	s32 y1 = static_cast<s32>(pos1.y);

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}


void static_pos_update(dot& pos, bool is_static) {
	if (!is_static) {
		pos -= camera.pos;
	}
}


// рисует спрайт
void draw_sprite(dot pos, point_t size, sprite_t sprite, u8 alpha = 255, bool is_static = !camera_mod) {

	static_pos_update(pos, is_static);


	point_t original_x = pos.x;
	dot half_size = dot(0.5, 0.5) * size;

	auto& pixels = Sprites[sprite].picture;

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

// рисует текстуру
// x_cnt, y_cnt - колво спрайтов по координатам
void draw_texture(dot pos, u32 x_cnt, u32 y_cnt, point_t size, sprite_t texture, u8 alpha = 255, bool is_static = !camera_mod) {

	static_pos_update(pos, is_static);

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
void draw_spritesheet(dot pos, point_t size, sprite_t spritesheet, u32 len_x, u32 sprite_count, u8 alpha = 255, bool is_static = !camera_mod) {

	static_pos_update(pos, is_static);

	// приготовления
	point_t original_x = pos.x;

	auto& pixels = Sprites[spritesheet].picture;

	u32 begin_x = len_x * sprite_count;
	u32 end_x = begin_x + len_x;

	for (u32 i = 0; i < pixels.getRowLen(); i++) {
		for (u32 j = begin_x; j < end_x; j++) {
			if (pixels[i][j].is_draw) {
				draw_rect2(pos, pos + dot(size, size), Color(pixels[i][j].color, alpha));
			}

			pos.x += size;
		}
		pos.y -= size;
		pos.x = original_x;
	}
}


#include "letters.cpp"

// рисует символ
void draw_symbol(char symbol, dot pos, point_t size, Color color) {

	if (symbol != ' ') {
		point_t original_x = pos.x;
		dot half_size = dot(1, 1) * (size * 0.5);

		if (symbol == 'q' || symbol == 'p' || symbol == 'y' || symbol == 'g') {
			pos.y -= 2 * size;
		}

		auto memory = get_symbol(symbol);

		for (u32 i = 0; i < 7; i++) {
			for (auto row = memory[i]; *row; row++) {
				if (*row == '0') {
					draw_rect(pos, half_size, color);
				}

				pos.x += size;
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}
}


// рисует текст
void draw_text(text_t text, dot pos, point_t size, Color color) {

	while (*text) {
		draw_symbol(*text, pos, size, color);
		pos.x += size * symbol_len(*text);
		text++;
	}
}

// рисует выравненный текст по центру
void draw_text_align(text_t text, dot pos, point_t size, Color color) {

	pos.x -= text_len(text) * size * 0.5;
	draw_text(text, pos, size, color);
}

// рисует число
template<typename T>
void draw_number(const T& number, dot pos, point_t size, Color color) {

	draw_text(cast(number).c_str(), pos, size, color);
}

#include "animation.cpp"