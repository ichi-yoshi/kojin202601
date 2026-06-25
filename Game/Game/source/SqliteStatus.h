#pragma once
#include "container.h"

struct StatusRow
{
	std::string name;
	double probability;
	std::array<double, 5> values{};
};

// SQLiteからサブステータスのデータを読み込む
bool LoadStatusSqlite(std::vector<StatusRow>& outRows, std::string* outError = nullptr);

// サブステータスの行からランダムに値を1つ選ぶ
double PickRandomValue(const StatusRow& row, std::mt19937& rng);