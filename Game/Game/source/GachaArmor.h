#pragma once
#include "GachaBase.h"
#include "SqliteArmor.h"
#include "container.h"

class GachaArmor : public GachaBase<ArmorRow>
{
public:
	bool Roll(int count = 1);

protected:
	bool LoadRows(std::vector<ArmorRow>& outRows, std::string* outError) override;
	std::string MakeResultLine(const ArmorRow& row, std::mt19937& rng) override;
};