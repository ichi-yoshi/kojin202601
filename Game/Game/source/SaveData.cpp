#include "SaveData.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <iomanip>

const std::vector<SaveData::AccountData>& SaveData::GetRows() const
{
	return _accountData;
}

bool SaveData::UpdateAccountAndSave(const AccountData& data, std::string* outError)
{
	// 内部の _accountData をクリアして、ModeGameBattleでまとめた新しいデータに入れ替える
	_accountData.clear();
	_accountData.push_back(data);

	// 入れ替えた最新のデータをそのままSQLiteへ保存する
	return SaveToSqlite(outError);
}

bool SaveData::SaveToSqlite(std::string* outError) const
{
	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }
	int ret = -1;
	int err = 0;
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh, "DELETE FROM AccountData;", NULL, NULL, &errorMessage);
		if(ret != SQLITE_OK) { err = 1; }
	}
	if(err == 0)
	{
		for(const auto& row : _accountData)
		{
			char sql[256];
			snprintf(sql, sizeof(sql),
				"INSERT INTO AccountData(uid, level, exp, coin, ClearCount) VALUES (%d, %d, %d, %d, %d);",
				row.uid, row.level, row.exp, row.coin, row.enemylevel);
			char* errorMessage;
			ret = sqlite3_exec(dbh, sql, NULL, NULL, &errorMessage);
			if(ret != SQLITE_OK) { err = 1; break; }
		}
	}
	sqlite3_close(dbh);
	if(err != 0 && outError)
	{
		*outError = "SQLite save failed";
		return false;
	}
	return true;
}

bool SaveData::LoadFromSqlite(std::string* outError)
{
	_accountData.clear();
	_hasResult = false;
	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }
	LoadContext ctx{ &_accountData };
	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT uid, level, exp, coin, ClearCount FROM AccountData;",
		LoadCallback, &ctx, &errorMessage);
	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite load failed";
	}
	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

int SaveData::LoadCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 5) { return 0; }
	auto* ctx = static_cast<LoadContext*>(param);
	SaveData::AccountData data;
	data.uid = row_txt[0] ? std::atoi(row_txt[0]) : 0;
	data.level = row_txt[1] ? std::atoi(row_txt[1]) : 0;
	data.exp = row_txt[2] ? std::atoi(row_txt[2]) : 0;
	data.coin = row_txt[3] ? std::atoi(row_txt[3]) : 0;
	data.enemylevel = row_txt[4] ? std::atoi(row_txt[4]) : 0;
	ctx->accountData->push_back(data);
	return 0;
}

bool SaveData::HasResult() const
{
	return _hasResult;
}

bool SaveData::IncrementEnemyLevelAndSave(std::string* outError)
{
	if(_accountData.empty())
	{
		// 念のため、セーブデータが空なら初期データを1件作る
		AccountData newData = { 0 }; // uidなどを0で初期化
		_accountData.push_back(newData);
	}

	// 敵が倒されたので、enemylevel を +1 する
	_accountData[0].enemylevel += 1;

	// そのままデータベースに保存する
	return SaveToSqlite(outError);
}

int SaveData::GetPlayerLevel() const
{
	// セーブデータが空でなければ、先頭レコードのレベルを返す
	if(!_accountData.empty())
	{
		return _accountData[0].level;
	}

	// データがない場合は初期値として 1 を返す安全ガード
	return 1;
}

std::vector<std::string> SaveData::ToLines() const
{
	std::vector<std::string> lines;

	// ラベルと値を行に追加するラムダ
	auto push = [&lines](const char* label, double value)
		{
			std::ostringstream os;
			os << label << ":" << std::fixed << std::setprecision(1) << value;
			lines.push_back(os.str());
		};

	push("uid", _accountData[0].uid);
	push("level", _accountData[0].level);
	push("exp", _accountData[0].exp);
	push("coin", _accountData[0].coin);
	push("ClearCount", _accountData[0].enemylevel);

	return lines;
}