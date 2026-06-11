#pragma once
#include "MouseInput.h"
#include "CharaAfterStatus.h"
#include "BattleTimer.h"
#include "CircleUI.h"
#include "GaugeUI.h"
#include <string>

class ModeGameBattle
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	void Process(MouseInput& mouse, CharaAfterStatus& afterStatus, double deltaTime);
	void Render();
	bool IsBattleEnd()const { return _isBattleEnd; }
	void Reset();
private:
	BattleTimer _battleTimer;
	CircleUI _circleUI;
	GaugeUI _gaugeUI;
	bool _isBattleEnd = false;
};

