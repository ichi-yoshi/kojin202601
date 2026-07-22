#pragma once

class MouseInput
{
public:
	MouseInput();
	~MouseInput() = default;

	void Update();

	// マウスを取得
	int GetX() const { return _x; }			// マウスポインタのX座標を取得
	int GetY() const { return _y; }			// マウスポインタのY座標を取得
	int GetWheel() const { return _wheel; }	// ホイールの回転量を取得

	// 左クリックの状態を取得
	bool IsLeftDown() const { return _leftDown; }	// 左クリックが押されているか
	bool IsLeftTrig() const { return _leftTrig; }	// 左クリックがトリガーされたか

	// 指定した矩形内にマウスポインタがあるか
	bool IsInRect(int x, int y, int w, int h) const;

	// マウスポインタの移動量を取得し、YawとPitchを更新する
	void MousePointerControl();

	// アクセサ
	float GetMouseSensitivity() const { return _mouseSensitivity; }	// マウス感度を取得
	float GetYaw() const { return _Yaw; }							// マウスの水平移動量を取得
	float GetPitch() const { return _Pitch; }						// マウスの垂直移動量を取得

	// マウスポインタを画面中央に戻す
	void ResetMousePointCenter();

private:
	// マウス制御用
	int _x;		// マウスポインタのX座標
	int _y;		// マウスポインタのY座標
	int _wheel;	// ホイールの回転量

	bool _leftDown;		// 左クリックの状態
	bool _leftTrig;		// 左クリックのトリガー
	bool _prevLeftDown;	// 前回の左クリック状態

	int _prevMouseX;			// 前回のマウスX座標
	int _prevMouseY;			// 前回のマウスY座標
	float _mouseSensitivity;	// マウス感度
	float _Yaw;					// マウスの水平移動量
	float _Pitch;				// マウスの垂直移動量
};

