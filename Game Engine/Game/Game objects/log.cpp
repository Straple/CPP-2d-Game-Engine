
#define LOG_SIZE 0.3

#define LOG_DELTA_DRAW_POS dot(-30, 18) * LOG_SIZE

#define LOG_RADIUS 4

#define LOG_DP_SPEED 200

struct Log {

	dot pos;
	dot dp;

	static const s32 exp_cnt = 0;
	static const s32 hp = 1; // бессмертный

	Log(){}
	Log(const dot& p) {
		pos = p;
	}

	collision_circle get_collision() const {
		return Circle(pos, LOG_RADIUS);
	}

	void simulate(point_t delta_time) {
		dot ddp;
		simulate_move2d(pos, dp, ddp, delta_time);
	}

	void draw() const {
		draw_sprite(pos + LOG_DELTA_DRAW_POS, LOG_SIZE, SP_LOG);

		draw_collision_obj(*this);
	}
	
	void simulate_hit(const Player& player) {
		add_hit_effect(pos + dot(-10, 8) * LOG_SIZE);

		dp += player.get_dir() * LOG_DP_SPEED;
	}
};

std::vector<Log> Logs;
