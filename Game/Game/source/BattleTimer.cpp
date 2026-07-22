#include "BattleTimer.h"
#include "container.h"

void BattleTimer::Update(double deltaTime)
{
	// 時間が0秒以上のときのみ減少させる
	if(_time > 0.0)
	{
		_time -= deltaTime;

		// 0秒未満になったら0秒で固定する
		if(_time<0.0)
		{
			_time = 0.0;
		}
	}
}

void BattleTimer::IsClearCircle(const CharaAfterStatus& afterStatus)
{
	if(_currentPhase != BattlePhase::Defense) return;

	// 円を消した時、残り時間を減少させる
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
		{ BattlePhase::Attack, 10.0 },	// 攻撃フェーズ
		{ BattlePhase::Defense, 40.0 },	// 防御フェーズ
		{ BattlePhase::Result, 5.0 },	// 結果フェーズ
		{ BattlePhase::Start, 5.0 }		// 戦闘開始フェーズ
	};

	// 現在のフェーズに対応する初期時間を取得する
	auto it = phaseTimes.find(_currentPhase);

	// フェーズが見つかった場合はその時間を設定し、見つからなかった場合は0秒に設定する
	if(it != phaseTimes.end())
	{
		_time = it->second; 
	}
	else
	{
		_time = 0.0; 
	}
}