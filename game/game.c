
proc void
render_weird_gradient(Input *input, Backbuffer backbuffer, f32 dt) {
	if (backbuffer.memory == nil) { return; }

	local_persist f32 offset = 0.f;
	offset += 256.f*dt;

	local_persist vec2f32 delta_accum = {0.f, 0.f};
	Input_Keyboard *keyboard = &input->keyboard;
	Input_Mouse *mouse = &input->mouse;
	Input_Gamepad *gamepad_one = &input->gamepads[0];
	Input_Gamepad *gamepad_two = &input->gamepads[1];
	delta_accum = add_2f32(delta_accum, scale_2f32(mouse->delta, 1.f));

	u8 *row = cast(u8*)(backbuffer.memory);
	for each_index(y, backbuffer.size.y) {
		for each_index(x, backbuffer.size.x) {
			u32 *pixel = cast(u32*)(row) + x;

			u8 red = 0;
			u8 green = 0;
			u8 blue = 0;

			if (!btn_down(keyboard->btns[57])) {
				blue = cast(u8)(y);
				green = cast(u8)(x + cast(s64)(offset));
			} else {
				green = cast(u8)(y);
				blue = cast(u8)(x + cast(s64)(offset));
			}

			if (btn_down(mouse->btns[Mouse_Button_Left])) {
				blue = cast(u8)(y + cast(s64)(delta_accum.y));
				green = cast(u8)(x + cast(s64)(delta_accum.x));
			}
			if (btn_down(gamepad_one->btns[Gamepad_Button_A])) {
				blue = cast(u8)(y + cast(s64)(delta_accum.y));
				green = cast(u8)(x + cast(s64)(delta_accum.x));
			}
			if (btn_down(gamepad_two->btns[Gamepad_Button_Y])) {
				blue = cast(u8)(y + cast(s64)(delta_accum.y));
				green = cast(u8)(x + cast(s64)(delta_accum.x));
			}

			u32 color = ((u32)red<<16) | ((u32)green<<8) | ((u32)blue<<0);
			*pixel = color;
		}
		row += backbuffer.stride;
	}
}

proc void
draw_rectangle(Backbuffer backbuffer, rng2f32 rect, vec4f32 color) {
	assert(backbuffer.size.x >= 0 && backbuffer.size.y >= 0);
	if (backbuffer.memory == nil) { return; }

	vec2s32 size = backbuffer.size;
	s32 minx = cast(s32)(rect.min.x);
	s32 miny = cast(s32)(rect.min.y);
	s32 maxx = cast(s32)(rect.max.x);
	s32 maxy = cast(s32)(rect.max.y);

	minx = clamp_bot(minx, 0);
	miny = clamp_bot(miny, 0);
	maxx = clamp_top(maxx, size.x);
	maxy = clamp_top(maxy, size.y);

	u8 *row = cast(u8*)backbuffer.memory + (miny*backbuffer.stride) + minx*size_of(u32);
	for (s32 y = miny; y < maxy; y += 1) {
		for (s32 x = minx; x < maxx; x += 1) {
			u32 *pixel = (u32*)row + x;
			*pixel = u32_from_rgba(color);
		}
		row += backbuffer.stride;
	}
}

proc void
game_update_and_render(Input *input, Backbuffer backbuffer, f32 dt) {

	if (backbuffer.memory != nil) {
		assert(backbuffer.size.x > 0 && backbuffer.size.y > 0);
		u64 backbuffer_total_size = cast(u64)backbuffer.size.y*cast(u64)backbuffer.stride;
		memory_zero(backbuffer.memory, backbuffer_total_size);
	}

	{
		local_persist vec2f32 player_pos = {};
		Input_Gamepad *gamepad = input->gamepads + 0;

		Input_Axis xaxis = gamepad->axes[Gamepad_Axis_X];
		f32 xaxis_dim = dim_1f32(xaxis.range);
		f32 x_value = (xaxis.value / xaxis_dim)*2.f - 1.f;
		Input_Axis yaxis = gamepad->axes[Gamepad_Axis_Y];
		f32 yaxis_dim = dim_1f32(yaxis.range);
		f32 y_value = (yaxis.value / yaxis_dim)*2.f - 1.f;

		Input_Axis speed_axis = gamepad->axes[Gamepad_Axis_RY];
		f32 speed_value = ((dim_1f32(speed_axis.range) - speed_axis.value) / dim_1f32(speed_axis.range));
		f32 speed = 100.f*speed_value;

		// infof("[%.6f, %.6f] / [%.6f, %.6f] --> [%.6f, %.6f]", (f64)xaxis.value, (f64)yaxis.value, (f64)xaxis_dim, (f64)yaxis_dim, (f64)x_value, (f64)y_value);
		player_pos.x += x_value*speed;
		player_pos.y += y_value*speed;

		vec2f32 player_size = v2f32(10.f, 10.f);
		rng2f32 rect = r2f32(player_pos, add_2f32(player_pos, player_size));

		Input_Axis color_axis = gamepad->axes[Gamepad_Axis_RZ];
		f32 color_t = (color_axis.value / dim_1f32(color_axis.range));
		vec4f32 color = mix_4f32(v4f32(1.f, 1.f, 1.f, 1.f), v4f32(0.f, 0.f, 0.f, 1.f), color_t);
		draw_rectangle(backbuffer, rect, color);
	}

	{
		local_persist vec2f32 player_pos = {};
		Input_Gamepad *gamepad = input->gamepads + 1;

		Input_Axis xaxis = gamepad->axes[Gamepad_Axis_X];
		f32 xaxis_dim = dim_1f32(xaxis.range);
		f32 x_value = (xaxis.value / xaxis_dim)*2.f - 1.f;
		Input_Axis yaxis = gamepad->axes[Gamepad_Axis_Y];
		f32 yaxis_dim = dim_1f32(yaxis.range);
		f32 y_value = (yaxis.value / yaxis_dim)*2.f - 1.f;

		Input_Axis speed_axis = gamepad->axes[Gamepad_Axis_RY];
		f32 speed_value = ((dim_1f32(speed_axis.range) - speed_axis.value) / dim_1f32(speed_axis.range));
		f32 speed = 100.f*speed_value;

		// infof("[%.6f, %.6f] / [%.6f, %.6f] --> [%.6f, %.6f]", (f64)xaxis.value, (f64)yaxis.value, (f64)xaxis_dim, (f64)yaxis_dim, (f64)x_value, (f64)y_value);
		player_pos.x += x_value*speed;
		player_pos.y += y_value*speed;

		vec2f32 player_size = v2f32(10.f, 10.f);
		rng2f32 rect = r2f32(player_pos, add_2f32(player_pos, player_size));

		Input_Axis color_axis = gamepad->axes[Gamepad_Axis_RZ];
		f32 color_t = (color_axis.value / dim_1f32(color_axis.range));
		vec4f32 color = mix_4f32(v4f32(1.f, 0.f, 1.f, 1.f), v4f32(0.f, 0.f, 0.f, 1.f), color_t);
		draw_rectangle(backbuffer, rect, color);
	}
}

