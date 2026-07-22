#pragma once
#include "ButtonUI.h"
#include "MouseInput.h"
#include "SqliteGauge.h"
#include "container.h"

class GaugeUI
{
public:
	// ゲージのパターンを設定
	void SetPattern(int targetX,int targetW,double speed);

	// 初期化
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	// 更新
	void Update(MouseInput& mouse);

	// 判定
	bool CheckHit();

	// 描画
	void Draw();

	// ゲージのボタンの矩形を設定
	void SetPos(int x, int y, int w, int h);

	// 結果をリセット
	void Reset();
	
	// 結果を取得
	bool IsSuccess() const { return _isSuccess; };
	bool HasResult() const { return _hasResult; };

private:
	// ゲージのボタンの矩形を取得
	const ButtonUI::ButtonRect& GetGaugeButtonRect() const { return _gaugeButton; }
	ButtonUI::ButtonRect _gaugeButton{500,600,400,100};
	SqliteGauge _sqliteGauge;

private:
	int _targetX;			// 赤枠の左端X座標
	int _targetW;			// 赤枠の幅
	double _speed;			// バーの移動速度
	double _currentValue;	// バーの現在位置
	double _maxValue;		// バーの最大値
	double _minValue;		// バーの最小値
	bool _isMoving;			// バーが動いているかどうか
	bool _isSuccess;		// 成功かどうか
	bool _hasResult;		// 結果があるかどうか
	bool _prevKeyF = false;	// 前回のFキーの状態
};

