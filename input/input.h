
// -------------------------------------------------------------------------------------------------
// Open

proc b8 input_layer_open(void);

// -------------------------------------------------------------------------------------------------
// Input Structures

typedef u64 IButton_State;
enum IButton_State {
	IButton_State_Pressed,
	IButton_State_Released,
};
typedef struct IButton IButton;
struct IButton {
	b8 down;
	u32 htc;
	u64 time;
};
typedef struct IAxis IAxis;
struct IAxis {
	f32 min, max;
	f32 value;
};

proc b8
ibtn_pressed(IButton btn);

proc b8
ibtn_released(IButton btn);

proc b8
ibtn_down(IButton btn);

// -------------------------------------------------------------------------------------------------
// Events

typedef struct IEvent IEvent;
struct IEvent {
	u64 time; // @Note: Time in microseconds
};

typedef struct IButton_Event IButton_Event;
struct IButton_Event {
	IEvent e;
	IButton_State state;
};

typedef struct IAxis_Event IAxis_Event;
struct IAxis_Event {
	IEvent e;
	f32 value;
};

typedef struct IPosition_Event IPosition_Event;
struct IPosition_Event {
	IEvent e;
	vec2f32 pos;
};

// -------------------------------------------------------------------------------------------------
// Mouse

typedef u64 IMouse_Button_Kind;
enum IMouse_Button_Kind {
	IMouse_Button_Kind_Left,
	IMouse_Button_Kind_Right,
	IMouse_Button_Kind_Middle,
	IMouse_Button_Kind_Forward,
	IMouse_Button_Kind_Back,
	IMouse_Button_Kind_COUNT,
};

typedef struct IMouse IMouse;
struct IMouse {
	vec2f32 abs_pos;
	vec2f32 delta;
	f32 wheel;
	IButton btns[IMouse_Button_Kind_COUNT];
};

proc void
input_clear_mouse(/**/IMouse *mouse) {
	mouse->wheel = 0;
	mouse->delta = v2f32(0.f,0.f);
	for (s32 i = 0; i < array_count(mouse->btns); i += 1) {
		IButton *btn = mouse->btns + i;
		btn->htc = 0;
	}
}

// -------------------------------------------------------------------------------------------------
// Keyboard

#define IKEYBOARD_SCANCODE_COUNT 256
typedef struct IKeyboard IKeyboard;
struct IKeyboard {
	IButton btns[IKEYBOARD_SCANCODE_COUNT];
};

proc void
input_clear_keyboard(/**/IKeyboard *keyboard) {
	for (s32 i = 0; i < array_count(keyboard->btns); i += 1) {
		IButton *btn = keyboard->btns + i;
		btn->htc = 0;
	}
}

// -------------------------------------------------------------------------------------------------
// Gamepad

typedef u64 IGamepad_Event_Kind;
enum IGamepad_Event_Kind {
	IGamepad_Event_Axis,
	IGamepad_Event_Button,
};

typedef struct IGamepad_Event IGamepad_Event;
struct IGamepad_Event {
	IGamepad_Event_Kind kind;
	union {
		IAxis axis;
		IButton button;
	};
};

typedef struct IGamepad_Events IGamepad_Events;
struct IGamepad_Events {
	IGamepad_Event *events;
	s64 count;
};

typedef u64 IGamepad_Button_Kind;
enum IGamepad_Button_Kind {
	IGamepad_Button_Kind_COUNT,
};

typedef u64 IGamepad_Axis_Kind;
enum IGamepad_Axis_Kind {
	IGamepad_Axis_Kind_COUNT,
};


#define IGAMEPAD_OPAQUE_SIZE 16
typedef struct IGamepad IGamepad;
struct IGamepad {
	b8      connected;
	b8      has_rumble;
	IButton btns[IGamepad_Button_Kind_COUNT];
	IAxis   axes[IGamepad_Axis_Kind_COUNT];
	cstring name;
	u8      name_buffer[128];
	u8      opaque[IGAMEPAD_OPAQUE_SIZE];
};

proc b8
input_gamepad_hotplugged(void);

proc void
input_gamepad_connect(/**/IGamepad *gamepad);

proc void
input_gamepad_set_rumble(f32 weak_rumble, f32 strong_rumble, /**/IGamepad *gamepad);

proc IGamepad_Events
input_gamepad_read_events(IGamepad *gamepad, Arena *arena);

