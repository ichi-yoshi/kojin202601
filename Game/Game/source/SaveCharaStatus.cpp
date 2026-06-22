#include "SaveCharaStatus.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"
#include <cstdio>
#include <cstdlib>

// CharaAfterStatusからステータスを行の配列に変換して保存する
void SaveCharaStatus::SetFromAfterStatus(const CharaAfterStatus& afterStatus)
{
	_rows.clear();

	const auto& s = afterStatus.GetAfterStatus();
	_rows.push_back({ "HP", s.hp });
	_rows.push_back({ "攻撃", s.attack });
	_rows.push_back({ "防御", s.defense });
	_rows.push_back({ "HP%", s.hpPercent });
	_rows.push_back({ "攻撃%", s.attackPercent });
	_rows.push_back({ "防御%", s.defensePercent });
	_rows.push_back({ "会心率", s.critRate });
	_rows.push_back({ "会心ダメージ", s.critDamage });
	_rows.push_back({ "速度", s.speed });
	_rows.push_back({ "運値", s.luck });
	_rows.push_back({ "Poop", s.poop });

	_hasResult = true;
}

const std::vector<SaveCharaStatus::StatusRow>& SaveCharaStatus::GetRows() const
{
	return _rows;
}

bool SaveCharaStatus::SaveToSqlite(std::string* outError) const
{
	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }

	int ret = -1;
	int err = 0;

	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh, "DELETE FROM chara_afterstatus;", NULL, NULL, &errorMessage);
		if(ret != SQLITE_OK) { err = 1; }
	}
	if(err == 0)
	{
		for(const auto& row : _rows)
		{
			std::string utf8Name = SqliteTextUtill::ToUtf8(row.name);
			std::string name = SqliteTextUtill::EscapeSqlString(utf8Name);
			char sql[256];
			snprintf(sql, sizeof(sql),
				"INSERT INTO chara_afterstatus(StatusName, Val, hasResult) VALUES ('%s', %.3f, %d);",
				name.c_str(), row.value, _hasResult ? 1 : 0);
			char* errorMessage;
			ret = sqlite3_exec(dbh, sql, NULL, NULL, &errorMessage);
			if(ret != SQLITE_OK) { err = 1; break; }
		}
	}
	sqlite3_close(dbh);
	return err == 0;
}

int SaveCharaStatus::LoadCallback(void* param, int col_cnt, char** row_txt, char** ) 
{
	if(!param || col_cnt < 3) { return 0; }
	auto* ctx = static_cast<LoadContext*>(param);
	auto* self = ctx->self;

	std::string rawName = row_txt[0] ? row_txt[0] : "";

	const std::string name = SqliteTextUtill::FromUtf8(rawName);
	const double value = row_txt[1] ? std::atof(row_txt[1]) : 0.0;
	const bool hasResult = (row_txt[2] ? std::atoi(row_txt[2]) : 0) == 1;

	if(!hasResult) { return 0; }

	self->_rows.push_back({ name, value });
	self->_hasResult = true;
	return 0;
}

bool SaveCharaStatus::LoadFromSqlite(std::string* outError) 
{
	_rows.clear();
	_hasResult = false;

	sqlite3* dbh = nullptr;
	if (!OpenSqliteConnection(&dbh, outError)) { return false; }

	LoadContext ctx{this};

	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT StatusName, Val, 1 AS hasResult FROM chara_afterstatus;",
		LoadCallback, &ctx, &errorMessage);

	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

void SaveCharaStatus::ClearStatus()
{
	_rows.clear();
	_hasResult = false;
}

bool SaveCharaStatus::HasResult() const
{
	return _hasResult;
}
