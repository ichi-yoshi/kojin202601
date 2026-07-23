#pragma once
#include "CharaAfterStatus.h"

class BattleTimer
{
public:
	enum class BattlePhase 
	{
		Attack,		// 攻撃フェーズ
		Defense,	// 防御フェーズ
		Result,		// 結果フェーズ
		Start,		// 戦闘開始フェーズ
		_EOT_		
	};

	void Update(double deltaTime);

	// 攻撃フェーズかどうかを判定する
	void IsClearCircle(const CharaAfterStatus& afterStatus);

	// 残り時間を取得する
	double GetTime() const { return _time; }

	// 現在のフェーズを取得する
	BattlePhase GetCurrentPhase() const { return _currentPhase; }

	// フェーズを変更する
	void ChangePhase(BattlePhase nextPhase);

	bool IsTimeUp() const { return _time <= _minTime; }

private:
	float _time;			// 現在の残り時間
	float _minTime=0.0f;	// 最小時間

private:
	BattlePhase _currentPhase;		// 現在の戦闘フェーズ
};

