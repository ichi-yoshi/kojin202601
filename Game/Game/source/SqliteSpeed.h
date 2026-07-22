#pragma once
#include "container.h"

class SqliteSpeed
{
public:
	// ゲージの初期データ
	struct SpeedRow
	{
		double minSpeed;	// 最小速度
		double maxSpeed;	// 最大速度
		double bonusTime;	// ボーナスタイム
	};

	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	// 現在の速度ステータスから、該当するボーナスタイムを取得する
	double GetBonusTime(double currentSpeed) const;

	bool LoadSpeedSqlite(std::vector<SpeedRow>& outRows, std::string* outError);

	std::vector<SpeedRow> _rows;
};

