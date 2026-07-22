#pragma once
#include "GachaBase.h"
#include "SqliteBasicStatus.h"
#include "container.h"

class GachaBasicStatus : public GachaBase<BasicStatusRow>
{
public:
	bool Roll(int count = 1);

protected:
	// GachaBaseの純粋仮想関数を実装
	bool LoadRows(std::vector<BasicStatusRow>& outRows, std::string* outError) override;
	std::string MakeResultRow(const BasicStatusRow& row, std::mt19937& rng) override;
};