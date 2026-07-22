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
	// 初期化
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	// 更新
	void Process(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData, float deltaTime);

	// 描画
	void Render(CharaAfterStatus& afterStatus);

	// バトルが終了したかどうかを取得する
	bool IsBattleEnd()const { return _isBattleEnd; }

	// バトルの状態をリセットする
	void Reset(const SaveData& saveData, CharaAfterStatus& afterStatus);

private:
	// 各フェーズの処理
	void UpdateDefense(MouseInput& mouse, CharaAfterStatus& afterStatus);					// 防御フェーズの更新
	void UpdateAttack(MouseInput& mouse, CharaAfterStatus& afterStatus);					// 攻撃フェーズの更新
	void UpdateStart(MouseInput& mouse, CharaAfterStatus& afterStatus);						// 戦闘開始フェーズの更新
	void UpdateResult(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData);// リザルトフェーズの更新

	void ProcessBattleResult(SaveData& saveData);		// バトル結果の処理
	void SetPhase(BattleTimer::BattlePhase nextPhase);	// フェーズの切り替え

	using PhaseFunc = std::function<void(MouseInput&, CharaAfterStatus&)>;	// フェーズごとの処理関数の型

	std::vector<double> _damageHistory;	// ダメージ履歴
	std::string _logCriticalExpr;		// クリティカル計算式
	std::string _logDefenseExpr;		// 防御計算式
	std::string _logFinalExpr;			// 最終ダメージ計算式
	double _logFinalDamage = 0.0;		// 最終ダメージ

	std::string _debugFormulaRaw;				// DBから読み込んだ生の計算式
	std::string _debugFormulaReplaced;			// 数値が代入された状態の計算式
	double _debugLastCalculatedDamage = 0.0;	// 最後に計算されたダメージ

	double _charaCurrentHP;		// プレイヤーの現在のHP
	bool _isHPInitialized;		// プレイヤーのHPが初期化済みかどうかのフラグ
	double _nextDamageTime;		// 次のダメージを受けるまでの時間
	double _damageFlashTimer;	// ダメージを受けたときの点滅タイマー
	bool _isBattleEnd = false;	// バトルが終了したかどうかのフラグ
	double _enemyCurrentHP;     // 敵の現在のHP
	bool _isResultProcessed = false;    // リザルト処理が完了したかどうかのフラグ
	double _maxDamageDealt = 0.0;		// 最大ダメージ

private:
	PhaseFunc _phaseUpdateFunc = nullptr;
	BattleTimer _battleTimer;
	CircleUI _circleUI;
	GaugeUI _gaugeUI;
	BattleUI _battleUI;
	SqliteEnemy _sqliteEnemy;
	SaveData saveData;
	Enemy* _enemy;
	CharaFormula _charaFormula;
	SqliteCharaFormula _SqliteCharaFormula;
};

