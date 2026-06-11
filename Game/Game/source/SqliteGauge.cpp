#include "SqliteGauge.h"
#include "SqliteUtill.h"
#include <cstdlib>

bool SqliteGauge::Initialize(const std::string& Path, std::string* outError)
{
    // 乱数生成器にシードを設定
    _rng = std::mt19937(std::random_device{}());

    // データベースから全行を読み込んでクラスのメンバ変数 _rows に保存する
    return LoadGaugeSqlite(_rows, outError);
}

static int GaugeCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if (!param || col_cnt < 4) { return 0; }
	auto* ctx = static_cast<GaugeContext*>(param);

	GaugeRow row;
    row.id = row_txt[0] ? row_txt[0] : "";
	row.targetX = row_txt[1] ? std::atoi(row_txt[1]) : 0;
	row.targetW = row_txt[2] ? std::atoi(row_txt[2]) : 0;
	row.speed = row_txt[3] ? std::atof(row_txt[3]) : 0.0;

	ctx->rows->push_back(row);
	return 0;
}

bool SqliteGauge::LoadGaugeSqlite(std::vector<GaugeRow>& outRows, std::string* outError) 
{
    outRows.clear();

    sqlite3* dbh = nullptr;
    if (!OpenSqliteConnection(&dbh, outError)) { return false; }

    GaugeContext ctx{ &outRows };

    char* errorMessage = nullptr;
    // SQL文でId, target_x, target_w, speed を指定（partは除外）
    int ret = sqlite3_exec(dbh,
        "SELECT id, target_x, target_w, speed FROM gauge;",
        GaugeCallback, &ctx, &errorMessage);

    if (ret != SQLITE_OK && outError)
    {
        *outError = errorMessage ? errorMessage : "SQLite query failed";
    }

    sqlite3_free(errorMessage);
    sqlite3_close(dbh);
    return ret == SQLITE_OK;
}

bool SqliteGauge::RollrandomGauge() 
{
    if (_rows.empty())
    {
        _hasGauge = false;
        return false;
    }

    // 0 ～ _rows.size()-1 の間でランダムなインデックスを抽選
    std::uniform_int_distribution<size_t> dist(0, _rows.size() - 1);
    _currentGauge = _rows[dist(_rng)];
    _hasGauge = true;
    return true;
}

bool SqliteGauge::GetGauge(GaugeRow& outRow)
{
	if (!_hasGauge) { return false; }
	outRow = _currentGauge;
	return true;
}