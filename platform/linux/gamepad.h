// -------------------------------------------------------------------------------------------------
// Linux gamepad

#define LINUX_GAMEPAD_UNIQUE_MAX_SIZE 128
typedef struct Linux_Gamepad Linux_Gamepad;
struct Linux_Gamepad {
	int device_number;
	s16 rumble_effect_id;
	int file;

	u8 unique_str_buffer[LINUX_GAMEPAD_UNIQUE_MAX_SIZE];
	string8 unique_str;

	Input_Gamepad *ptr;
};

// -------------------------------------------------------------------------------------------------
// Linux Gamepad Buttons
typedef s64 Linux_Gamepad_Button_Kind;
enum Linux_Gamepad_Button_Kind {
	Linux_Gamepad_Button_A,
	Linux_Gamepad_Button_B,
	Linux_Gamepad_Button_C,
	Linux_Gamepad_Button_X,
	Linux_Gamepad_Button_Y,
	Linux_Gamepad_Button_Z,
	Linux_Gamepad_Button_TL,
	Linux_Gamepad_Button_TR,
	Linux_Gamepad_Button_TL2,
	Linux_Gamepad_Button_TR2,
	Linux_Gamepad_Button_SELECT,
	Linux_Gamepad_Button_START,
	Linux_Gamepad_Button_MODE,
	Linux_Gamepad_Button_THUMBL,
	Linux_Gamepad_Button_THUMBR,

	Linux_Gamepad_Button_COUNT,
};

global string8 linux_gamepad_button_names[Linux_Gamepad_Button_COUNT] = {
	[Linux_Gamepad_Button_A]        = SC("A"),
	[Linux_Gamepad_Button_B]        = SC("B"),
	[Linux_Gamepad_Button_C]        = SC("C"),
	[Linux_Gamepad_Button_X]        = SC("X"),
	[Linux_Gamepad_Button_Y]        = SC("Y"),
	[Linux_Gamepad_Button_Z]        = SC("Z"),
	[Linux_Gamepad_Button_TL]       = SC("TL"),
	[Linux_Gamepad_Button_TR]       = SC("TR"),
	[Linux_Gamepad_Button_TL2]      = SC("TL2"),
	[Linux_Gamepad_Button_TR2]      = SC("TR2"),
	[Linux_Gamepad_Button_SELECT]   = SC("SELECT"),
	[Linux_Gamepad_Button_START]    = SC("START"),
	[Linux_Gamepad_Button_MODE]     = SC("MODE"),
	[Linux_Gamepad_Button_THUMBL]   = SC("THUMBL"),
	[Linux_Gamepad_Button_THUMBR]   = SC("THUMBR"),
};

// -------------------------------------------------------------------------------------------------
// Linux Gamepad Axis
typedef s64 Linux_Gamepad_Axis_Kind;
enum Linux_Gamepad_Axis_Kind {
	Linux_Gamepad_Axis_X,
	Linux_Gamepad_Axis_Y,
	Linux_Gamepad_Axis_Z,
	Linux_Gamepad_Axis_RX,
	Linux_Gamepad_Axis_RY,
	Linux_Gamepad_Axis_RZ,
	Linux_Gamepad_Axis_THROTTLE,
	Linux_Gamepad_Axis_RUDDER,
	Linux_Gamepad_Axis_WHEEL,
	Linux_Gamepad_Axis_GAS,
	Linux_Gamepad_Axis_BRAKE,

	Linux_Gamepad_Axis_i0,
	Linux_Gamepad_Axis_i1,
	Linux_Gamepad_Axis_i2,
	Linux_Gamepad_Axis_i3,
	Linux_Gamepad_Axis_i4,

	Linux_Gamepad_Axis_Hat0X,
	Linux_Gamepad_Axis_Hat0Y,
	Linux_Gamepad_Axis_Hat1X,
	Linux_Gamepad_Axis_Hat1Y,
	Linux_Gamepad_Axis_Hat2X,
	Linux_Gamepad_Axis_Hat2Y,
	Linux_Gamepad_Axis_Hat3X,
	Linux_Gamepad_Axis_Hat3Y,
	Linux_Gamepad_Axis_PRESSURE,
	Linux_Gamepad_Axis_DISTANCE,
	Linux_Gamepad_Axis_TILT_X,
	Linux_Gamepad_Axis_TILT_Y,
	Linux_Gamepad_Axis_TOOL_WIDTH,

	Linux_Gamepad_Axis_COUNT,
};

