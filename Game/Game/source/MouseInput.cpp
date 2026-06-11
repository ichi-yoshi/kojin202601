#include "AppFrame.h"
#include "MouseInput.h"

// マウスポインタの位置とホイールの回転量、左クリックの状態を更新する
void MouseInput::Update()
{
	GetMousePoint(&_x, &_y);
	_wheel = GetMouseWheelRotVol();

	_leftDown = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
	_leftTrig = _leftDown && !_prevLeftDown;
	_prevLeftDown = _leftDown;
}

// 指定した矩形内にマウスポインタがあるか
bool MouseInput::IsInRect(int x, int y, int w, int h) const
{
	return _x >= x && _x <= x + w &&
		_y >= y && _y <= y + h;
}