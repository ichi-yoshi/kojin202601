#pragma once
#include <string>
#include <vector>

struct BasicStatusRow
{
	std::string name;
	double probability = 1.0;
	double value = 0.0;
};

bool LoadBasicStatusSqlite(std::vector<BasicStatusRow>& outRows, std::string* outError = nullptr);