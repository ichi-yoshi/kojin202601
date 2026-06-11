#pragma once
#include <string>
#include <vector>

struct ArmorRow
{
	std::string name;
	double probability = 1.0;
};

// SQLiteから装備のデータを読み込む
bool LoadArmorSqlite(std::vector<ArmorRow>& outRows, std::string* outError = nullptr);