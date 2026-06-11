#pragma once
#include <string>
#include <vector>
#include <random>

template <typename TRow>
class GachaBase
{
public:
	bool Initialize(const std::string& Path, std::string* outError = nullptr);
	bool Roll(int count);

	bool HasResult() const { return _hasResult; }
	void ClearResult();
	const std::vector<std::string>& GetResultLines() const { return _resultLines; }

protected:
	virtual bool LoadRows(std::vector<TRow>& outRows, std::string* outError) = 0;
	virtual std::string MakeResultLine(const TRow& row, std::mt19937& rng) = 0;

private:
	std::vector<TRow> _rows;				// ガチャの行データ
	std::mt19937 _rng;						// ガチャの乱数生成器
	std::vector<std::string> _resultLines;	// ガチャの結果の行データ
	bool _hasResult = false;
};