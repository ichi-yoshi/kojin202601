#include "GachaBase.h"
#include "SqliteStatus.h"
#include "SqliteBasicStatus.h"
#include "SqliteArmor.h"

template <typename TRow>
bool GachaBase<TRow>::Initialize(const std::string& Path, std::string* outError)
{
	// SQLiteの行データをロードする
	(void)Path;

	// 乱数生成器を初期化する
	_rng = std::mt19937(std::random_device{}());

	return LoadRows(_rows, outError);
}

template <typename TRow>
bool GachaBase<TRow>::Roll(int count)
{
	// 結果をクリアする
	_resultLines.clear();

	// 行がない場合はエラー
	if(_rows.empty())
	{
		_resultLines.push_back("SQLite empty");
		_hasResult = true;
		return false;
	}

	// 確率に基づいて行をランダムに選ぶ
	std::vector<double> probability;													// 確率の配列を作成
	probability.reserve(_rows.size());													// 事前に容量を確保しておく
	for(const auto& r : _rows) { probability.push_back(r.probability); }				// 確率を配列に追加
	std::discrete_distribution<size_t> distRow(probability.begin(), probability.end());	// 確率に基づく離散分布を作成

	// 指定された回数だけ行を選んで結果を作る
	for(int i = 0; i < count; ++i)
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
