#pragma once
#include "container.h"

struct CircleRow
{
	std::string id;		// 円ID
	int minX;			// 円の最小X座標
	int maxX;			// 円の最大X座標
	int minY;			// 円の最小Y座標
	int maxY;			// 円の最大Y座標
	int radius;			// 円の半径
	int count;			// 円の点の数
};

struct CircleContext
{
	std::vector<CircleRow>* rows = nullptr;
};

class SqliteCircle
{
public:
	bool Initialize(const std::string& Path, std::string* outError);
	bool RollrandomCircle();
	bool GetCircle(CircleRow& outRow);

private:
	std::vector<CircleRow> _rows;		// 円の行データ
	std::mt19937 _rng;					// 乱数生成器
	CircleRow _currentCircle;			// 現在の円データ
	bool _hasCircle = false;			// 円データがあるかどうか
	bool LoadCircleSqlite(std::vector<CircleRow>& outRows, std::string* outError);
};

