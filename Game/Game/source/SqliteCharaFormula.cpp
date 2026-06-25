#include "SqliteCharaFormula.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"

struct CharaFormulaContext
{
	std::vector<CharaFormulasRow>* rows = nullptr;
};

bool SqliteCharaFormula::Initialize(const std::string& dbPath, std::string* outError)
{
	return LoadCharaFormulasSqlite(_rows, outError);
}

static int CharaFormulasCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 4) { return 0; }
	auto* ctx = static_cast<CharaFormulaContext*>(param);
	CharaFormulasRow row;
	row.formulaName = SqliteTextUtill::FromUtf8(row_txt[0] ? row_txt[0] : "");
	row.formula = SqliteTextUtill::FromUtf8(row_txt[1] ? row_txt[1] : "");
	row.successValue = row_txt[2] ? std::atof(row_txt[2]) : 0.0;
	row.failureValue = row_txt[3] ? std::atof(row_txt[3]) : 0.0;
	ctx->rows->push_back(row);
	return 0;
}

bool SqliteCharaFormula::LoadCharaFormulasSqlite(std::vector<CharaFormulasRow>& outRows, std::string* outError)
{
	outRows.clear();
	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }
	CharaFormulaContext context;
	context.rows = &outRows;
	char* errorMessage = nullptr;
	int ret = sqlite3_exec(dbh,
		"SELECT FormulaName, Formula, gaugeSuccess, gaugeFail FROM charaFormulas;",
		CharaFormulasCallback, &context, &errorMessage);
	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}
	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

bool SqliteCharaFormula::GetCharaFormula(const std::string& name, CharaFormulasRow& outRow)
{
	for(const auto& row : _rows)
	{
		if(row.formulaName == name)
		{
			outRow = row;
			return true;
		}
	}
	return false;
}