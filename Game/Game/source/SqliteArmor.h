#pragma once
#include "container.h"

struct ArmorRow
{
	std::string name;	// 装備名
	double probability;	// 確率
};

// SQLiteから装備のデータを読み込む
bool LoadArmorSqlite(std::vector<ArmorRow>& outRows, std::string* outError = nullptr);