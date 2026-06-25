#pragma once
#include "container.h"

struct CharaFormulasRow
{
	std::string formulaName;
	std::string formula;
	double successValue;
	double failureValue;
};

class SqliteCharaFormula
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	bool LoadCharaFormulasSqlite(std::vector<CharaFormulasRow>& outRows, std::string* outError = nullptr);
	bool GetCharaFormula(const std::string& name, CharaFormulasRow& outRow);
private:
	std::vector<CharaFormulasRow> _rows;
};

