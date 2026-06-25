#include "SaveEquipment.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"

namespace
{
	const SaveEquipment::PartResult EmptyResult;
}

// 装備名から装備部位を判定する
SaveEquipment::EquipPart SaveEquipment::GetPartFromName(const std::string& armorName)
{
	if (armorName.find("頭") != std::string::npos) { return EquipPart::Head; }
	if (armorName.find("胴") != std::string::npos) { return EquipPart::Body; }
	if (armorName.find("腕") != std::string::npos) { return EquipPart::Arms; }
	if (armorName.find("靴") != std::string::npos) { return EquipPart::Feet; }
	return EquipPart::Head;
}

// 結果の保存
void SaveEquipment::SaveResult(const std::string& armorName,
	const std::vector<std::string>& basicStatusLines,
	const std::vector<std::string>& statusLines)
{
	if (armorName.empty()) { return; }

	EquipPart part = GetPartFromName(armorName);
	auto& result = _results[static_cast<int>(part)];

	result.hasResult = true;
	result.armorName = armorName;
	result.basicStatusLines = basicStatusLines;
	result.statusLines = statusLines;
}

// 結果の取得
const SaveEquipment::PartResult& SaveEquipment::GetResult(EquipPart part) const
{
	int index = static_cast<int>(part);
	if (index < 0 || index >= static_cast<int>(EquipPart::_EOT_)) { return EmptyResult; }
	return _results[index];
}

// 装備部位のラベルを取得する
const char* SaveEquipment::GetPartLabel(EquipPart part) const
{
	switch (part)
	{
	case EquipPart::Head: return "頭";
	case EquipPart::Body: return "胴";
	case EquipPart::Arms: return "腕";
	case EquipPart::Feet: return "靴";
	default: return "";
	}
}

// データをSQLiteに保存する
bool SaveEquipment::SaveToSqlite(std::string* outError) const
{
	sqlite3* dbh = nullptr;
	if (!OpenSqliteConnection(&dbh, outError)) { return false; }

	int ret = -1;
	int err = 0;

	if (err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh, "DELETE FROM save_equipment;", NULL, NULL, &errorMessage);
		if (ret != SQLITE_OK) { err = 1; }
	}

	if (err == 0)
	{
		for (int i = 0; i < static_cast<int>(EquipPart::_EOT_); ++i)
		{
			const auto& result = _results[i];

			// CP932 -> UTF-8 に変換する
			std::string armorUtf8 = SqliteTextUtill::ToUtf8(result.armorName);
			std::string basicUtf8 = SqliteTextUtill::ToUtf8(SqliteTextUtill::JoinLines(result.basicStatusLines));
			std::string statusUtf8 = SqliteTextUtill::ToUtf8(SqliteTextUtill::JoinLines(result.statusLines));

			// UTF-8 にした文字列をエスケープする
			std::string armor = SqliteTextUtill::EscapeSqlString(armorUtf8);
			std::string basic = SqliteTextUtill::EscapeSqlString(basicUtf8);
			std::string status = SqliteTextUtill::EscapeSqlString(statusUtf8);

			char sql[1024];
			snprintf(sql, sizeof(sql),
				"INSERT INTO save_equipment(part, hasResult, armorName, basicStatusLines, statusLines) "
				"VALUES(%d, %d, '%s', '%s', '%s');",
				i, result.hasResult ? 1 : 0,
				armor.c_str(), basic.c_str(), status.c_str());

			char* errorMessage;
			ret = sqlite3_exec(dbh, sql, NULL, NULL, &errorMessage);
			if (ret != SQLITE_OK) { err = 1; break; }
		}
	}

	sqlite3_close(dbh);
	return err == 0;
}

// SQLiteからデータをロードするためのコールバック関数
int SaveEquipment::LoadCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if (!param || col_cnt < 5) { return 0; }
	auto* ctx = static_cast<LoadContext*>(param);
	auto* self = ctx->self;

	int index = row_txt[0] ? std::atoi(row_txt[0]) : 0;
	if (index < 0 || index >= static_cast<int>(SaveEquipment::EquipPart::_EOT_)) { return 0; }

	auto& result = self->_results[index];
	result = SaveEquipment::PartResult{};
	result.hasResult = (row_txt[1] ? std::atoi(row_txt[1]) : 0) == 1;
	if (!result.hasResult) { return 0; }

	std::string armorUtf8 = row_txt[2] ? row_txt[2] : "";
	std::string basicUtf8 = row_txt[3] ? row_txt[3] : "";
	std::string statusUtf8 = row_txt[4] ? row_txt[4] : "";

	result.armorName = SqliteTextUtill::FromUtf8(armorUtf8);

	std::string basicSjis = SqliteTextUtill::FromUtf8(basicUtf8);
	SqliteTextUtill::SplitLines(basicSjis, result.basicStatusLines);

	std::string statusSjis = SqliteTextUtill::FromUtf8(statusUtf8);
	SqliteTextUtill::SplitLines(statusSjis, result.statusLines);
	return 0;
}

// SQLiteからデータをロードする
bool SaveEquipment::LoadFromSqlite(std::string* outError)
{
	for (int i = 0; i < static_cast<int>(EquipPart::_EOT_); ++i)
	{
		_results[i] = PartResult{};
	}

	sqlite3* dbh = nullptr;
	if (!OpenSqliteConnection(&dbh, outError)) { return false; }

	LoadContext ctx{ this };

	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT part, hasResult, armorName, basicStatusLines, statusLines FROM save_equipment;",
		LoadCallback, &ctx, &errorMessage);

	if (ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite load failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

// 結果のクリア
void SaveEquipment::ClearResults()
{
	for (int i = 0; i < static_cast<int>(EquipPart::_EOT_); ++i)
	{
		_results[i] = PartResult{};
	}
}