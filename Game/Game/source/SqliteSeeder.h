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
	const char* id;		// ID
	int target_x;		// 目標X座標
	int target_w;		// 目標幅
	double speed;		// 移動速度
};

//円の初期データ
struct CircleSeed 
{
	const char* id;		// ID
	int minX;			// 生成範囲の最小X座標
	int maxX;			//			 最大X座標
	int minY;			//			 最小Y座標
	int maxY;			//			 最大Y座標
	int radius;			// 半径
	int count;			// 生成する円の数
};

//速度の初期データ
struct SpeedSeed
{
	double minSpeed;	// 最小速度
	double maxSpeed;	// 最大速度
	double bonusTime;	// ボーナスタイム
};

//敵の基礎データ
struct EnemyBaseSeed
{
	const char* enemyName;	// 敵の名前
	double hp;				// HP
	double attack;			// 攻撃
	const char* lv;			// レベル表記
	int startLevel;			// 開始レベル
	double levelBonus;		// レベルボーナス倍率
};

// キャラのダメージ計算式の初期データ
struct CharaFormulasSeed 
{
	const char* formulaName;	// 式の名前
	const char* formula;		// 式の内容
	double gaugeSuccess;		// ゲージ成功時の値
	double gaugeFail;			// ゲージ失敗時の値
};

struct AccountDataSeed
{
	int uid;			// ユーザーID
	int level;			// プレイヤーレベル
	int exp;			// 経験値
	int coin;			// コイン
	int clearcount;		// 敵レベル
	int gachaCount;		// ガチャ回数
};

// SQLiteに初期データを投入する
bool SeedSqliteData(sqlite3* dbh);

// SQLを実行するユーティリティ関数
static bool ExecSql(sqlite3* dbh, const char* sql, const char* label, int& err);

// シードデータのベクターからSQLを生成して実行するユーティリティ関数
template <typename T, typename BuildSql>
static bool InsertSeeds(sqlite3* dbh, const std::vector<T>& seeds, const char* label, BuildSql build, int& err);
