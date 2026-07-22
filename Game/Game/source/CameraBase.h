#pragma once
#include "container.h"
#include "MouseInput.h"

class CameraBase 
{
public:
    CameraBase();
    ~CameraBase() = default;

    // 初期化・終了処理
    bool Initialize();
    bool Terminate();

    // 更新処理
    void Update(VECTOR playerPos, int& key);

    // カメラ設定の適用
    void ApplyCamera();

    // アクセサ
	VECTOR GetPosition() const { return _vPos; }    // カメラの位置を取得
	VECTOR GetTarget() const { return _vTarget; }   // カメラの注視点を取得
	VECTOR GetDirection() const;                    // カメラの向きを取得
	float GetYaw() const { return _camYaw; }        // カメラのヨー角を取得
	float GetPitch() const { return _camPitch; }    // カメラのピッチ角を取得

    // 設定
	void SetPosition(VECTOR pos) { _vPos = pos; }           // カメラの位置を設定
	void SetTarget(VECTOR target) { _vTarget = target; }    // カメラの注視点を設定
	void SetClip(float nearClip, float farClip) { _clipNear = nearClip; _clipFar = farClip; }   // クリップ距離を設定

private:
    VECTOR _vPos;     // 位置
    VECTOR _vTarget;  // 注視点
	float _clipNear;  // 近クリップ距離
	float _clipFar;   // 遠クリップ距離
	float _camYaw;    // カメラのヨー角
	float _camPitch;  // カメラのピッチ角

    // FPS視点用パラメータ
	const float FPS_CAMERA_HEIGHT = 100.0f; // プレイヤーの頭部の高さ

private:
	// マウス入力処理
    void HandleMouseInput();

	// FPS視点カメラの更新
    void UpdateFPSCamera(VECTOR playerPos);

private:
    MouseInput _mouse;
};

