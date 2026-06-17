#pragma once
#include "MouseInput.h"
#include "CharaAfterStatus.h"
#include "Enemy.h"          
#include "SaveData.h"       
#include "SqliteEnemy.h"    
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
	void Process(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData, double deltaTime);
	void Render();
	bool IsBattleEnd()const { return _isBattleEnd; }
	void Reset(const SaveData& saveData);
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

	SqliteEnemy _sqliteEnemy;
	SaveData saveData;
	Enemy* _enemy;              // 動的に生成する敵のポインタ
	double _enemyCurrentHP;     // 敵の現在のHP
	bool _isEnemyDefeated;      // 敵を倒したかどうかのフラグ
};

