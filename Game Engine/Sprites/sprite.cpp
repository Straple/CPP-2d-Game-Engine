
/*		
*		+------+ 	       +------+
*		|sprite|	---->  |object|
*		+------+           +------+
*/

enum sprite_t {

	SP_bush,
	SP_small_bush,

	SP_cursor,
	SP_focus_cursor,

	SP_player,

	SP_grass_background,

	SP_medium_shadow,
	SP_large_shadow,


	SP_COUNT,
};

sprite Sprites[sprite_t::SP_COUNT];

void init_sprites() {

#define init(type, name) Sprites[type] = sprite(std::string("Sprites/") + name)


	init(SP_player, "player.esg");

	init(SP_bush, "bush.eng");
	init(SP_small_bush, "small_bush.eng");

	init(SP_cursor, "cursor.eng");
	init(SP_focus_cursor, "focus_cursor.eng");
	init(SP_grass_background, "grass_background.esg");
	init(SP_medium_shadow, "medium_shadow.eng");
	init(SP_large_shadow, "large_shadow.eng");
	// ...

#undef init
}
