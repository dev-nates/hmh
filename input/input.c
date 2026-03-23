
//--------------------------------------------------------------------------------
// input buttons

proc b8
ibtn_pressed(IButton button)
{
	b8 in_order = (button.down == (button.htc&1));
	b8 result = (in_order) ? (button.htc >= 1) : (button.htc >= 2);
	return result;
}

proc b8
ibtn_released(IButton button)
{
	b8 in_order = (button.down == (button.htc&1));
	b8 result = (in_order) ? (button.htc >= 2) : (button.htc >= 1);
	return result;
}

proc b8
ibtn_down(IButton button)
{
	return button.down;
}

