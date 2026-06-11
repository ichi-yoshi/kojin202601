#pragma once
#include <vector>
#include <string>
#include <random>

struct GaugeRow
{
	std::string id;		// ゲージID
	int targetX;		// 目標X座標
	int targetW;		// 目標幅
	double speed;		// 移動速度
};

struct GaugeContext 
{
	std::vector<GaugeRow>* rows = nullptr;
};

class SqliteGauge
{
public:
	bool Initialize(const std::string& Path, std::string* outError);

	bool RollrandomGauge();

	bool GetGauge(GaugeRow& outRow);

private:
	std::vector<GaugeRow> _rows;		// ゲージの行データ
	std::mt19937 _rng;					// 乱数生成器
	GaugeRow _currentGauge;				// 現在のゲージデータ
	bool _hasGauge = false;				// ゲージデータがあるかどうか

	bool LoadGaugeSqlite(std::vector<GaugeRow>& outRows, std::string* outError);
	
};

