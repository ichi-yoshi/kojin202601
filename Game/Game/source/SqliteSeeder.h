#pragma once
#include "../sqlite3/sqlite3.h"
#include <vector>
#include <cstddef>

//シードデータ
//メインステータス
struct BasicStatusSeed
{
	const char* name;		// ステータス名
	double probability;		// 確率
	double val1;			// 値
};

//サブステータス
struct StatusSeed
{
	const char* name;		// ステータス名
	double probability;		// 確率
	double val1;			// 値1(1～5を抽選する)
	double val2;			// 値2
	double val3;			// 値3
	double val4;			// 値4
	double val5;			// 値5
};

//装備
struct ArmorSeed
{
	const char* name;		// 装備名
	double probability;		// 確率
};

//キャラの基礎ステータス
struct CharaStatusSeed
{
	const char* name;		// ステータス名
	double val1;			// 値
};

//ゲージの初期データ
struct GaugeSeed 
{
	const char* id;		// ゲージID
	int target_x;		// ゲージの目標X座標
	int target_w;		// ゲージの目標幅
	double speed;		// ゲージの移動速度
};

//円の初期データ
struct CircleSeed 
{
	const char* id;			// 円ID
	int minX;
	int maxX;
	int minY;
	int maxY;
	int radius;
	int count;
};

struct SpeedSeed
{
	double minSpeed;
	double maxSpeed;
	double bonusTime;
};

// SQLiteに初期データを投入する
bool SeedSqliteData(sqlite3* dbh);

// SQLを実行するユーティリティ関数
static bool ExecSql(sqlite3* dbh, const char* sql, const char* label, int& err);

// シードデータのベクターからSQLを生成して実行するユーティリティ関数
template <typename T, typename BuildSql>
static bool InsertSeeds(sqlite3* dbh, const std::vector<T>& seeds, const char* label, BuildSql build, int& err);
