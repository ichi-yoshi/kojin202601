#include "Gacha.h"

bool Gacha::Roll(int count)
{
	return GachaBase::Roll(count);
}

bool Gacha::LoadRows(std::vector<StatusRow>& outRows, std::string* outError)
{
	return LoadStatusSqlite(outRows, outError);
}

std::string Gacha::MakeResultRow(const StatusRow& row, std::mt19937& rng)
{
	// ランダムに値を選択
	double value = PickRandomValue(row, rng);

	// 結果の行を作成
	std::ostringstream os;

	// 小数点第1位までの精度で表示
	os << row.name << ": " << std::fixed << std::setprecision(1) << value;

	return os.str();
}