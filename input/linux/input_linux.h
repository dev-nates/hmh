
#include "sys/inotify.h"
#include "linux/input.h"

typedef struct Linux_Gamepad_Data Linux_Gamepad_Data;
struct Linux_Gamepad_Data {
	int device_number;	
	s16 rumble_effect_id;
	int file;
};
static_assert(size_of(Linux_Gamepad_Data) <= IGAMEPAD_OPAQUE_SIZE, check_linux_gamepad_data_size);

global int gamepad_notify = -1;

// -------------------------------------------------------------------------------------------------
// Linux Gamepad Buttons
typedef s64 Linux_Gamepad_Button_Kind;
enum Linux_Gamepad_Button_Kind {
	Linux_Gamepad_Button_0,
	Linux_Gamepad_Button_1,
	Linux_Gamepad_Button_2,
	Linux_Gamepad_Button_3,
	Linux_Gamepad_Button_4,
	Linux_Gamepad_Button_5,
	Linux_Gamepad_Button_6,
	Linux_Gamepad_Button_7,
	Linux_Gamepad_Button_8,
	Linux_Gamepad_Button_9,
	Linux_Gamepad_Button_10,
	Linux_Gamepad_Button_11,
	Linux_Gamepad_Button_12,
	Linux_Gamepad_Button_13,
	Linux_Gamepad_Button_14,
	Linux_Gamepad_Button_15,

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

global string linux_gamepad_button_names[Linux_Gamepad_Button_COUNT] = {
	[Linux_Gamepad_Button_0]  = SC("Button0"),
	[Linux_Gamepad_Button_1]  = SC("Button1"),
	[Linux_Gamepad_Button_2]  = SC("Button2"),
	[Linux_Gamepad_Button_3]  = SC("Button3"),
	[Linux_Gamepad_Button_4]  = SC("Button4"),
	[Linux_Gamepad_Button_5]  = SC("Button5"),
	[Linux_Gamepad_Button_6]  = SC("Button6"),
	[Linux_Gamepad_Button_7]  = SC("Button7"),
	[Linux_Gamepad_Button_8]  = SC("Button8"),
	[Linux_Gamepad_Button_9]  = SC("Button9"),
	[Linux_Gamepad_Button_10] = SC("Button10"),
	[Linux_Gamepad_Button_11] = SC("Button11"),
	[Linux_Gamepad_Button_12] = SC("Button12"),
	[Linux_Gamepad_Button_13] = SC("Button13"),
	[Linux_Gamepad_Button_14] = SC("Button14"),
	[Linux_Gamepad_Button_15] = SC("Button15"),

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

global string linux_gameapad_axis_names[Linux_Gamepad_Axis_COUNT] = {
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

proc IGamepad_Button_Kind
_gamepad_btn_from_linux_btn(Linux_Gamepad_Button_Kind kind);

proc IGamepad_Axis_Kind
_gamepad_axis_from_linux_axis(Linux_Gamepad_Axis_Kind kind);

