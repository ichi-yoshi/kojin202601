#pragma once
#include <string>
#include <vector>

struct BasicStatusRow
{
	std::string name;
	double probability;
	double value;
};

bool LoadBasicStatusSqlite(std::vector<BasicStatusRow>& outRows, std::string* outError = nullptr);