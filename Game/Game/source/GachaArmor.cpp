#include "GachaArmor.h"

bool GachaArmor::Roll(int count)
{
	return GachaBase::Roll(count);
}

bool GachaArmor::LoadRows(std::vector<ArmorRow>& outRows, std::string* outError)
{
	return LoadArmorSqlite(outRows, outError);
}

std::string GachaArmor::MakeResultLine(const ArmorRow& row, std::mt19937&)
{
	return row.name;
}