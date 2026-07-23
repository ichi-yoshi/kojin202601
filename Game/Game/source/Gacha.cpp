#include "Gacha.h"

bool Gacha::Roll(int count)
{
	_isMaxVal.clear(); // 前回の結果をクリア
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

	// 選択された値が最大値に近いかどうかを判定
	bool isMax = (std::abs(value - row.values[4]) < 0.1); 
	_isMaxVal.push_back(isMax);

	// 結果の行を作成
	std::ostringstream os;

	// 小数点第1位までの精度で表示
	os << row.name << ": " << std::fixed << std::setprecision(1) << value;

	return os.str();
}