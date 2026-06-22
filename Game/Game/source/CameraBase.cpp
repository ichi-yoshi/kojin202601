#include "CameraBase.h"
#include "ApplicationMain.h"

// 定数定義
const float CameraBase::FPS_CAMERA_HEIGHT = 100.0f;
const int CameraBase::SCREEN_CENTER_X = 1920 / 2;
const int CameraBase::SCREEN_CENTER_Y = 1080 / 2;

CameraBase::CameraBase()
{
	_vPos = VGet(0, 90.f, -300.f);
	_vTarget = VGet(0, 60, 0);
	_clipFar = 10000.f;
	_clipNear = 2.f;
	_prevMouseX = -1;
	_prevMouseY = -1;
	_camYaw = 0.0f;
	_camPitch = 0.0f;
	_mouseSensitivity = 0.005f;

}

CameraBase::~CameraBase()
{

}

bool CameraBase::Initialize()
{
    // カメラの初期設定
    _vPos = VGet(0, 90.f, -300.f);
    _vTarget = VGet(0, 60, 0);
    _clipNear = 2.f;
    _clipFar = 10000.f;

    // マウス関連の初期化
    _prevMouseX = -1;
    _prevMouseY = -1;
    _camYaw = 0.0f;
    _camPitch = 0.0f;

    SetMouseDispFlag(FALSE);
    return true;
}

bool CameraBase::Terminate()
{
    return true;
}

void CameraBase::Update(VECTOR playerPos, int& key)
{
    HandleMouseInput();
    UpdateFPSCamera(playerPos);
    ResetMouseToCenter();
}

void CameraBase::HandleMouseInput()
{
    // マウス移動量取得
    int mouseX, mouseY;
    GetMousePoint(&mouseX, &mouseY);

    // 初回時の前回位置設定
    if(_prevMouseX == -1)
    {
        _prevMouseX = mouseX;
        _prevMouseY = mouseY;
    }

    int deltaX = mouseX - _prevMouseX;
    int deltaY = mouseY - _prevMouseY;
    _prevMouseX = mouseX;
    _prevMouseY = mouseY;

    // カメラ回転(マウス移動で制御)FPS視点用
    _camYaw += deltaX * _mouseSensitivity;
    _camPitch -= deltaY * _mouseSensitivity;

    // ピッチ制限
    if(_camPitch < -1.2f) _camPitch = -1.2f;
    if(_camPitch > 1.2f) _camPitch = 1.2f;
}

void CameraBase::UpdateFPSCamera(VECTOR playerPos)
{
    // FPS視点のカメラ位置と向き
    _vPos = VAdd(playerPos, VGet(0, FPS_CAMERA_HEIGHT, 0)); // プレイヤーの頭部

    VECTOR forward = VGet(
        cos(_camPitch) * sin(_camYaw),
        sin(_camPitch),
        cos(_camPitch) * cos(_camYaw)
    );
    _vTarget = VAdd(_vPos, forward); // 視線方向
}

void CameraBase::ResetMouseToCenter()
{
    // 毎フレーム中央に戻す
    SetMousePoint(SCREEN_CENTER_X, SCREEN_CENTER_Y);
    _prevMouseX = SCREEN_CENTER_X;
    _prevMouseY = SCREEN_CENTER_Y;
}

void CameraBase::ApplyCamera()
{
    // カメラ設定更新
    SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);
    SetCameraNearFar(_clipNear, _clipFar);
}

VECTOR CameraBase::GetDirection() const
{
    return VGet(sin(_camYaw), 0, cos(_camYaw));
}
