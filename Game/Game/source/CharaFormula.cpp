#include "CharaFormula.h"
#include "EvaluateFormula.h"
#include "SqliteCharaFormula.h"
#include "SaveData.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <iomanip>

bool CharaFormula::Initialize(const std::string& dbPath, std::string* outError)
{
	return _charaFormula.Initialize(dbPath, outError);
}

std::string CharaFormula::ReplaceVar(std::string sourceStr, const std::string& status, double value)
{
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(4) << value;
	std::string replaceStr = ss.str();

	// 正規表現を使わず、文字列検索 (find) で愚直に置換する
	size_t pos = sourceStr.find(status);
	while(pos != std::string::npos)
	{
		sourceStr.replace(pos, status.length(), replaceStr);
		// 次のキーワードを検索（同じ文字が複数あってもすべて置換する）
		pos = sourceStr.find(status, pos + replaceStr.length());
	}

	return sourceStr;
}

double CharaFormula::GetLiveCriticalMultiplier(const CharaAfterStatus& afterstatus)
{
	double charaCritRate = afterstatus.GetAfterStatus().critRate;

	// 0〜9999のサイコロを振り、会心率(%)未満なら当選
	if((rand() % 10000) < (charaCritRate * 100))
	{
		// 当選したら「会心ダメージ / 100」の倍率（例: 1.78倍）を返す
		return 1.0 + afterstatus.GetAfterStatus().critDamage / 100.0;
	}

	// 外れたら等倍（1.0倍）
	return 1.0;
}

double CharaFormula::GetLiveLuckMultiplier(const CharaAfterStatus& afterstatus)
{
	double charaLuck = afterstatus.GetAfterStatus().luck;
	// ご指定の数式で『発動確率(%)』を算出する (例: 114.0 -> 5.7%)
	double luckProbability = (charaLuck / 10.0) / 2.0;

	// 100% を超えないように上限キャップ
	if(luckProbability > 100.0) { luckProbability = 100.0; }

	// 算出した確率（%）でサイコロを振る
	if((rand() % 10000) < (luckProbability * 100))
	{
		// 当選したら2倍！
		return 2.0;
	}

	// 外れたら等倍（1.0倍）
	return 1.0;
}

double CharaFormula::GetDefenseMultiplier(const CharaAfterStatus& afterstatus, const Enemy& enemy)
{
	CharaFormulasRow row;
	if(!_charaFormula.GetCharaFormula("敵防御倍率", row))return 1.0;
	int playerLevel = _saveData.GetPlayerLevel();
	std::string expr = ReplaceVar(row.formula, "キャラレベル", playerLevel);
	expr = ReplaceVar(expr, "敵レベル", enemy.GetLevel());
	return EvaluateFormula::Evaluate(expr);
}

double CharaFormula::GetDecayRate(const CharaAfterStatus& afterstatus)
{
	CharaFormulasRow row;
	if(!_charaFormula.GetCharaFormula("ダメージ減衰率", row))return 1.0;
	std::string expr = ReplaceVar(row.formula, "Poop", afterstatus.GetAfterStatus().poop);
	return EvaluateFormula::Evaluate(expr);
}

double CharaFormula::CalculateFinalDamage(const CharaAfterStatus& afterstatus, const Enemy& enemy, bool isGaugeSuccess)
{
	if(_evaluatedDamage > 0.0)
	{
		double temp = _evaluatedDamage;
		_evaluatedDamage = 0.0; // 次の計算のために 0 にリセットしておく
		return temp;
	}

	double res_defense = GetDefenseMultiplier(afterstatus, enemy);
	double res_decay = GetDecayRate(afterstatus);
	double res_critical = GetLiveCriticalMultiplier(afterstatus);
	double res_luck = GetLiveLuckMultiplier(afterstatus);

	CharaFormulasRow rowFinal;
	if(!_charaFormula.GetCharaFormula("最終ダメージ", rowFinal)) return 0.0;

	std::string exprFinal = rowFinal.formula;
	exprFinal = ReplaceVar(exprFinal, "攻撃", afterstatus.GetAfterStatus().attack);
	exprFinal = ReplaceVar(exprFinal, "敵防御倍率", res_defense);
	exprFinal = ReplaceVar(exprFinal, "ダメージ減衰率", res_decay);
	exprFinal = ReplaceVar(exprFinal, "会心倍率", res_critical);
	exprFinal = ReplaceVar(exprFinal, "運値倍率", res_luck);

	extern std::string g_debugFormulaReplaced;
	g_debugFormulaReplaced = exprFinal;

	// 確率抽選を含んだ数式を一発で評価計算（ここが本来のルートです！）
	double baseFinalDamage = EvaluateFormula::Evaluate(exprFinal);

	// 今まで通り、ゲージ倍率を最後に掛け合わせる
	double gaugeBonus = isGaugeSuccess ? rowFinal.successValue : rowFinal.failureValue;
	double totalDamage = baseFinalDamage * gaugeBonus;

	return totalDamage;
}

double CharaFormula::CalculateEnemyDamage(const CharaAfterStatus& afterstatus, const Enemy& enemy)
{
	CharaFormulasRow row;
	if(!_charaFormula.GetCharaFormula("敵の攻撃ダメージ", row)) return 0.0;

	std::string expr = row.formula; // "(キャラ最大HP*0.02)+敵攻撃力*(敵攻撃力/キャラ防御力)"

	// 長いキーワードから順番に完全置換（文字被りバグ防止）
	expr = ReplaceVar(expr, "キャラ最大HP", afterstatus.GetAfterStatus().hp);
	expr = ReplaceVar(expr, "キャラ防御力", afterstatus.GetAfterStatus().defense);
	expr = ReplaceVar(expr, "敵攻撃力", enemy.GetAttack());

	// 数式エンジンで評価（今度は純粋な足し算として完璧に計算されます）
	double enemyDamage = EvaluateFormula::Evaluate(expr);

	return enemyDamage;
}

void CharaFormula::SetFinalDamage(double damage)
{
	_evaluatedDamage = damage;
}