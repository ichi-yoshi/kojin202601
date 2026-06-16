#pragma once
#include "MouseInput.h"
#include "CharaAfterStatus.h"
#include "BattleTimer.h"
#include "CircleUI.h"
#include "GaugeUI.h"
#include <string>
#include <functional>
#include <unordered_map>

class ModeGameBattle
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	void Process(MouseInput& mouse, CharaAfterStatus& afterStatus, double deltaTime);
	void Render();
	bool IsBattleEnd()const { return _isBattleEnd; }
	void Reset();
private:
	// 各フェーズの処理
	void UpdateDefense(MouseInput& mouse, CharaAfterStatus& afterStatus);
	void UpdateAttack(MouseInput& mouse, CharaAfterStatus& afterStatus);
	void UpdateStart(MouseInput& mouse, CharaAfterStatus& afterStatus);

	void SetPhase(BattleTimer::BattlePhase nextPhase);

	using PhaseFunc = std::function<void(MouseInput&, CharaAfterStatus&)>;

	PhaseFunc _phaseUpdateFunc = nullptr;
	BattleTimer _battleTimer;
	CircleUI _circleUI;
	GaugeUI _gaugeUI;

	double _resultWaitTime = 0.0;

	bool _isBattleEnd = false;
};

