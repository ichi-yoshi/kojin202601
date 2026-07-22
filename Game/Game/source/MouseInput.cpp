#include "AppFrame.h"
#include "MouseInput.h"
#include "MagicNumberConfig.h"

// 名前空間の使用の宣言
using namespace UIConfig;

MouseInput::MouseInput() 
{
	_x = 0;                     
	_y = 0;                     
	_wheel = 0;                 
	_leftDown = false;          
	_leftTrig = false;          
	_prevLeftDown = false;      
	_prevMouseX = -1;           
	_prevMouseY = -1;           
	_mouseSensitivity = 0.005f; 
	_Yaw = 0.0f;                
	_Pitch = 0.0f;    
}

// マウスポインタの位置とホイールの回転量、左クリックの状態を更新する
void MouseInput::Update()
{
	GetMousePoint(&_x, &_y);        // マウスポインタの位置を取得
	_wheel = GetMouseWheelRotVol(); // ホイールの回転量を取得
	_leftDown = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;  // 左クリックの状態を取得
	_leftTrig = _leftDown && !_prevLeftDown;                // 左クリックのトリガーを判定
	_prevLeftDown = _leftDown;                              // 前回の左クリック状態を更新
}

// 指定した矩形内にマウスポインタがあるか
bool MouseInput::IsInRect(int x, int y, int w, int h) const
{
	return _x >= x && _x <= x + w &&
		_y >= y && _y <= y + h;
}

// マウスポインタの移動量を取得し、YawとPitchを更新する
void MouseInput::MousePointerControl()
{
    // マウス移動量取得
    GetMousePoint(&_x, &_y);

    // 初回時の前回位置設定
    if(_prevMouseX == -1)
    {
        _prevMouseX = _x;
        _prevMouseY = _y;
    }

	// マウスの移動量を計算
	int deltaX = _x - _prevMouseX;	// X座標の移動量
	int deltaY = _y - _prevMouseY;	// Y座標の移動量

	// 前回のマウス位置を更新
	_prevMouseX = _x;	// X座標
	_prevMouseY = _y;	// Y座標

	// マウス感度を考慮してYawとPitchを更新
    _Yaw = deltaX * _mouseSensitivity;
    _Pitch = deltaY * _mouseSensitivity;
}

// マウスポインタを画面中央に戻す
void MouseInput::ResetMousePointCenter()
{
	int centerX = Layout::Screen.w / 2;
	int centerY = Layout::Screen.h / 2;

	// 毎フレーム中央に戻す
	SetMousePoint(centerX, centerY);
	_prevMouseX = centerX;
	_prevMouseY = centerY;
}