global string8 linux_gameapad_axis_names[Linux_Gamepad_Axis_COUNT] = {
	[Linux_Gamepad_Axis_X]          = SC("X"),
	[Linux_Gamepad_Axis_Y]          = SC("Y"),
	[Linux_Gamepad_Axis_Z]          = SC("Z"),
	[Linux_Gamepad_Axis_RX]         = SC("RX"),
	[Linux_Gamepad_Axis_RY]         = SC("RY"),
	[Linux_Gamepad_Axis_RZ]         = SC("RZ"),
	[Linux_Gamepad_Axis_THROTTLE]   = SC("THROTTLE"),
	[Linux_Gamepad_Axis_RUDDER]     = SC("RUDDER"),
	[Linux_Gamepad_Axis_WHEEL]      = SC("WHEEL"),
	[Linux_Gamepad_Axis_GAS]        = SC("GAS"),
	[Linux_Gamepad_Axis_BRAKE]      = SC("BRAKE"),

	[Linux_Gamepad_Axis_i0]         = SC("IGNORED"),
	[Linux_Gamepad_Axis_i1]         = SC("IGNORED"),
	[Linux_Gamepad_Axis_i2]         = SC("IGNORED"),
	[Linux_Gamepad_Axis_i3]         = SC("IGNORED"),
	[Linux_Gamepad_Axis_i4]         = SC("IGNORED"),

	[Linux_Gamepad_Axis_Hat0X]      = SC("HAT0X"),
	[Linux_Gamepad_Axis_Hat0Y]      = SC("HAT0Y"),
	[Linux_Gamepad_Axis_Hat1X]      = SC("HAT1X"),
	[Linux_Gamepad_Axis_Hat1Y]      = SC("HAT1Y"),
	[Linux_Gamepad_Axis_Hat2X]      = SC("HAT2X"),
	[Linux_Gamepad_Axis_Hat2Y]      = SC("HAT2Y"),
	[Linux_Gamepad_Axis_Hat3X]      = SC("HAT3X"),
	[Linux_Gamepad_Axis_Hat3Y]      = SC("HAT3Y"),
	[Linux_Gamepad_Axis_PRESSURE]   = SC("PRESSURE"),
	[Linux_Gamepad_Axis_DISTANCE]   = SC("DISTANCE"),
	[Linux_Gamepad_Axis_TILT_X]     = SC("TILT_X"),
	[Linux_Gamepad_Axis_TILT_Y]     = SC("TILT_Y"),

	[Linux_Gamepad_Axis_TOOL_WIDTH] = SC("TOOL_WIDTH"),
};

global f32 _default_linux_gamepad_axis_values[Gamepad_Axis_Kind_COUNT] = {
	[Gamepad_Axis_X]          = 128.f,
	[Gamepad_Axis_Y]          = 128.f,
	[Gamepad_Axis_Z]          = 0.f,
	[Gamepad_Axis_RX]         = 128.f,
	[Gamepad_Axis_RY]         = 128.f,
	[Gamepad_Axis_RZ]         = 0.f,
	[Gamepad_Axis_Throttle]   = 128.f,
	[Gamepad_Axis_Rudder]     = 128.f,
	[Gamepad_Axis_Wheel]      = 128.f,
	[Gamepad_Axis_Gas]        = 128.f,
	[Gamepad_Axis_Brake]      = 128.f,
	[Gamepad_Axis_Hat0x]      = 0.f,
	[Gamepad_Axis_Hat0y]      = 0.f,
	[Gamepad_Axis_Hat1x]      = 0.f,
	[Gamepad_Axis_Hat1y]      = 0.f,
	[Gamepad_Axis_Hat2x]      = 0.f,
	[Gamepad_Axis_Hat2y]      = 0.f,
	[Gamepad_Axis_Hat3x]      = 0.f,
	[Gamepad_Axis_Hat3y]      = 0.f,
	[Gamepad_Axis_Pressure]   = 128.f,
	[Gamepad_Axis_Distance]   = 128.f,
	[Gamepad_Axis_Tilt_X]     = 128.f,
	[Gamepad_Axis_Tilt_Y]     = 128.f,
	[Gamepad_Axis_Tool_Width] = 128.f,
};

#define DEADZONE_DISTANCE 10.f
global Gamepad_Deadzone _linux_gamepad_axis_deadzones[Gamepad_Axis_Kind_COUNT] = {
	[Gamepad_Axis_X]          = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Y]          = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Z]          = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_RX]         = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_RY]         = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_RZ]         = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Throttle]   = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Rudder]     = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Wheel]      = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Gas]        = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Brake]      = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Hat0x]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Hat0y]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Hat1x]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Hat1y]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Hat2x]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Hat2y]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Hat3x]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Hat3y]      = (Gamepad_Deadzone){0.f,   0.f},
	[Gamepad_Axis_Pressure]   = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Distance]   = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Tilt_X]     = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Tilt_Y]     = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
	[Gamepad_Axis_Tool_Width] = (Gamepad_Deadzone){128.f, DEADZONE_DISTANCE},
};
#undef DEADZONE_DISTANCE

