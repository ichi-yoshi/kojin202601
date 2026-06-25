#include "SqliteStatus.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"

struct StatusContext
{
	std::vector<StatusRow>* rows = nullptr;
};

static int StatusCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 7) { return 0; }
	auto* ctx = static_cast<StatusContext*>(param);

	StatusRow row;
	row.name = SqliteTextUtill::FromUtf8(row_txt[0] ? row_txt[0] : "");
	row.probability = row_txt[1] ? std::atof(row_txt[1]) : 0.0;
	row.values[0] = row_txt[2] ? std::atof(row_txt[2]) : 0.0;
	row.values[1] = row_txt[3] ? std::atof(row_txt[3]) : 0.0;
	row.values[2] = row_txt[4] ? std::atof(row_txt[4]) : 0.0;
	row.values[3] = row_txt[5] ? std::atof(row_txt[5]) : 0.0;
	row.values[4] = row_txt[6] ? std::atof(row_txt[6]) : 0.0;

	ctx->rows->push_back(row);
	return 0;
}

// SQLiteからサブステータスのデータを読み込む
bool LoadStatusSqlite(std::vector<StatusRow>& outRows, std::string* outError)
{
	outRows.clear();

	sqlite3* dbh = nullptr;
	if (!OpenSqliteConnection(&dbh, outError)) { return false; }

	StatusContext ctx{ &outRows };

	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT StatusName, probability, Val1, Val2, Val3, Val4, Val5 FROM status;",
		StatusCallback, &ctx, &errorMessage);

	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

// サブステータスの行からランダムに値を1つ選ぶ
double PickRandomValue(const StatusRow& row, std::mt19937& rng)
{
	std::uniform_int_distribution<int> dist(0, 4);
	return row.values[dist(rng)];
}