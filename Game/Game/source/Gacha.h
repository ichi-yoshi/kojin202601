#pragma once
#include "GachaBase.h"
#include "SqliteStatus.h"
#include "container.h"

class Gacha : public GachaBase<StatusRow>
{
public:
	bool Roll(int count = 4);

	const std::vector<bool>& GetIsMaxVal() const { return _isMaxVal; }

protected:
	// GachaBaseの純粋仮想関数を実装
	bool LoadRows(std::vector<StatusRow>& outRows, std::string* outError) override;
	std::string MakeResultRow(const StatusRow& row, std::mt19937& rng) override;
	std::vector<bool> _isMaxVal;
};