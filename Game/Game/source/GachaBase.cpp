#include "GachaBase.h"
#include "SqliteStatus.h"
#include "SqliteBasicStatus.h"
#include "SqliteArmor.h"

template <typename TRow>
bool GachaBase<TRow>::Initialize(const std::string& Path, std::string* outError)
{
	(void)Path;
	_rng = std::mt19937(std::random_device{}());
	return LoadRows(_rows, outError);
}

template <typename TRow>
bool GachaBase<TRow>::Roll(int count)
{
	_resultLines.clear();

	// 行がない場合はエラー
	if (_rows.empty())
	{
		_resultLines.push_back("SQLite empty");
		_hasResult = true;
		return false;
	}

	// 確率に基づいて行をランダムに選ぶ
	std::vector<double> probability;
	probability.reserve(_rows.size());
	for (const auto& r : _rows) { probability.push_back(r.probability); }
	std::discrete_distribution<size_t> distRow(probability.begin(), probability.end());

	// 指定された回数だけ行を選んで結果を作る
	for (int i = 0; i < count; ++i)
	{
		const auto& row = _rows[distRow(_rng)];
		_resultLines.push_back(MakeResultLine(row, _rng));
	}

	_hasResult = true;
	return true;
}

template <typename TRow>
void GachaBase<TRow>::ClearResult()
{
	_resultLines.clear();
	_hasResult = false;
}

// 明示的インスタンス化
template class GachaBase<StatusRow>;
template class GachaBase<BasicStatusRow>;
template class GachaBase<ArmorRow>;
