


typedef struct Backbuffer Backbuffer;
struct Backbuffer {
	rawptr memory;
	s64 stride;
	vec2s32 size;
};

#define MAX_GAMEPADS 8
typedef struct Input Input;
struct Input {
	Input_Mouse mouse;
	Input_Keyboard keyboard;

	Input_Gamepad gamepads[MAX_GAMEPADS];
	s64 gamepad_count;
};

proc void
game_update_and_render(Input *input, Backbuffer backbuffer, f32 dt);
