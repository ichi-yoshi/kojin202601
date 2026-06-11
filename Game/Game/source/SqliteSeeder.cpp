#include "SqliteSeeder.h"
#include "SqliteTextUtill.h"
#include <vector>
#include <cstdio>

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

	// 既存データ削除
	if(err == 0)
	{
		char* errorMessage = nullptr;
		const char* sql =
			"BEGIN;"
			"DELETE FROM basic_status;"
			"DELETE FROM status;"
			"DELETE FROM armor;"
			"DELETE FROM chara_status;"
			"DELETE FROM gauge;"
			"DELETE FROM circle;"
			"DELETE FROM speed_list;"
			"COMMIT;";

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

	if(err == 0)
	{
		InsertSeeds(dbh, basicSeeds, "basic_status",
			[](char* sql, size_t size, const BasicStatusSeed& s)
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size,
					"INSERT INTO basic_status (StatusName, probability, Val1) VALUES ('%s', %.3f, %.3f);",
					name.c_str(), s.probability, s.val1);
			}, err);
	}

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

	if(err == 0)
	{
		InsertSeeds(dbh, statusSeeds, "status",
			[](char* sql, size_t size, const StatusSeed& s)
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size,
					"INSERT INTO status (StatusName, probability, Val1, Val2, Val3, Val4, Val5) "
					"VALUES ('%s', %.3f, %.3f, %.3f, %.3f, %.3f, %.3f);",
					name.c_str(), s.probability, s.val1, s.val2, s.val3, s.val4, s.val5);
			}, err);
	}

	// 装備（装備名、確率）
	const std::vector<ArmorSeed> armorSeeds =
	{
		{"頭", 25},
		{"胴", 25},
		{"腕", 25},
		{"靴", 25}
	};

	if(err == 0)
	{
		InsertSeeds(dbh, armorSeeds, "armor",
			[](char* sql, size_t size, const ArmorSeed& s)
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size,
					"INSERT INTO armor (ArmorName, probability) VALUES ('%s', %.3f);",
					name.c_str(), s.probability);
			}, err);
	}

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

	if(err == 0)
	{
		InsertSeeds(dbh, charaStatusSeeds, "chara_status",
			[](char* sql, size_t size, const CharaStatusSeed& s)
			{
				std::string name = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.name));
				snprintf(sql, size,
					"INSERT INTO chara_status (StatusName, Val1) VALUES ('%s', %.3f);",
					name.c_str(), s.val1);
			}, err);
	}

	// ゲージの初期データ（ゲージID、目標X座標、目標幅、移動速度）
	const std::vector<GaugeSeed> gaugeSeeds =
	{
		{"gauge1", 100, 200, 50.0},
		{"gauge2", 150, 150, 70.0},
		{"gauge3", 200, 100, 90.0}
	};

	if(err == 0)
	{
		InsertSeeds(dbh, gaugeSeeds, "gauge",
			[](char* sql, size_t size, const GaugeSeed& s)
			{
				std::string id = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.id));
				snprintf(sql, size,
					"INSERT INTO gauge (id, target_x, target_w, speed) VALUES ('%s', %d, %d, %.3f);",
					id.c_str(), s.target_x, s.target_w, s.speed);
			}, err);
	}

	const std::vector<CircleSeed> circleSeeds =
	{
		{ "circle1", 530, 870, 130, 470, 30, 5 },
		{ "circle2", 540, 860, 140, 460, 40, 7 },
		{ "circle3", 550, 850, 150, 450, 50, 10 }
	};
	
	if(err==0)
	{
		InsertSeeds(dbh, circleSeeds, "circle",
			[](char* sql, size_t size, const CircleSeed& s)
			{
				std::string id = SqliteTextUtill::EscapeSqlString(SqliteTextUtill::ToUtf8(s.id));
				snprintf(sql, size,
					"INSERT INTO circle (Id, min_x, max_x, min_y, max_y, radius, count) VALUES ('%s', %d, %d, %d, %d, %d, %d);",
					id.c_str(), s.minX, s.maxX, s.minY, s.maxY, s.radius, s.count);
			}, err);
	}

	const std::vector<SpeedSeed> speedSeeds =
	{
		{   0.0,  69.0, 0.0 },
		{  70.0,  80.0, 0.4 },
		{  81.0,  90.0, 0.8 },
		{  91.0, 100.0, 1.2 },
		{ 101.0, 110.0, 1.6 },
		{ 111.0, 120.0, 2.0 },
		{ 121.0, 130.0, 2.4 },
		{ 131.0, 140.0, 2.8 },
		{ 141.0, 150.0, 3.0 },
		{ 151.0, 160.0, 3.2 },
		{ 161.0, 170.0, 3.4 },
		{ 171.0, 180.0, 3.6 },
		{ 181.0, 190.0, 3.8 },
		{ 191.0, 200.0, 4.0 },
		{ 201.0, 210.0, 4.1 },
		{ 211.0, 220.0, 4.2 },
		{ 221.0, 230.0, 4.3 },
		{ 231.0, 240.0, 4.4 },
		{ 241.0, 250.0, 4.5 },
		{ 251.0, 260.0, 4.6 },
		{ 261.0, 270.0, 4.7 },
		{ 271.0, 280.0, 4.8 }
	};

	if(err == 0)
	{
		InsertSeeds(dbh, speedSeeds, "speed_list",
			[](char* sql, size_t size, const SpeedSeed& s)
			{
				snprintf(sql, size,
					"INSERT INTO speed_list (min_speed, max_speed, BonusTime) VALUES (%.3f, %.3f, %.3f);",
					s.minSpeed, s.maxSpeed, s.bonusTime);
			}, err);
	}

	return err == 0;
}