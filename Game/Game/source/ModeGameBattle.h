#pragma once
#include "MouseInput.h"
#include "CharaAfterStatus.h"
#include "Enemy.h"          
#include "SaveData.h"       
#include "SqliteEnemy.h"    
#include "BattleTimer.h"
#include "CircleUI.h"
#include "GaugeUI.h"
#include "CharaFormula.h"
#include "SqliteCharaFormula.h"
#include "BattleUI.h"
#include "container.h"

class ModeGameBattle
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	void Process(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData, double deltaTime);
	void Render(CharaAfterStatus& afterStatus);
	bool IsBattleEnd()const { return _isBattleEnd; }
	void Reset(const SaveData& saveData, CharaAfterStatus& afterStatus);
private:
	// 各フェーズの処理
	void UpdateDefense(MouseInput& mouse, CharaAfterStatus& afterStatus);
	void UpdateAttack(MouseInput& mouse, CharaAfterStatus& afterStatus);
	void UpdateStart(MouseInput& mouse, CharaAfterStatus& afterStatus);
	void UpdateResult(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData);

	void ProcessBattleResult(SaveData& saveData);
	void SetPhase(BattleTimer::BattlePhase nextPhase);

	using PhaseFunc = std::function<void(MouseInput&, CharaAfterStatus&)>;

	PhaseFunc _phaseUpdateFunc = nullptr;
	BattleTimer _battleTimer;
	CircleUI _circleUI;
	GaugeUI _gaugeUI;
	BattleUI _battleUI;

	bool _isBattleEnd = false;

	SqliteEnemy _sqliteEnemy;
	SaveData saveData;
	Enemy* _enemy;             

	CharaFormula _charaFormula;
	SqliteCharaFormula _SqliteCharaFormula;

	double _enemyCurrentHP;     // 敵の現在のHP

	std::vector<double> _damageHistory;

	std::string _logCriticalExpr;
	std::string _logDefenseExpr;
	std::string _logFinalExpr;
	double _logFinalDamage = 0.0;

	std::string _debugFormulaRaw;				// DBから読み込んだ生の計算式
	std::string _debugFormulaReplaced;			// 数値が代入された状態の計算式
	double _debugLastCalculatedDamage = 0.0;	// 最後に計算されたダメージ

	double _charaCurrentHP;
	bool _isHPInitialized ;
	double _nextDamageTime;
	double _damageFlashTimer;

	bool _isResultProcessed = false;     // リザルト処理が完了したかどうかのフラグ
	double _maxDamageDealt = 0.0;
};

