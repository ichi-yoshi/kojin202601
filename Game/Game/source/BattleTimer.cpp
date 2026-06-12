#include "BattleTimer.h"
#include <unordered_map>

void BattleTimer::Initialize()
{
	_currentPhase = BattlePhase::Defense;
}

void BattleTimer::Update(double deltaTime)
{
	// 時間が0秒以上のときのみ減少させる
	if(_time > 0.0)
	{
		_time -= deltaTime;
		if(_time<0.0)
		{
			_time = 0.0;
		}
	}
}

void BattleTimer::IsClearCircle(const CharaAfterStatus& afterStatus)
{
	if(_currentPhase != BattlePhase::Defense) return;

	double digestTime = afterStatus.GetcoolTime();
	_time -= digestTime;

	// 0秒未満になったら0秒で固定する
	if(_time < 0.0)
	{
		_time = 0.0;
	}

}

void BattleTimer::ChangePhase(BattlePhase nextPhase)
{
	_currentPhase = nextPhase;

	// フェーズごとの初期時間を定義するマップ
	static const std::unordered_map<BattlePhase, double> phaseTimes = 
	{
		{ BattlePhase::Attack, 20.0 },
		{ BattlePhase::Defense, 40.0 },
		{ BattlePhase::Result, 5.0 }
	};

	auto it = phaseTimes.find(_currentPhase);

	if(it != phaseTimes.end())
	{
		_time = it->second; // 例：Attackなら 20.0 が _time に入る
	}
	else
	{
		_time = 0.0; // 万が一見つからなかった場合の安全対策
	}
}