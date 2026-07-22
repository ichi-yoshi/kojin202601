#include "GachaBasicStatus.h"

bool GachaBasicStatus::Roll(int count)
{
	return GachaBase::Roll(count);
}

bool GachaBasicStatus::LoadRows(std::vector<BasicStatusRow>& outRows, std::string* outError)
{
	return LoadBasicStatusSqlite(outRows, outError);
}

std::string GachaBasicStatus::MakeResultRow(const BasicStatusRow& row, std::mt19937&)
{
	// 結果の行を作成する
	std::ostringstream os;

	// 小数点第1位までの精度で表示する
	os << row.name << ": " << std::fixed << std::setprecision(1) << row.value;

	return os.str();
}