#pragma once
#include <string>
#include <vector>
#include "SqliteCharaFormula.h"
#include "CharaAfterStatus.h"
#include "Enemy.h"

class CharaFormula
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	double CalculateFinalDamage(const CharaAfterStatus& player, const Enemy& enemy, bool isGaugeSuccess);
	double CalculateEnemyDamage(const CharaAfterStatus& afterstatus, const Enemy& enemy);
	std::string ReplaceVar(std::string sourceStr, const std::string& status, double value);
	// 各種計算式の評価関数
	double GetDefenseMultiplier(const CharaAfterStatus& afterstatus, const Enemy& enemy);
	double GetDecayRate(const CharaAfterStatus& afterstatus);
	double GetLiveCriticalMultiplier(const CharaAfterStatus& afterstatus);
	double GetLiveLuckMultiplier(const CharaAfterStatus& afterstatus);
	void SetFinalDamage(double damage);
	
private:
	SqliteCharaFormula _charaFormula;

	double _evaluatedDamage = 0.0;
};

