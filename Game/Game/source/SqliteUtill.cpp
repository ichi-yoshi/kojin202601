#include "SqliteUtill.h"
#include "SqliteConfig.h"

// SQLiteのデータベースに接続する
bool OpenSqliteConnection(sqlite3** outDb, std::string* outError)
{
	if(!outDb) { return false; }
	*outDb = nullptr;

	// データベースに接続
	int ret = sqlite3_open_v2(
		kSqliteDbPath,
		outDb,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		nullptr);

	// 接続失敗
	if(ret != SQLITE_OK)
	{
		if(outError) { *outError = "SQLite open failed"; }
		return false;
	}
	return true;
}