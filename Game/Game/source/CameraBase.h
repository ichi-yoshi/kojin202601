#pragma once
#include "container.h"

class CameraBase {
public:
    CameraBase();
    ~CameraBase();

    // 初期化・終了処理
    bool Initialize();
    bool Terminate();

    // 更新処理
    void Update(VECTOR playerPos, int& key);

    // カメラ設定の適用
    void ApplyCamera();

    // アクセサ
    VECTOR GetPosition() const { return _vPos; }
    VECTOR GetTarget() const { return _vTarget; }
    VECTOR GetDirection() const;
    float GetYaw() const { return _camYaw; }
    float GetPitch() const { return _camPitch; }

    // 設定
    void SetPosition(VECTOR pos) { _vPos = pos; }
    void SetTarget(VECTOR target) { _vTarget = target; }
    void SetClip(float nearClip, float farClip) { _clipNear = nearClip; _clipFar = farClip; }

private:
    // カメラパラメータ
    VECTOR _vPos;                   // 位置
    VECTOR _vTarget;                // 注視点
    float _clipNear, _clipFar;      // クリップ距離

    // マウス制御用
    int _prevMouseX, _prevMouseY;   // 前回のマウス位置
    float _camYaw, _camPitch;       // カメラの角度
    float _mouseSensitivity;        // マウス感度

    // FPS視点用パラメータ
    static const float FPS_CAMERA_HEIGHT;  // カメラの高さ
    static const int SCREEN_CENTER_X;       // 画面中央X座標
    static const int SCREEN_CENTER_Y;       // 画面中央Y座標

    // 内部処理
    void HandleMouseInput();
    void UpdateFPSCamera(VECTOR playerPos);
    void ResetMouseToCenter();
};

