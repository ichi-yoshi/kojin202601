#include "Chara.h"
#include <sstream>
#include <iomanip>

namespace 
{
	// ステータスを加算する（％は加算後に反映される前提）
	void AddStatus(CharaStatus& dst, const CharaStatus& src)
	{
		dst.hp += src.hp;
		dst.attack += src.attack;
		dst.defense += src.defense;

		dst.hpPercent += src.hpPercent;
		dst.attackPercent += src.attackPercent;
		dst.defensePercent += src.defensePercent;

		dst.critRate += src.critRate;
		dst.critDamage += src.critDamage;
		dst.speed += src.speed;
		dst.luck += src.luck;
		dst.poop += src.poop;
	}

	// ステータス名に対応するステータスに値を加算する
	void ApplyStatusName(CharaStatus& status, const std::string& name, double value)
	{
		if (name == "HP") {status.hp += value;}
		else if (name == "攻撃") {status.attack += value;}
		else if (name == "防御") {status.defense += value;}
		else if (name == "HP%") {status.hpPercent += value;}
		else if (name == "攻撃%") {status.attackPercent += value;}
		else if (name == "防御%") {status.defensePercent += value;}
		else if (name == "会心率") {status.critRate += value;}
		else if (name == "会心ダメージ") {status.critDamage += value;}
		else if (name == "速度") {status.speed += value;}
		else if (name == "運値") {status.luck += value;}
		else if (name == "Poop") {status.poop += value;}
	}
}

// 基礎 + 装備合計 で最終ステータスを作成
CharaAfterStatus Chara::CalculateAfterStatus(const CharaBase& base, const CharaStatus& equipTotal)
{
	CharaAfterStatus after;
	CharaStatus result = base.GetBaseStatus();
	AddStatus(result, equipTotal);


	// ％を実数に反映（合算後に適用）
	const double hpRate = 1.0 + (result.hpPercent / 100.0);
	const double attackRate = 1.0 + (result.attackPercent / 100.0);
	const double defenseRate = 1.0 + (result.defensePercent / 100.0);

	result.hp *= hpRate;
	result.attack *= attackRate;
	result.defense *= defenseRate;

	after.SetAfterStatus(result);
	return after;
}

// ステータスの行からステータスを作成する
CharaStatus Chara::ParseStatusLines(const std::vector<std::string>& lines)
{
	CharaStatus status;

	for(const auto& line : lines)
	{
		const auto pos = line.find(':');
		if (pos == std::string::npos) { continue; }

		const std::string name = line.substr(0, pos);
		const std::string valueText = line.substr(pos + 1);

		try 
		{
			const double value = std::stod(valueText);
			ApplyStatusName(status, name, value);
		}
		catch (...)
		{
			// 変換失敗は無視
			continue;
		}
	}
	return status;
}
