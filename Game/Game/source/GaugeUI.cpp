#include "GaugeUI.h"
#include "ButtonUI.h"
#include "DxLib.h"

const ButtonUI::ButtonRect& GaugeUI::GetGaugeButtonRect() const
{
	return _gaugeButton;
}

bool GaugeUI::Initialize(const std::string& dbPath, std::string* outError)
{
	// ゲージのマスターデータをロード
	if (!_sqliteGauge.Initialize(dbPath, outError))
	{
		return false;
	}

	// 最初のゲージをロードしてリセット
	Reset();
	return true;
}

void GaugeUI::SetPattern(int targetX, int targetW, double speed)
{
	_targetX = targetX;
	_targetW = targetW;
	_speed = speed/1000;
	
}

void GaugeUI::SetPos(int x, int y, int w, int h)
{
	_gaugeButton.x = x;
	_gaugeButton.y = y;
	_gaugeButton.w = w;
	_gaugeButton.h = h;
}

void GaugeUI::Reset() 
{
	_currentValue = 0.0;
	_isMoving = true;
	_isSuccess = false;
	_hasResult = false;

	// SQLiteからランダムに1件抽選して適用する
	if (_sqliteGauge.RollrandomGauge())
	{
		GaugeRow row;
		_sqliteGauge.GetGauge(row);
		SetPattern(row.targetX, row.targetW, row.speed);
	}
}

void GaugeUI::Update(MouseInput& mouse)
{
	if(!_isMoving) { return; }

	// ゲージの値を更新
	_currentValue += _speed;

	// 1.0を超えたらリセットしてループさせる
	if(_currentValue>1.0)
	{
		_currentValue = 0.0;
	}

	// 2. ゲージの矩形範囲が左クリックされた時の判定
	if (mouse.IsLeftTrig() && mouse.IsInRect(_gaugeButton.x, _gaugeButton.y, _gaugeButton.w, _gaugeButton.h))
	{
		_isMoving = false; // バーの動きをピタッと止める
		_hasResult = true;

		// 現在のバーの「相対ピクセルX座標」を計算
		int currentX = static_cast<int>(_gaugeButton.w * _currentValue);

		// バーのX座標が、ターゲットの赤枠範囲内（左端 ～ 右端）に収まっているか判定
		if (currentX >= _targetX && currentX <= (_targetX + _targetW))
		{
			_isSuccess = true;  
		}
		else
		{
			_isSuccess = false; 
		}
	}
}

void GaugeUI::Draw() 
{
	// 基準となるゲージの座標とサイズを取得
	int bx = _gaugeButton.x;
	int by = _gaugeButton.y;
	int bw = _gaugeButton.w;
	int bh = _gaugeButton.h;

	DrawBox(bx, by, bx + bw, by + bh, GetColor(255, 255, 255), false);

	int targetLeft = bx + _targetX;
	int targetRight = targetLeft + _targetW;
	DrawBox(targetLeft, by, targetRight, by + bh, GetColor(255,120,120), true);

	int barX = bx + static_cast<int>(bw * _currentValue);
	DrawBox(barX - 1, by - 2, barX + 2, by + bh + 2, GetColor(255, 255, 255), true);

	if (_hasResult) 
	{
		if (_isSuccess) 
		{
			DrawString(bx,by-20, "Success!", GetColor(0, 255, 0));
		}
		else 
		{
			DrawString(bx,by-20, "Failed!", GetColor(255, 0, 0));
		}
	}
}

bool GaugeUI::CheckHit() 
{
	if(_isMoving) { return _isSuccess; }

	_isMoving = false;
	_hasResult = true;

	int currentLocalX = static_cast<int>(_currentValue * _gaugeButton.w);

	if(currentLocalX >= _targetX && currentLocalX < _targetX + _targetW)
	{
		_isSuccess = true;
	}
	else
	{
		_isSuccess = false;
	}

	return _isSuccess;
}