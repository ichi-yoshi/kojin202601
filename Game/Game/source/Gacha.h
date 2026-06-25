#pragma once
#include "GachaBase.h"
#include "SqliteStatus.h"
#include "container.h"

class Gacha : public GachaBase<StatusRow>
{
public:
	bool Roll(int count = 4);

protected:
	bool LoadRows(std::vector<StatusRow>& outRows, std::string* outError) override;
	std::string MakeResultLine(const StatusRow& row, std::mt19937& rng) override;
};