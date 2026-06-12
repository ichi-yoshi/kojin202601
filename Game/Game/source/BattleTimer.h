#pragma once
#include "CharaAfterStatus.h"

class BattleTimer
{
public:
	enum class BattlePhase 
	{
		Attack,
		Defense,
		Result,
		_EOT_
	};

	void Initialize();
	void Update(double deltaTime);

	void IsClearCircle(const CharaAfterStatus& afterStatus);
	double GetTime() const { return _time; }
	BattlePhase GetCurrentPhase() const { return _currentPhase; }

	void ChangePhase(BattlePhase nextPhase);

	bool IsTimeUp() const { return _time <= 0.0; }
private:
	double _time;					// 現在の残り時間
	BattlePhase _currentPhase;		// 現在の戦闘フェーズ
};

