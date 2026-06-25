#include "SqliteCircle.h"
#include "SqliteUtill.h"

bool SqliteCircle::Initialize(const std::string& Path, std::string* outError)
{
	// 乱数生成器にシードを設定
	_rng = std::mt19937(std::random_device{}());
	// データベースから全行を読み込んでクラスのメンバ変数 _rows に保存する
	return LoadCircleSqlite(_rows, outError);
}

static int CircleCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if(!param || col_cnt < 7) { return 0; }
	auto* ctx = static_cast<CircleContext*>(param);
	CircleRow row;
	row.id = row_txt[0] ? row_txt[0] : "";
	row.minX = row_txt[1] ? std::atoi(row_txt[1]) : 0;
	row.maxX = row_txt[2] ? std::atoi(row_txt[2]) : 0;
	row.minY = row_txt[3] ? std::atoi(row_txt[3]) : 0;
	row.maxY = row_txt[4] ? std::atoi(row_txt[4]) : 0;
	row.radius = row_txt[5] ? std::atoi(row_txt[5]) : 0;
	row.count = row_txt[6] ? std::atoi(row_txt[6]) : 0;
	ctx->rows->push_back(row);
	return 0;
}

bool SqliteCircle::LoadCircleSqlite(std::vector<CircleRow>& outRows, std::string* outError)
{
	outRows.clear();
	sqlite3* dbh = nullptr;
	if(!OpenSqliteConnection(&dbh, outError)) { return false; }
	CircleContext ctx{ &outRows };
	char* errorMessage = nullptr;
	int ret = sqlite3_exec(dbh,
		"SELECT id, min_x, max_x, min_y, max_y, radius, count FROM circle;",
		CircleCallback, &ctx, &errorMessage);
	if(ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}
	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}

bool SqliteCircle::RollrandomCircle()
{
	if(_rows.empty())
	{
		_hasCircle = false;
		return false;
	}
	std::uniform_int_distribution<int> dist(0, static_cast<int>(_rows.size() - 1));
	int index = dist(_rng);
	_currentCircle = _rows[index];
	_hasCircle = true;
	return true;
}

bool SqliteCircle::GetCircle(CircleRow& outRow)
{
	if(!_hasCircle) { return false; }
	outRow = _currentCircle;
	return true;
}


