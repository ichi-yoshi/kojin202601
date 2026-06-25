#include "SqliteSeeder.h"
#include "SqliteTextUtill.h"

// SQLを実行するユーティリティ関数
static bool ExecSql(sqlite3* dbh, const char* sql, const char* label, int& err)
{
	char* errorMessage = nullptr;
	int ret = sqlite3_exec(dbh, sql, nullptr, nullptr, &errorMessage);
	if(ret == SQLITE_OK)
	{
		printf("%s追加：成功\n", label);
	}
	else
	{
		printf("%s追加：失敗(%d)\n", label, ret);
		err = 1;
	}
	if(errorMessage) { sqlite3_free(errorMessage); }
	return ret == SQLITE_OK;
}

// 共通のseed挿入
template <typename T, typename BuildSql>
static bool InsertSeeds(sqlite3* dbh, const std::vector<T>& seeds, const char* label, BuildSql build, int& err)
{
	for(const auto& s : seeds)
	{
		char sql[512];
		build(sql, sizeof(sql), s);
		if(!ExecSql(dbh, sql, label, err))
		{
			return false;
		}
	}
	return true;
}

// データベースに初期データを投入する
bool SeedSqliteData(sqlite3* dbh)
{
	int ret = -1;
	int err = 0;

	char* errorMessage = nullptr;

	// 最初にトランザクションを開始
	ret = sqlite3_exec(dbh, "BEGIN TRANSACTION;", NULL, NULL, &errorMessage);
	if(ret != SQLITE_OK)
	{
		printf("トランザクション開始失敗\n");
		if(errorMessage) sqlite3_free(errorMessage);
		return false;
	}

	// 既存データ削除
	if(err == 0)
	{
		const char* sql =
			"DELETE FROM basic_status;"
			"DELETE FROM status;"
			"DELETE FROM armor;"
			"DELETE FROM chara_status;"
			"DELETE FROM gauge;"
			"DELETE FROM circle;"
			"DELETE FROM speed_list;"
			"DELETE FROM enemybase;"
			"DELETE FROM CharaFormulas;"
			"DELETE FROM AccountData;";

		ret = sqlite3_exec(dbh, sql, NULL, NULL, &errorMessage);
		if(ret != SQLITE_OK)
		{
			printf("既存データ削除：失敗(%d)\n", ret);
			err = 1;
		}
		if(errorMessage) { sqlite3_free(errorMessage); }
	}

	// メインステータス（ステータス名、確率、値）
	const std::vector<BasicStatusSeed> basicSeeds =
	{
		{"HP%", 10, 42.0},
		{"HP", 11, 1300},
		{"攻撃%", 5, 37.0},
		{"攻撃", 8, 1300},
		{"防御%", 10, 48.0},
		{"防御", 10, 1300},
		{"会心率", 8, 35.0},
		{"会心ダメージ", 8, 73.0},
		{"速度", 6, 22},
		{"運値", 10, 23.0},
		{"Poop", 14, 1114}
	};

	// サブステータス（ステータス名、確率、値1～5）
	const std::vector<StatusSeed> statusSeeds =
	{
		{"HP%", 10, 7.0, 8.0, 9.0, 10.0, 11.0},
		{"HP", 11, 300, 400, 500, 600, 700},
		{"攻撃%", 5, 7.0, 8.0, 9.0, 10.0, 11.0},
		{"攻撃", 8, 30, 40, 50, 60, 70},
		{"防御%", 10, 8.0, 9.0, 10.0, 11.0, 12.0},
		{"防御", 10, 300, 400, 500, 600, 700},
		{"会心率", 8, 5.0, 6.0, 8.0, 10.0, 14.0},
		{"会心ダメージ", 8, 13.0, 15.0, 17.0, 20.0, 23.0},
		{"速度", 6, 2, 5, 7, 9, 12},
		{"運値", 10, 3.0, 4.0, 5.0, 7.0, 9.0},
		{"Poop", 14, 111, 367, 555, 666, 822}
	};

	// 装備（装備名、確率）
	const std::vector<ArmorSeed> armorSeeds =
	{
		{"頭", 25},
		{"胴", 25},
		{"腕", 25},
		{"靴", 25}
	};

	// キャラの基礎ステータス（ステータス名、値）
	const std::vector<CharaStatusSeed> charaStatusSeeds =
	{
		{"レベル",1},
		{"HP", 1000},
		{"攻撃", 1000},
		{"防御", 1000},
		{"会心率", 5.0},
		{"会心ダメージ", 50.0},
		{"速度", 70},
		{"運値", 100},
		{"Poop",0 }
	};

	// ゲージの初期データ（ゲージID、目標X座標、目標幅、移動速度）
	const std::vector<GaugeSeed> gaugeSeeds =
	{
		{"gauge1", 100, 200, 20.0},
		{"gauge2", 150, 150, 20.0},
		{"gauge3", 200, 100, 20.0},
		{"gauge4", 250,  50, 10.0},
		{"gauge5", 300,  35, 20.0}
	};

	// サークルの初期データ（サークルID、最小X座標、最大X座標、最小Y座標、最大Y座標、半径、出現数）
	const std::vector<CircleSeed> circleSeeds =
	{
		{ "circle1", 530, 870, 130, 470, 30, 5 },
		{ "circle2", 540, 860, 140, 460, 40, 7 },
		{ "circle3", 550, 850, 150, 450, 50, 3 },
		{ "circle4", 560, 840, 160, 440, 60, 8 },
		{ "circle5", 570, 830, 170, 430, 30, 4 }
	};

	// 速度の初期データ（最小速度、最大速度、ボーナスタイム）
	const std::vector<SpeedSeed> speedSeeds =
	{
		{   0.0,  70.0, 0.0 },
		{  71.0,  90.0, 0.4 },
		{  91.0, 100.0, 0.8 },
		{ 101.0, 120.0, 1.2 },
		{ 121.0, 140.0, 1.6 },
		{ 141.0, 160.0, 2.0 },
		{ 161.0, 180.0, 2.4 },
		{ 181.0, 200.0, 2.8 },
		{ 201.0, 220.0, 3.0 },
		{ 221.0, 240.0, 3.2 },
		{ 241.0, 260.0, 3.4 },
		{ 261.0, 280.0, 3.6 },
		{ 281.0, 300.0, 3.8 },
		{ 301.0, 320.0, 4.0 },
		{ 321.0, 340.0, 4.2 },
		{ 341.0, 350.0, 4.4 }
	};

	// 敵の基礎データ（敵の名前、HP、攻撃、レベル表記、開始レベル、レベルボーナス倍率）
	const std::vector<EnemyBaseSeed> enemybaseSeeds =
	{
		{"ゴエポン", 200000.0, 1000.0, "Lv.", 10, 2.0}
	};

	// 式の初期データ（式の名前、式の内容、ゲージ成功時の値、ゲージ失敗時の値）
	const std::vector<CharaFormulasSeed> charaFormulasSeeds =
	{
		{"敵防御倍率", "(100+キャラレベル)/((100+キャラレベル)+(99+敵レベル))",0.0,0.0},
		{"ダメージ減衰率","1+(Poop/5000)",0.0,0.0},
		{"最終ダメージ","(攻撃*敵防御倍率*ダメージ減衰率)*会心倍率*運値倍率",1.3,0.2},
		{"敵の攻撃ダメージ","(キャラ最大HP*0.02)+敵攻撃力*(敵攻撃力/キャラ防御力)",0.0,0.0}
	};

	// デフォルトのアカウントデータ（uid、level、exp、coin、ClearCount、GachaCount）
	const std::vector<AccountDataSeed> accountDataSeeds =
	{
		{ 0, 1, 0, 30000, 0, 0 }
	};

	// データ挿入
	if(err == 0)
	{
		InsertSeeds(dbh, basicSeeds, "basic_status",
			[](char* sql, size_t size, const BasicStatusSeed& s) 
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size, "INSERT INTO basic_status (StatusName, probability, Val1) VALUES ('%s', %.3f, %.3f);",
					name.c_str(), s.probability, s.val1);
			}, err);
	}

	if(err == 0)
	{
		InsertSeeds(dbh, statusSeeds, "status",
			[](char* sql, size_t size, const StatusSeed& s) 
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size, "INSERT INTO status (StatusName, probability, Val1, Val2, Val3, Val4, Val5) VALUES ('%s', %.3f, %.3f, %.3f, %.3f, %.3f, %.3f);",
					name.c_str(), s.probability, s.val1, s.val2, s.val3, s.val4, s.val5);
			}, err);
	}

	if(err == 0)
	{
		InsertSeeds(dbh, armorSeeds, "armor",
			[](char* sql, size_t size, const ArmorSeed& s) 
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size, "INSERT INTO armor (ArmorName, probability) VALUES ('%s', %.3f);",
					name.c_str(), s.probability);
			}, err);
	}

	if(err == 0)
	{
		InsertSeeds(dbh, charaStatusSeeds, "chara_status",
			[](char* sql, size_t size, const CharaStatusSeed& s) 
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size, "INSERT INTO chara_status (StatusName, Val1) VALUES ('%s', %.3f);",
					name.c_str(), s.val1);
			}, err);
	}

	if(err == 0)
	{
		InsertSeeds(dbh, gaugeSeeds, "gauge",
			[](char* sql, size_t size, const GaugeSeed& s) 
			{
				std::string id = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.id));
				snprintf(sql, size, "INSERT INTO gauge (id, target_x, target_w, speed) VALUES ('%s', %d, %d, %.3f);",
					id.c_str(), s.target_x, s.target_w, s.speed);
			}, err);
	}

	if(err == 0)
	{
		InsertSeeds(dbh, circleSeeds, "circle",
			[](char* sql, size_t size, const CircleSeed& s) 
			{
				std::string id = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.id));
				snprintf(sql, size, "INSERT INTO circle (Id, min_x, max_x, min_y, max_y, radius, count) VALUES ('%s', %d, %d, %d, %d, %d, %d);",
					id.c_str(), s.minX, s.maxX, s.minY, s.maxY, s.radius, s.count);
			}, err);
	}

	if(err == 0)
	{
		InsertSeeds(dbh, speedSeeds, "speed_list",
			[](char* sql, size_t size, const SpeedSeed& s) 
			{
				snprintf(sql, size, "INSERT INTO speed_list (min_speed, max_speed, BonusTime) VALUES (%.3f, %.3f, %.3f);", 
					s.minSpeed, s.maxSpeed, s.bonusTime);
			}, err);
	}

	if(err == 0) 
	{
		InsertSeeds(dbh, enemybaseSeeds, "enemybase",
			[](char* sql, size_t size, const EnemyBaseSeed& s)
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.enemyName));
				snprintf(sql, size, "INSERT INTO enemybase (EnemyName, HP, Attack, Lv, StartLevel, LevelBonus) VALUES ('%s', %.3f, %.3f, '%s', %d, %.3f);",
					name.c_str(), s.hp, s.attack, s.lv, s.startLevel, s.levelBonus);
			}, err);
	}

	if(err==0)
	{
		InsertSeeds(dbh, charaFormulasSeeds, "charaFormulas",
			[](char* sql, size_t size, const CharaFormulasSeed& s)
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.formulaName));
				std::string formula = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.formula));
				snprintf(sql, size, "INSERT INTO charaFormulas (FormulaName, Formula, gaugeSuccess, gaugeFail) VALUES ('%s', '%s', %.3f, %.3f);",
					name.c_str(), formula.c_str(), s.gaugeSuccess, s.gaugeFail);
			}, err);
	}

	if(err == 0) 
	{
		InsertSeeds(dbh, accountDataSeeds, "AccountData",
			[](char* sql, size_t size, const AccountDataSeed& s)
			{
				snprintf(sql, size, "INSERT OR IGNORE INTO AccountData(uid, level, exp, coin, ClearCount) VALUES (%d, %d, %d, %d, %d);",
					s.uid, s.level, s.exp, s.coin, s.clearcount);
			}, err);
	}

	if(err == 0)
	{
		ret = sqlite3_exec(dbh, "COMMIT;", NULL, NULL, &errorMessage);
		if(ret != SQLITE_OK) 
		{
			printf("コミット失敗\n");
			err = 1;
		}
	}
	else
	{
		// 途中でエラーがあった場合はロールバック
		sqlite3_exec(dbh, "ROLLBACK;", NULL, NULL, nullptr);
	}

	if(errorMessage) { sqlite3_free(errorMessage); }
	return err == 0;
}