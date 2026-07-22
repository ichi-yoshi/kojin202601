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
	// 装備ステータスの合計を計算する
	CharaStatus equipTotal{};

	// 名前空間の使用宣言
	using namespace Status;

	// 装備部位ごとにステータスを取得して合計する
	for(int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		const auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);
		if(!result.hasResult) { continue; }

		// 装備のステータスを取得する
		const auto basic = Chara::ParseStatusRows(result.basicStatusRows);	// 基礎ステータス
		const auto sub = Chara::ParseStatusRows(result.statusRows);			// 装備ステータス

		// 基礎のステータスを合計する
		Status::AddStatus(equipTotal, basic);	// 基礎ステータスは装備の基本値として加算する
		Status::AddStatus(equipTotal, sub);		// 装備ステータスを合計する
	}

	// 基礎ステータスと装備ステータスの合計から最終ステータスを計算する
	const auto after = Chara::CalculateAfterStatus(base, equipTotal);
	_afterStatus = after.GetAfterStatus();

	// 速度ステータスからボーナスタイムを取得する
	_bonusTime = _sqliteSpeed.GetBonusTime(_afterStatus.speed);
	_coolTime =_basicCoolTime + _bonusTime;	// クールタイムは1秒 + ボーナスタイム
}

// ステータスを行の文字列配列に変換する
std::vector<std::string> CharaAfterStatus::ToRows() const
{
	std::vector<std::string> rows;

	// ラベルと値を行に追加するラムダ
	auto push = [&rows](const char* label, double value)
	{
			std::ostringstream os;
			os << label << ":" << std::fixed << std::setprecision(1) << value;
			rows.push_back(os.str());
	};

	// ステータスを行に追加
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

	return	rows;
}