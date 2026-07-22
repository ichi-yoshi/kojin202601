#pragma once
#include "container.h"

template <typename TRow>
class GachaBase
{
public:
	bool Initialize(const std::string& Path, std::string* outError = nullptr);

	// ガチャを回す
	bool Roll(int count);

	// ガチャの結果があるかどうかを取得
	bool HasResult() const { return _hasResult; }

	// ガチャの結果をクリアする
	void ClearResult();

	// ガチャの結果の行データを取得
	const std::vector<std::string>& GetResultLines() const { return _resultLines; }

protected:
	// 純粋仮想関数：派生クラスで実装する必要がある
	virtual bool LoadRows(std::vector<TRow>& outRows, std::string* outError) = 0;
	virtual std::string MakeResultLine(const TRow& row, std::mt19937& rng) = 0;

private:
	std::vector<TRow> _rows;				// ガチャの行データ
	std::mt19937 _rng;						// ガチャの乱数生成器
	std::vector<std::string> _resultLines;	// ガチャの結果の行データ
	bool _hasResult = false;				// ガチャの結果があるかどうか
};