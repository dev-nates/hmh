
typedef struct Linux_Gamepad_Data Linux_Gamepad_Data;
struct Linux_Gamepad_Data {
	int device_number;	
	s16 rumbe_effect_id;
	int fd;
};
static_assert(size_of(Linux_Gamepad_Data) <= IGAMEPAD_OPAQUE_SIZE, check_linux_gamepad_data_size);

global int gamepad_notify = 0;
