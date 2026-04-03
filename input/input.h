
// -------------------------------------------------------------------------------------------------
// Input structures

typedef u64 Input_Button_State;
enum Input_Button_State {
	Input_Button_State_Pressed,
	Input_Button_State_Released,
};
typedef struct Input_Button Input_Button;
struct Input_Button {
	b8 down;
	u32 htc;
	u64 time;
};
typedef struct Input_Axis Input_Axis;
struct Input_Axis {
	u64 time;
	rng1f32 range;
	f32 value;
};

proc b8
btn_pressed(Input_Button btn);

proc b8
bnt_released(Input_Button btn);

proc b8
btn_down(Input_Button btn);

// -------------------------------------------------------------------------------------------------
// Mouse

typedef s64 Mouse_Button_Kind;
enum Mouse_Button_Kind {
	Mouse_Button_Left,
	Mouse_Button_Right,
	Mouse_Button_Middle,
	Mouse_Button_Forward,
	Mouse_Button_Back,
	Mouse_Button_Kind_COUNT,
};

typedef struct Input_Mouse Input_Mouse;
struct Input_Mouse {
	u64 abs_pos_event_time;
	u64 delta_event_time;
	u64 wheel_event_time;

	vec2f32 abs_pos;
	vec2f32 delta;
	f32 wheel;
	Input_Button btns[Mouse_Button_Kind_COUNT];
};

proc void
input_clear_mouse(Input_Mouse *mouse);

// -------------------------------------------------------------------------------------------------
// Keyboard

#define INPUT_KEYBOARD_SCANCODE_COUNT 256
typedef struct Input_Keyboard Input_Keyboard;
struct Input_Keyboard {
	Input_Button btns[INPUT_KEYBOARD_SCANCODE_COUNT];
};

proc void
input_clear_keyboard(Input_Keyboard *keyboard);

// -------------------------------------------------------------------------------------------------
// Gamepad

typedef s64 Gamepad_Button_Kind;
enum Gamepad_Button_Kind {
	Gamepad_Button_A,
	Gamepad_Button_B,
	Gamepad_Button_C,

	Gamepad_Button_X,
	Gamepad_Button_Y,
	Gamepad_Button_Z,


	Gamepad_Button_LTrigger,
	Gamepad_Button_RTrigger,

	Gamepad_Button_LBumper,
	Gamepad_Button_RBumper,

	Gamepad_Button_LThumb,
	Gamepad_Button_RThumb,

	Gamepad_Button_Select,
	Gamepad_Button_Start,
	Gamepad_Button_Mode,
	Gamepad_Button_Kind_COUNT,
};

typedef s64 Gamepad_Axis_Kind;
enum Gamepad_Axis_Kind {
	Gamepad_Axis_X,
	Gamepad_Axis_Y,
	Gamepad_Axis_Z,
	Gamepad_Axis_RX,
	Gamepad_Axis_RY,
	Gamepad_Axis_RZ,

	Gamepad_Axis_Throttle,
	Gamepad_Axis_Rudder,
	Gamepad_Axis_Wheel,
	Gamepad_Axis_Gas,
	Gamepad_Axis_Brake,

	Gamepad_Axis_Hat0x, Gamepad_Axis_Hat0y,
	Gamepad_Axis_Hat1x, Gamepad_Axis_Hat1y,
	Gamepad_Axis_Hat2x, Gamepad_Axis_Hat2y,
	Gamepad_Axis_Hat3x, Gamepad_Axis_Hat3y,

	Gamepad_Axis_Pressure,
	Gamepad_Axis_Distance,

	Gamepad_Axis_Tilt_X,
	Gamepad_Axis_Tilt_Y,
	Gamepad_Axis_Tool_Width,

	Gamepad_Axis_Kind_COUNT,
};


#define GAMEPAD_OPAQUE_SIZE 16
typedef struct Input_Gamepad Input_Gamepad;
struct Input_Gamepad {
	b8           connected;
	b8           has_rumble;
	Input_Button btns[Gamepad_Button_Kind_COUNT];
	Input_Axis   axes[Gamepad_Axis_Kind_COUNT];

	u8      name_buffer[128];
	string8 name;
};

typedef struct Gamepad_Deadzone Gamepad_Deadzone;
struct Gamepad_Deadzone {
	f32 center;
	f32 dist;
};

proc void
input_clear_gamepad(Input_Gamepad *gamepad);

global string8 gamepad_axis_names[Gamepad_Axis_Kind_COUNT] = {
	[Gamepad_Axis_X] = SC("X"),
	[Gamepad_Axis_Y] = SC("Y"),
	[Gamepad_Axis_Z] = SC("Z"),
	[Gamepad_Axis_RX] = SC("RX"),
	[Gamepad_Axis_RY] = SC("RY"),
	[Gamepad_Axis_RZ] = SC("RZ"),
	[Gamepad_Axis_Throttle] = SC("Throttle"),
	[Gamepad_Axis_Rudder] = SC("Rudder"),
	[Gamepad_Axis_Wheel] = SC("Wheel"),
	[Gamepad_Axis_Gas] = SC("Gas"),
	[Gamepad_Axis_Brake] = SC("Brake"),
	[Gamepad_Axis_Hat0x] = SC("Hat0x"),
	[Gamepad_Axis_Hat0y] = SC("Hat0y"),
	[Gamepad_Axis_Hat1x] = SC("Hat1x"),
	[Gamepad_Axis_Hat1y] = SC("Hat1y"),
	[Gamepad_Axis_Hat2x] = SC("Hat2x"),
	[Gamepad_Axis_Hat2y] = SC("Hat2y"),
	[Gamepad_Axis_Hat3x] = SC("Hat3x"),
	[Gamepad_Axis_Hat3y] = SC("Hat3y"),
	[Gamepad_Axis_Pressure] = SC("Pressure"),
	[Gamepad_Axis_Distance] = SC("Distance"),
	[Gamepad_Axis_Tilt_X] = SC("Tilt_X"),
	[Gamepad_Axis_Tilt_Y] = SC("Tilt_Y"),
	[Gamepad_Axis_Tool_Width] = SC("Tool_Width"),
};
