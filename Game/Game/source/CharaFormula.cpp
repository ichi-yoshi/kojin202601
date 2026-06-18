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

double CharaFormula::GetCriticalMultiplier(const CharaAfterStatus& afterstatus)
{
	CharaFormulasRow row;
	if(!_charaFormula.GetCharaFormula("会心倍率", row))return 1.0;
	std::string expr = ReplaceVar(row.formula, "会心率", afterstatus.GetAfterStatus().critRate); 
	expr = ReplaceVar(expr, "会心ダメージ", afterstatus.GetAfterStatus().critDamage); 
	return EvaluateFormula::Evaluate(expr);
}

double CharaFormula::GetDefenseMultiplier(const CharaAfterStatus& afterstatus, const Enemy& enemy)
{
	CharaFormulasRow row;
	if(!_charaFormula.GetCharaFormula("防御倍率", row))return 1.0;
	std::string expr = ReplaceVar(row.formula, "キャラレベル", 10);
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

double CharaFormula::GetLuckExpectation(const CharaAfterStatus& afterstatus)
{
	CharaFormulasRow row;
	if(!_charaFormula.GetCharaFormula("運値期待値", row))return 1.0;
	std::string expr = ReplaceVar(row.formula, "運値", afterstatus.GetAfterStatus().luck);
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

	double res_critical = GetCriticalMultiplier(afterstatus);
	double res_defense = GetDefenseMultiplier(afterstatus, enemy);
	double res_decay = GetDecayRate(afterstatus);
	double res_luck = GetLuckExpectation(afterstatus);

	CharaFormulasRow rowFinal;
	if(!_charaFormula.GetCharaFormula("最終ダメージ", rowFinal)) return 0.0;

	std::string exprFinal = rowFinal.formula;
	exprFinal = ReplaceVar(exprFinal, "攻撃", afterstatus.GetAfterStatus().attack);
	exprFinal = ReplaceVar(exprFinal, "会心倍率", res_critical);
	exprFinal = ReplaceVar(exprFinal, "敵防御倍率", res_defense);
	exprFinal = ReplaceVar(exprFinal, "ダメージ減衰率", res_decay);
	exprFinal = ReplaceVar(exprFinal, "運値期待値", res_luck);

	double baseFinalDamage = EvaluateFormula::Evaluate(exprFinal);
	double gaugeBonus = isGaugeSuccess ? rowFinal.successValue : rowFinal.failureValue;
	double totalDamage = baseFinalDamage * gaugeBonus;

	std::cout << "[最終ダメージ計算式]: " << exprFinal << " * 補正(" << gaugeBonus << ") = " << totalDamage << std::endl;

	return totalDamage;
}

void CharaFormula::SetFinalDamage(double damage)
{
	_evaluatedDamage = damage;
}