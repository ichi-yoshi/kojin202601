#pragma once
#include "container.h"
#include "CharaBase.h"
#include "SaveEquipment.h"
#include "SqliteSpeed.h"

class CharaAfterStatus
{
public:
	void SetAfterStatus(const CharaStatus& status);
	const CharaStatus& GetAfterStatus() const;

	bool InitializeSpeedTable(const std::string& dbPath, std::string* outError = nullptr);

	// 装備から最終ステータスを更新する
	void UpdateFrom(const CharaBase& base, const SaveEquipment& saveEquipment);

	// ステータスを行の文字列配列に変換する
	std::vector<std::string> ToLines() const;

	double GetcoolTime() const { return _coolTime; };
	double GetBonusTime()const { return _bonusTime; };
private:
	CharaStatus _afterStatus;
	SqliteSpeed _sqliteSpeed;
	double _coolTime = 0.0;
	double _bonusTime = 0.0;
};

