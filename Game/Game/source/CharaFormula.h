#pragma once
#include "SqliteCharaFormula.h"
#include "CharaAfterStatus.h"
#include "Enemy.h"
#include "SaveData.h"
#include "container.h"

class CharaFormula
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	// ダメージ計算関数
	double CalculateFinalDamage(const CharaAfterStatus& player, const Enemy& enemy, bool isGaugeSuccess);	// 最終ダメージを計算する関数
	double CalculateEnemyDamage(const CharaAfterStatus& afterstatus, const Enemy& enemy);					// 敵のダメージを計算する関数

	// 文字列中の変数を置換する関数
	std::string ReplaceVar(std::string sourceStr, const std::string& status, double value);

	// 各種計算式の評価関数
	double GetDefenseMultiplier(const CharaAfterStatus& afterstatus, const Enemy& enemy);	// 防御倍率
	double GetDecayRate(const CharaAfterStatus& afterstatus);								// ダメージ減衰率
	double GetLiveCriticalMultiplier(const CharaAfterStatus& afterstatus);					// 会心倍率
	double GetLiveLuckMultiplier(const CharaAfterStatus& afterstatus);						// 運値倍率
	void SetFinalDamage(double damage);														// 最終ダメージを設定
	
private:
	SqliteCharaFormula _charaFormula;
	SaveData _saveData;

private:
	double _evaluatedDamage = 0.0;	// 最終ダメージの評価値
};

