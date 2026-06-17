#include "SqliteTableCreator.h"
#include <stdio.h>

// データベースにテーブルを作成する
bool CreateSqliteTables(sqlite3* dbh)
{
	int ret = -1;
	int err = 0;

	// メインステータス
	if (err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS basic_status ("
			"StatusName TEXT PRIMARY KEY, "
			"probability REAL, "
			"Val1 REAL"
			");",
			NULL, NULL, &errorMessage);
		if (ret == SQLITE_OK) 
		{
			printf("basic_status生成：成功\n");
		}
		else 
		{
			printf("basic_status生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}

	// サブステータス
	if (err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS status ("
			"StatusName TEXT PRIMARY KEY, "
			"probability REAL, "
			"Val1 REAL, Val2 REAL, Val3 REAL, Val4 REAL, Val5 REAL"
			");",
			NULL, NULL, &errorMessage);
		if (ret == SQLITE_OK)
		{
			printf("status生成：成功\n");
		}
		else
		{
			printf("status生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}

	// 装備
	if (err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS armor ("
			"ArmorName TEXT PRIMARY KEY, "
			"probability REAL"
			");",
			NULL, NULL, &errorMessage);
		if (ret == SQLITE_OK) 
		{
			printf("armor生成：成功\n");
		}
		else 
		{
			printf("armor生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}

	// 保存された装備
	if (err == 0)
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
		if (ret == SQLITE_OK) 
		{
			printf("save_equipment生成：成功\n");
		}
		else 
		{
			printf("save_equipment生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}

	// キャラの基礎ステータス
	if (err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS chara_status ("
			"StatusName TEXT PRIMARY KEY, "
			"Val1 REAL"
			");",
			NULL, NULL, &errorMessage);
		if (ret == SQLITE_OK)
		{
			printf("chara_status生成：成功\n");
		}
		else
		{
			printf("chara_status生成：失敗(%d)\n", ret);
		}
	}

	//キャラの装備後最終ステータス
	if (err == 0)
	{
		char* errorMessage;
		ret = sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS chara_afterstatus ("
			"StatusName TEXT PRIMARY KEY, "
			"Val REAL, "
			"hasResult INTEGER"
			");",
			NULL, NULL, &errorMessage);
		if (ret == SQLITE_OK)
		{
			printf("chara_afterstatus生成：成功\n");
		}
		else
		{
			printf("chara_afterstatus生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}

	// ゲージの初期データ
	if (err == 0) 
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
		if (ret == SQLITE_OK)
		{
			printf("gauge生成：成功\n");
		}
		else
		{
			printf("gauge生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}

	// 円の初期データ
	if (err == 0)
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
		if (ret == SQLITE_OK)
		{
			printf("circle生成：成功\n");
		}
		else
		{
			printf("circle生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}

	// 速度とボーナスタイムの対応表
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
		if(ret == SQLITE_OK)
		{
			printf("speed_list生成：成功\n");
		}
		else
		{
			printf("speed_list生成：失敗(%d)\n", ret);
			//err = 1;
		}
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
		if (ret == SQLITE_OK)
		{
			printf("enemybase生成：成功\n");
		}
		else
		{
			printf("enemybase生成：失敗(%d)\n", ret);
			//err = 1;
		}

	}

	if(err == 0) 
	{
		char* errorMessage;
		ret=sqlite3_exec(dbh,
			"CREATE TABLE IF NOT EXISTS Formulas("
			"FormulaName TEXT PRIMARY KEY,"
			"Formula TEXT,"
			"gaugeSuccess REAL,"
			"gaugeFail REAL"
			");",
			NULL, NULL, &errorMessage);
		if(ret == SQLITE_OK) 
		{
			printf("Formulas生成：成功\n");

		}
		else
		{
			printf("Formulas生成：失敗(%d)\n", ret);
			//err = 1;
		}
	}
	return err == 0;
}