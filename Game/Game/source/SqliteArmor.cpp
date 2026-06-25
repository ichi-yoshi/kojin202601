#include "SqliteArmor.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"

struct ArmorContext
{
	std::vector<ArmorRow>* rows = nullptr;
};

// SQLiteのコールバック関数
static int ArmorCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 2) { return 0; }
	auto* ctx = static_cast<ArmorContext*>(param);

	ArmorRow row;
	row.name = SqliteTextUtill::FromUtf8(row_txt[0] ? row_txt[0] : "");
	row.probability = row_txt[1] ? std::atof(row_txt[1]) : 0.0;

	ctx->rows->push_back(row);
	return 0;
}

// SQLiteから装備のデータを読み込む
bool LoadArmorSqlite(std::vector<ArmorRow>& outRows, std::string* outError)
{
	outRows.clear();

	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }

	ArmorContext ctx{ &outRows };

	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT ArmorName, probability FROM armor;",
		ArmorCallback, &ctx, &errorMessage);

	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}
