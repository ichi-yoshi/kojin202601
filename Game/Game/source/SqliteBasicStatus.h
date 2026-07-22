#pragma once
#include "container.h"

struct BasicStatusRow
{
	std::string name;	// ステータス名
	double probability;	// 確率
	double value;		// Val1
};

// SQLiteから基礎ステータスのデータを読み込む
bool LoadBasicStatusSqlite(std::vector<BasicStatusRow>& outRows, std::string* outError = nullptr);