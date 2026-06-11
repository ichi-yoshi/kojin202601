#pragma once
#include <string>
#include <vector>
#include "../sqlite3/sqlite3.h"

class SaveEquipment
{
public:
	enum class EquipPart
	{
		Head,
		Body,
		Arms,
		Feet,
		_EOT_
	};

	struct PartResult
	{
		bool hasResult = false;
		std::string armorName;
		std::vector<std::string> basicStatusLines;
		std::vector<std::string> statusLines;
	};

	// 結果の保存
	void SaveResult(const std::string& armorName,
		const std::vector<std::string>& basicStatusLines,
		const std::vector<std::string>& statusLines);

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