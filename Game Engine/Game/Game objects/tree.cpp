
#define TREE_SIZE 0.5

#define TREE_HP 200

#define TREE_COLLISION_RADIUS 5

#define TREE_DELTA_DRAW_POS dot(-16, 36) * TREE_SIZE

struct Tree {
	dot pos;

	s32 hp = TREE_HP;
	static const s32 exp_cnt = 4;

	Tree(){}
	Tree(const dot& p) {
		pos = p;
	}

	collision_circle get_collision() const {
		return Circle(pos, TREE_COLLISION_RADIUS);
	}

	void draw() const {
		draw_sprite(pos + dot(-16, 6) * TREE_SIZE, TREE_SIZE, SP_LARGE_SHADOW);
		draw_sprite(pos + TREE_DELTA_DRAW_POS, TREE_SIZE, SP_TREE);

		draw_collision_obj(*this);
		draw_hp(*this);
	}

	void simulate_hit(const Player& player) {
		add_hit_effect(pos + dot(-8, 8) * TREE_SIZE);

		hp -= player.damage;

		if (hp <= 0) {
			add_death_effect(pos + dot(-12, 12) * TREE_SIZE);
			Logs.push_back(pos);
		}
	}

};
