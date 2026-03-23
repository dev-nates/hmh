
typedef u64 Default_Cursor_Kind;
enum Default_Cursor_Kind {
	Default_Cursor_Kind_Default,
	Default_Cursor_Kind_Pointer,
	Default_Cursor_Kind_Text,
	Default_Cursor_Kind_Crosshair,
	Default_Cursor_Kind_Help,
	Default_Cursor_Kind_Progress,
	Default_Cursor_Kind_Wait,
	Default_Cursor_Kind_No_Drop,
	Default_Cursor_Kind_Not_Allowed,
	Default_Cursor_Kind_Copy,

	Default_Cursor_Kind_N_Resize,
	Default_Cursor_Kind_S_Resize,
	Default_Cursor_Kind_E_Resize,
	Default_Cursor_Kind_W_Resize,
	Default_Cursor_Kind_NE_Resize,
	Default_Cursor_Kind_SE_Resize,
	Default_Cursor_Kind_SW_Resize,
	Default_Cursor_Kind_NW_Resize,
	Default_Cursor_Kind_COUNT,
};

typedef u64 Window_Swap_Mode;
enum Window_Swap_Mode {
	Fifo,
	Fifo_Relaxed,
	Mailbox,
	Immediate,
};

typedef u64 Window_Flags;
enum Window_Flag {
	Window_Flag_Maximized,
	Window_Flag_Fullscreen,
	Window_Flag_Resizing,

	Window_Flag_Activated,
	Window_Flag_Suspended,

	Window_Flag_Tiled_Left,
	Window_Flag_Tiled_Above,
	Window_Flag_Tiled_Right,
	Window_Flag_Tiled_Below,
};


typedef u64 Window_Event_Kind;
enum Window_Event_Kind {
	// Input events
	Window_Event_Mouse_Enter,
	Window_Event_Mouse_Leave,
	Window_Event_Mouse_Position,
	Window_Event_Mouse_Move,
	Window_Event_Mouse_Wheel,
	Window_Event_Mouse_Button,
	Window_Event_Keyboard_Scancode,
	Window_Event_Keyboard_Text,

	// Window events
	Window_Event_Resize,
};

typedef struct Window_Event Window_Event;
struct Window_Event {
	Window_Event_Kind kind;
	union {
	};
};
