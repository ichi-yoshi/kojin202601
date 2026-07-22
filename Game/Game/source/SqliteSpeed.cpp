#include "SqliteSpeed.h"
#include "SqliteUtill.h"

bool SqliteSpeed::Initialize(const std::string& Path, std::string* outError)
{
	return LoadSpeedSqlite(_rows, outError);
}

static int SpeedCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 3) { return 0; }
	auto* rows = static_cast<std::vector<SqliteSpeed::SpeedRow>*>(param);

	// SQLiteの結果をSpeedRowに変換して保存
	SqliteSpeed::SpeedRow row;
	row.minSpeed = row_txt[0] ? std::atoi(row_txt[0]) : 0;
	row.maxSpeed = row_txt[1] ? std::atoi(row_txt[1]) : 0;
	row.bonusTime = row_txt[2] ? std::atof(row_txt[2]) : 0.0;

	rows->push_back(row);
	return 0;
}

bool SqliteSpeed::LoadSpeedSqlite(std::vector<SpeedRow>& outRows, std::string* outError)
{
	outRows.clear();

	// SQLiteデータベースに接続
	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }

	// SQLiteのクエリを実行してデータを取得
	char* errorMessage = nullptr;
	int ret = sqlite3_exec(dbh,
		"SELECT min_speed, max_speed, BonusTime FROM speed_list;",
		SpeedCallback, &outRows, &errorMessage);

	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite speed query failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

double SqliteSpeed::GetBonusTime(double currentSpeed) const
{
	// 速度を整数にキャストして判定
	int speedVal = static_cast<int>(currentSpeed);

	for(const auto& row : _rows)
	{
		// キャラ速度が min と max の間に収まっているかチェック
		if(speedVal >= row.minSpeed && speedVal <= row.maxSpeed)
		{
			return row.bonusTime;
		}
	}

	// 該当する範囲がなければボーナスは 0.0s
	return 0.0;
}