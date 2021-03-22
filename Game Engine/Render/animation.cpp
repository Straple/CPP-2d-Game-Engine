

struct animation {
	sprite_t sprite_sheet; // ���� ��������

	u32 frame_begin; // ������ ����� � ����� ��������
	u32 frame_size; // ���������� ������ � ��������
	u32 frame_count; // ������� �������� ����� � 0

	point_t frame_duration; // ����������������� �����
	point_t frame_time_accum; // ����� ���������� �� ����������������� �����

	u32 len_x; // ����� ������� ������ ����� �� x

	animation(sprite_t sprite_sheet, u32 frame_begin, u32 frame_size, point_t frame_duration, u32 len_x) {
		this->sprite_sheet = sprite_sheet;
		this->frame_begin = frame_begin;
		this->frame_size = frame_size;
		this->frame_duration = frame_duration;
		this->len_x = len_x;
		frame_count = 0;
		frame_time_accum = 0;
	}

	void frame_update(point_t delta_time) {
		frame_time_accum += delta_time;
		if (frame_time_accum > frame_duration) {
			frame_time_accum = 0;
			frame_count++;
			if (frame_count >= frame_size) {
				frame_count = 0;
			}
		}
	}

	void draw(dot pos, point_t size) {
		draw_spritesheet(pos, size, sprite_sheet, len_x, frame_begin + frame_count);
	}
};

