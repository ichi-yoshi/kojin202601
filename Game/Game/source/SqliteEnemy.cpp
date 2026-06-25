#include "SqliteEnemy.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"

bool SqliteEnemy::Initialize(const std::string& Path, std::string* outError)
{
	// データベースから全行を読み込んでクラスのメンバ変数 _rows に保存する
	return LoadEnemyBaseSqlite(_rows, outError);
}

static int EnemyBaseCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 6) { return 0; }
	auto* ctx = static_cast<std::vector<EnemyBaseRow>*>(param);

	EnemyBaseRow row;
	row.enemyName = SqliteTextUtill::FromUtf8(row_txt[0] ? row_txt[0] : "");
	row.hp = row_txt[1] ? std::atof(row_txt[1]) : 0.0;
	row.attack = row_txt[2] ? std::atof(row_txt[2]) : 0.0;
	row.lv = SqliteTextUtill::FromUtf8(row_txt[3] ? row_txt[3] : "");
	row.startLevel = row_txt[4] ? std::atoi(row_txt[4]) : 0;
	row.levelBonus = row_txt[5] ? std::atof(row_txt[5]) : 0.0;
	ctx->push_back(row);
	return 0;
}

bool SqliteEnemy::LoadEnemyBaseSqlite(std::vector<EnemyBaseRow>& outRows, std::string* outError)
{
	outRows.clear();
	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }
	char* errorMessage = nullptr;
	int ret = sqlite3_exec(dbh,
		"SELECT EnemyName, HP, Attack, Lv, StartLevel, LevelBonus FROM enemybase;",
		EnemyBaseCallback, &outRows, &errorMessage);
	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}
	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

// 以下の関数が丸ごと抜けていたため、エラーになっていました。これを追加します。
bool SqliteEnemy::GetEnemyBase(EnemyBaseRow& outRow)
{
	// 溜め込んだデータ (_rows) が空っぽなら false を返す
	if(_rows.empty())
	{
		return false;
	}

	// 今回は最初の1件（ドラゴンなど）をそのまま outRow にコピーして渡す
	outRow = _rows[0];
	return true;
}