#include "CharaAfterStatus.h"
#include "Chara.h"
#include "SaveEquipment.h"

bool CharaAfterStatus::InitializeSpeedTable(const std::string& dbPath, std::string* outError)
{
	if(!_sqliteSpeed.LoadSpeedSqlite(_sqliteSpeed._rows, outError))
	{
		return false;
	}
	return true;
}

void CharaAfterStatus::SetAfterStatus(const CharaStatus& status)
{
	_afterStatus = status;
}

// 装備から最終ステータスを更新する
void CharaAfterStatus::UpdateFrom(const CharaBase& base, const SaveEquipment& saveEquipment)
{
	CharaStatus equipTotal{};

	// 名前空間の使用宣言
	using namespace Status;

	for(int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		const auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);
		if(!result.hasResult) { continue; }

		const auto basic = Chara::ParseStatusLines(result.basicStatusLines);
		const auto sub = Chara::ParseStatusLines(result.statusLines);

		// 基礎ステータスと装備ステータスを合計する
		Status::AddStatus(equipTotal, basic);
		Status::AddStatus(equipTotal, sub);
	}

	const auto after = Chara::CalculateAfterStatus(base, equipTotal);
	_afterStatus = after.GetAfterStatus();

	_bonusTime = _sqliteSpeed.GetBonusTime(_afterStatus.speed);
	_coolTime = 1.0 + _bonusTime;
}

// ステータスを行の文字列配列に変換する
std::vector<std::string> CharaAfterStatus::ToLines() const
{
	std::vector<std::string> lines;

	// ラベルと値を行に追加するラムダ
	auto push = [&lines](const char* label, double value)
	{
			std::ostringstream os;
			os << label << ":" << std::fixed << std::setprecision(1) << value;
			lines.push_back(os.str());
	};

	push("HP", _afterStatus.hp);
	push("攻撃", _afterStatus.attack);
	push("防御", _afterStatus.defense);
	push("HP%", _afterStatus.hpPercent);
	push("攻撃%", _afterStatus.attackPercent);
	push("防御%", _afterStatus.defensePercent);
	push("会心率", _afterStatus.critRate);
	push("会心ダメージ", _afterStatus.critDamage);
	push("速度", _afterStatus.speed);
	push("運値", _afterStatus.luck);
	push("Poop", _afterStatus.poop);

	return lines;
}