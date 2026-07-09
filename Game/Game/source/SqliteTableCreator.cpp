#include "SqliteTableCreator.h"

// データベースにテーブルを作成する
bool CreateSqliteTables(sqlite3* dbh)
{
	int ret = -1;
	int err = 0;

	// メインステータス
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS basic_status ("
			"StatusName TEXT PRIMARY KEY, "
			"probability REAL, "
			"Val1 REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	// サブステータス
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS status ("
			"StatusName TEXT PRIMARY KEY, "
			"probability REAL, "
			"Val1 REAL, Val2 REAL, Val3 REAL, Val4 REAL, Val5 REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	// 装備
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS armor ("
			"ArmorName TEXT PRIMARY KEY, "
			"probability REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	// 保存された装備
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS save_equipment ("
			"part INTEGER PRIMARY KEY, "
			"hasResult INTEGER, "
			"armorName TEXT, "
			"basicStatusLines TEXT, "
			"statusLines TEXT"
			");",
			NULL, NULL, &errorMessage);
	}

	// キャラの基礎ステータス
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS chara_status ("
			"StatusName TEXT PRIMARY KEY, "
			"Val1 REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	//キャラの装備後最終ステータス
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS chara_afterstatus ("
			"StatusName TEXT PRIMARY KEY, "
			"Val REAL, "
			"hasResult INTEGER"
			");",
			NULL, NULL, &errorMessage);
	}

	// ゲージの初期データ
	if(err == 0) 
	{
		char* errorMessage;
		ret=sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS gauge("
			"part INTEGER PRIMARY KEY, "
			"Id INTEGER,"
			"target_x INTEGER,"
			"target_w INTEGER,"
			"speed REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	// 円の初期データ
	if(err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS circle ("
			"Id TEXT PRIMARY KEY, "
			"min_x INTEGER,"
			"max_x INTEGER,"
			"min_y INTEGER,"
			"max_y INTEGER,"
			"radius INTEGER,"
			"count INTEGER"
			");",
			NULL, NULL, &errorMessage);
	}

	// 速度とそれに応じたボーナスの対応表
	if(err == 0) 
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS speed_list ("
			"min_speed INTEGER PRIMARY KEY,"
			"max_speed INTEGER,"
			"BonusTime REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	// 敵の基礎データ
	if(err == 0) 
	{
		char* errorMessage;
		ret=sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS enemybase("
			"EnemyName TEXT PRIMARY KEY,"
			"HP REAL,"
			"Attack REAL,"
			"Lv TEXT,"
			"StartLevel INTEGER,"
			"LevelBonus REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	// キャラのダメージ計算式
	if(err == 0) 
	{
		char* errorMessage;
		ret=sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS charaFormulas("
			"FormulaName TEXT PRIMARY KEY,"
			"Formula TEXT,"
			"gaugeSuccess REAL,"
			"gaugeFail REAL"
			");",
			NULL, NULL, &errorMessage);
	}

	// アカウントデータ
	if(err == 0)
	{
		char* errorMessage;
		ret=sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS AccountData("
			"UID INTEGER PRIMARY KEY,"
			"Level INTEGER,"
			"Exp INTEGER,"
			"Coin INTEGER,"
			"ClearCount INTEGER,"
			"gachaCount INTEGER"
			");",
			NULL, NULL, &errorMessage);
	}

	return err == 0;
}