#include "GaugeUI.h"
#include "ButtonUI.h"
#include "MagicNumberConfig.h"

bool GaugeUI::Initialize(const std::string& dbPath, std::string* outError)
{
	// ゲージのマスターデータをロード
	if(!_sqliteGauge.Initialize(dbPath, outError))
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
	/**/_speed = speed / 1000;	// 速度をミリ秒単位から秒単位に変換
	
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
	_prevKeyF = (CheckHitKey(KEY_INPUT_F) == 1);

	// SQLiteからランダムに1件抽選して適用する
	if(_sqliteGauge.RollrandomGauge())
	{
		GaugeRow row;
		_sqliteGauge.GetGauge(row);
		SetPattern(row.targetX, row.targetW, row.speed);
	}
}

void GaugeUI::Update(MouseInput& mouse)
{
	if(!_isMoving) { return; }

	bool currentKeyF = (CheckHitKey(KEY_INPUT_F) == 1);
	bool isFTrigger = (currentKeyF && !_prevKeyF);

	// ゲージの値を更新
	_currentValue += _speed;

	// 1.0を超えたらリセットしてループさせる
	/**/if(_currentValue>1.0)
	{
		_currentValue = 0.0;
	}

	// ゲージの矩形範囲が左クリックされた時の判定
	if(mouse.IsLeftTrig() && mouse.IsInRect(_gaugeButton.x, _gaugeButton.y, _gaugeButton.w, _gaugeButton.h)|| isFTrigger)
	{
		_isMoving = false; // バーの動きをピタッと止める
		_hasResult = true;

		// 現在のバーの「相対ピクセルX座標」を計算
		int currentX = static_cast<int>(_gaugeButton.w * _currentValue);

		// バーのX座標が、ターゲットの赤枠範囲内（左端 ～ 右端）に収まっているか判定
		if(currentX >= _targetX && currentX <= (_targetX + _targetW))
		{
			_isSuccess = true;  
		}
		else
		{
			_isSuccess = false; 
		}
	}
	_prevKeyF = currentKeyF;	// 前回のFキーの状態を更新
}

void GaugeUI::Draw() 
{
	// 名前空間の使用の宣言
	using namespace UIConfig;

	// 基準となるゲージの座標とサイズを取得
	int bx = _gaugeButton.x;
	int by = _gaugeButton.y;
	int bw = _gaugeButton.w;
	int bh = _gaugeButton.h;
	DrawBox(bx, by, bx + bw, by + bh, Color::White(), false);

	// 赤枠の座標とサイズを計算して描画
	int targetLeft = bx + _targetX;
	int targetRight = targetLeft + _targetW;
	DrawBox(targetLeft, by, targetRight, by + bh, Color::Red(), true);

	// バーを描画 (太さオフセット適用)
	int barX = bx + static_cast<int>(bw * _currentValue);
	DrawBox(barX - Common::GaugeBarThickness,
		by - Common::GaugeBarOverflow,
		barX + Common::GaugeBarThickness,
		by + bh + Common::GaugeBarOverflow,
		Color::White(), TRUE);

	// 結果がある場合は成功・失敗の文字列を描画
	if(_hasResult) 
	{
		int textY = by - Common::TextMarginTop;
		if (_isSuccess) 
		{
			DrawString(bx, textY, "成功", Color::Green());
		}
		else 
		{
			DrawString(bx, textY, "失敗", Color::Red());
		}
	}
}

bool GaugeUI::CheckHit() 
{
	if(_isMoving) { return _isSuccess; }

	_isMoving = false;
	_hasResult = true;

	// 現在のバーの「相対ピクセルX座標」を計算
	int currentLocalX = static_cast<int>(_currentValue * _gaugeButton.w);

	// バーのX座標が、ターゲットの赤枠範囲内（左端 ～ 右端）に収まっているか判定
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