
// функции внутри game objects
// simulate - обновить объект
// draw - рисовать объект

// если у объекта есть коллизия, то нужно написать фукнцию get_collision

#include "bush.cpp"

struct Mouse {

	sprite_t sprite;
	sprite_t focus_sprite;

	point_t size;

	dot pos;
	dot focus_pos; // позиция точки, когда мы нажали на мышку и удерживали

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

	// обновит состояние фокуса мыши
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

#include "player.cpp"

#include "slime.cpp"