#pragma once
#include <vector>
#include <string>
#include <random>

class SqliteSpeed
{
public:
	// ゲージの初期データ
	struct SpeedRow
	{
	    double minSpeed;
	    double maxSpeed;
		double bonusTime;
	};

	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	// 現在の速度ステータスから、該当するボーナスタイムを取得する
	double GetBonusTime(double currentSpeed) const;

	bool LoadSpeedSqlite(std::vector<SpeedRow>& outRows, std::string* outError);

	std::vector<SpeedRow> _rows;
};

