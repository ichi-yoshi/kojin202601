#pragma once
#include "container.h"
#include "SqliteUtill.h"

class SaveEquipment
{
public:
	enum class EquipPart
	{
		Head,	// 頭
		Body,	// 胴
		Arms,	// 腕
		Feet,	// 靴
		_EOT_
	};

	struct PartResult
	{
		bool hasResult = false;
		std::string armorName;
		std::vector<std::string> basicStatusRows;
		std::vector<std::string> statusRows;
	};

	// 結果の保存
	void SaveResult(const std::string& armorName,
		const std::vector<std::string>& basicStatusRows,
		const std::vector<std::string>& statusRows);

	// 結果の取得
	const PartResult& GetResult(EquipPart part) const;
	const char* GetPartLabel(EquipPart part) const;

	// データの保存とロード
	bool SaveToSqlite(std::string* outError = nullptr) const;
	bool LoadFromSqlite(std::string* outError = nullptr);

	// 結果のクリア
	void ClearResults();

	// 装備名から装備部位を取得する
	static EquipPart GetPartFromName(const std::string& armorName);
private:
	struct LoadContext
	{
		SaveEquipment* self = nullptr;
	};

	// SQLiteのコールバック関数
	static int LoadCallback(void* param, int col_cnt, char** row_txt, char** col_name);

	// 部位ごとの結果
	PartResult _results[static_cast<int>(EquipPart::_EOT_)];
};