#include "GachaBasicStatus.h"

bool GachaBasicStatus::Roll(int count)
{
	return GachaBase::Roll(count);
}

bool GachaBasicStatus::LoadRows(std::vector<BasicStatusRow>& outRows, std::string* outError)
{
	return LoadBasicStatusSqlite(outRows, outError);
}

std::string GachaBasicStatus::MakeResultLine(const BasicStatusRow& row, std::mt19937&)
{
	std::ostringstream os;
	os << row.name << ": " << std::fixed << std::setprecision(1) << row.value;
	return os.str();
}