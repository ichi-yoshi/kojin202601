#include "BattleTimer.h"

void BattleTimer::Initialize()
{
	_time = 40.0;
	_currentPhase = BattlePhase::Defense;
}

void BattleTimer::Update(double deltaTime)
{
	if(_currentPhase == BattlePhase::Defense)
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
	if(_currentPhase == BattlePhase::Defense)
	{
		_time = 40.0;
	}
}