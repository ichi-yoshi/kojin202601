#include "SqliteCharaStatus.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"
#include "Chara.h"

struct CharaStatusContext
{
	CharaStatus* status = nullptr;
};

static int CharaStatusCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 2) { return 0; }
	auto* ctx = static_cast<CharaStatusContext*>(param);

	// SQLiteの結果をCharaStatusに変換して加算
	const std::string rawName = row_txt[0] ? row_txt[0] : "";
	const std::string name = SqliteTextUtill::FromUtf8(rawName);
	const double value = row_txt[1] ? std::atof(row_txt[1]) : 0.0;

	// ステータス名に対応するステータスに値を加算
	Status::ApplyStatusName(*ctx->status, name, value);
	return 0;
}

bool LoadCharaBaseStatusSqlite(CharaStatus& outStatus, std::string* outError)
{
	outStatus = CharaStatus{};

	// SQLiteデータベースに接続
	sqlite3* dbh = nullptr;
	if (!OpenSqliteConnection(&dbh, outError)) { return false; }

	CharaStatusContext ctx{ &outStatus };

	// SQLiteのクエリを実行してデータを取得
	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT StatusName, Val1 FROM chara_status;",
		CharaStatusCallback, &ctx, &errorMessage);

	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}
