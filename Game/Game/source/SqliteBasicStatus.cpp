#include "SqliteBasicStatus.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"
#include <cstdlib>

struct BasicStatusContext
{
	std::vector<BasicStatusRow>* rows = nullptr;
};

static int BasicStatusCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 3) { return 0; }
	auto* ctx = static_cast<BasicStatusContext*>(param);

	BasicStatusRow row;
	row.name = SqliteTextUtill::FromUtf8(row_txt[0] ? row_txt[0] : "");
	row.probability = row_txt[1] ? std::atof(row_txt[1]) : 0.0;
	row.value = row_txt[2] ? std::atof(row_txt[2]) : 0.0;

	ctx->rows->push_back(row);
	return 0;
}

bool LoadBasicStatusSqlite(std::vector<BasicStatusRow>& outRows, std::string* outError)
{
	outRows.clear();

	sqlite3* dbh = nullptr;
	if (!OpenSqliteConnection(&dbh, outError)) { return false; }

	BasicStatusContext ctx{ &outRows };

	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT StatusName, probability, Val1 FROM basic_status;",
		BasicStatusCallback, &ctx, &errorMessage);

	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}