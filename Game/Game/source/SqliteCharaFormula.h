#pragma once
#include "container.h"

struct CharaFormulasRow
{
	std::string formulaName;	// 計算式名
	std::string formula;		// 計算式の内容
	double successValue;		// 成功時の値
	double failureValue;		// 失敗時の値
};

class SqliteCharaFormula
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	// SQLiteから計算式のデータを読み込む
	bool LoadCharaFormulasSqlite(std::vector<CharaFormulasRow>& outRows, std::string* outError = nullptr);

	// 指定された計算式名に対応する行を取得する
	bool GetCharaFormula(const std::string& name, CharaFormulasRow& outRow);
private:
	std::vector<CharaFormulasRow> _rows;
};