proc Gamepad_Button_Kind
_gamepad_button_from_linux_button(Linux_Gamepad_Button_Kind kind) {
	Gamepad_Button_Kind arr[Linux_Gamepad_Button_COUNT] = {
		[Linux_Gamepad_Button_A]      = Gamepad_Button_A,
		[Linux_Gamepad_Button_B]      = Gamepad_Button_B,
		[Linux_Gamepad_Button_C]      = Gamepad_Button_C,

		// @Note: X and Y are reversed on Linux
		[Linux_Gamepad_Button_X]      = Gamepad_Button_Y,
		[Linux_Gamepad_Button_Y]      = Gamepad_Button_X,
		[Linux_Gamepad_Button_Z]      = Gamepad_Button_Z,
		[Linux_Gamepad_Button_TL]     = Gamepad_Button_LTrigger,
		[Linux_Gamepad_Button_TR]     = Gamepad_Button_RTrigger,
		[Linux_Gamepad_Button_TL2]    = Gamepad_Button_LBumper,
		[Linux_Gamepad_Button_TR2]    = Gamepad_Button_RBumper,
		[Linux_Gamepad_Button_SELECT] = Gamepad_Button_Select,
		[Linux_Gamepad_Button_START]  = Gamepad_Button_Start,
		[Linux_Gamepad_Button_MODE]   = Gamepad_Button_Mode,
		[Linux_Gamepad_Button_THUMBL] = Gamepad_Button_LThumb,
		[Linux_Gamepad_Button_THUMBR] = Gamepad_Button_RThumb,
	};
	assert(kind < array_count(arr));
	return arr[kind];
}

proc Gamepad_Axis_Kind
_gamepad_axis_from_linux_axis(Linux_Gamepad_Axis_Kind kind) {
	Gamepad_Axis_Kind arr[Linux_Gamepad_Axis_COUNT] = {
		[Linux_Gamepad_Axis_X]          = Gamepad_Axis_X,
		[Linux_Gamepad_Axis_Y]          = Gamepad_Axis_Y,
		[Linux_Gamepad_Axis_Z]          = Gamepad_Axis_Z,
		[Linux_Gamepad_Axis_RX]         = Gamepad_Axis_RX,
		[Linux_Gamepad_Axis_RY]         = Gamepad_Axis_RY,
		[Linux_Gamepad_Axis_RZ]         = Gamepad_Axis_RZ,
		[Linux_Gamepad_Axis_THROTTLE]   = Gamepad_Axis_Throttle,
		[Linux_Gamepad_Axis_RUDDER]     = Gamepad_Axis_Rudder,
		[Linux_Gamepad_Axis_WHEEL]      = Gamepad_Axis_Wheel,
		[Linux_Gamepad_Axis_GAS]        = Gamepad_Axis_Gas,
		[Linux_Gamepad_Axis_BRAKE]      = Gamepad_Axis_Brake,

		[Linux_Gamepad_Axis_i0]         = -1,
		[Linux_Gamepad_Axis_i1]         = -1,
		[Linux_Gamepad_Axis_i2]         = -1,
		[Linux_Gamepad_Axis_i3]         = -1,
		[Linux_Gamepad_Axis_i4]         = -1,

		[Linux_Gamepad_Axis_Hat0X]      = Gamepad_Axis_Hat0x,
		[Linux_Gamepad_Axis_Hat0Y]      = Gamepad_Axis_Hat0y,
		[Linux_Gamepad_Axis_Hat1X]      = Gamepad_Axis_Hat1x,
		[Linux_Gamepad_Axis_Hat1Y]      = Gamepad_Axis_Hat1y,
		[Linux_Gamepad_Axis_Hat2X]      = Gamepad_Axis_Hat2x,
		[Linux_Gamepad_Axis_Hat2Y]      = Gamepad_Axis_Hat2y,
		[Linux_Gamepad_Axis_Hat3X]      = Gamepad_Axis_Hat3x,
		[Linux_Gamepad_Axis_Hat3Y]      = Gamepad_Axis_Hat3y,
		[Linux_Gamepad_Axis_PRESSURE]   = Gamepad_Axis_Pressure,
		[Linux_Gamepad_Axis_DISTANCE]   = Gamepad_Axis_Distance,
		[Linux_Gamepad_Axis_TILT_X]     = Gamepad_Axis_Tilt_X,
		[Linux_Gamepad_Axis_TILT_Y]     = Gamepad_Axis_Tilt_Y,
		[Linux_Gamepad_Axis_TOOL_WIDTH] = Gamepad_Axis_Tool_Width,
	};
	assert(kind < array_count(arr));
	return arr[kind];
}
