
typedef struct Backbuffer Backbuffer;
struct Backbuffer {
	s64 stride;
	rawptr memory;
};

proc void
game_update_and_render(Backbuffer backbuffer);
