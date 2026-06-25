#pragma once
#include "container.h"

struct CircleRow
{
	std::string id;			// 円ID
	int minX;
	int maxX;
	int minY;
	int maxY;
	int radius;
	int count;
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

