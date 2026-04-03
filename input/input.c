
//--------------------------------------------------------------------------------
// input buttons

proc b8
btn_pressed(Input_Button btn) {
	b8 in_order = (btn.down == (btn.htc&1));
	b8 result = (in_order) ? (btn.htc >= 1) : (btn.htc >= 2);
	return result;
}

proc b8
btn_released(Input_Button btn) {
	b8 in_order = (btn.down == (btn.htc&1));
	b8 result = (in_order) ? (btn.htc >= 2) : (btn.htc >= 1);
	return result;
}

proc b8
btn_down(Input_Button btn) {
	return btn.down;
}

// -------------------------------------------------------------------------------------------------
// Mouse

proc void
input_clear_mouse(Input_Mouse *mouse) {
	mouse->wheel = 0;
	mouse->delta = v2f32(0.f,0.f);
	for (s32 i = 0; i < array_count(mouse->btns); i += 1) {
		Input_Button *btn = mouse->btns + i;
		btn->htc = 0;
	}
}

// -------------------------------------------------------------------------------------------------
// Keyboard

proc void
input_clear_keyboard(Input_Keyboard *keyboard) {
	for (s32 i = 0; i < array_count(keyboard->btns); i += 1) {
		Input_Button *btn = keyboard->btns + i;
		btn->htc = 0;
	}
}

// -------------------------------------------------------------------------------------------------
// Gamepad

proc void
input_clear_gamepad(Input_Gamepad *gamepad) {
	for each_element(i, gamepad->btns) {
		Input_Button *btn = gamepad->btns + i;
		btn->htc = 0;
	}
}
