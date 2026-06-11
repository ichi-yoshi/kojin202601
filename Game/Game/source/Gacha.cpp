#include "Gacha.h"
#include <sstream>
#include <iomanip>

bool Gacha::Roll(int count)
{
	return GachaBase::Roll(count);
}

bool Gacha::LoadRows(std::vector<StatusRow>& outRows, std::string* outError)
{
	return LoadStatusSqlite(outRows, outError);
}

std::string Gacha::MakeResultLine(const StatusRow& row, std::mt19937& rng)
{
	double value = PickRandomValue(row, rng);
	std::ostringstream os;
	os << row.name << ": " << std::fixed << std::setprecision(1) << value;
	return os.str();
}