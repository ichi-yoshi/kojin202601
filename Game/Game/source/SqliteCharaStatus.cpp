#include "SqliteCharaStatus.h"
#include "SqliteUtill.h"
#include "SqliteTextUtill.h"
#include <cstdlib>

namespace
{
	void ApplyCharaStatus(CharaStatus& status, const std::string& name, double value)
	{
		if (name == "HP") { status.hp = value; }
		else if (name == "攻撃") { status.attack = value; }
		else if (name == "防御") { status.defense = value; }
		else if (name == "HP%") { status.hpPercent = value; }
		else if (name == "攻撃%") { status.attackPercent = value; }
		else if (name == "防御%") { status.defensePercent = value; }
		else if (name == "会心率") { status.critRate = value; }
		else if (name == "会心ダメージ") { status.critDamage = value; }
		else if (name == "速度") { status.speed = value; }
		else if (name == "運値") { status.luck = value; }
		else if (name == "Poop") { status.poop = value; }
	}
}

struct CharaStatusContext
{
	CharaStatus* status = nullptr;
};

static int CharaStatusCallback(void* param, int col_cnt, char** row_txt, char**)
{
	if (!param || col_cnt < 2) { return 0; }
	auto* ctx = static_cast<CharaStatusContext*>(param);

	const std::string rawName = row_txt[0] ? row_txt[0] : "";
	const std::string name = SqliteTextUtill::FromUtf8(rawName);
	const double value = row_txt[1] ? std::atof(row_txt[1]) : 0.0;

	ApplyCharaStatus(*ctx->status, name, value);
	return 0;
}

bool LoadCharaBaseStatusSqlite(CharaStatus& outStatus, std::string* outError)
{
	outStatus = CharaStatus{};

	sqlite3* dbh = nullptr;
	if (!OpenSqliteConnection(&dbh, outError)) { return false; }

	CharaStatusContext ctx{ &outStatus };

	char* errorMessage;
	int ret = sqlite3_exec(dbh,
		"SELECT StatusName, Val1 FROM chara_status;",
		CharaStatusCallback, &ctx, &errorMessage);

	if (ret != SQLITE_OK && outError)
	{
		*outError = errorMessage ? errorMessage : "SQLite query failed";
	}

	sqlite3_free(errorMessage);
	sqlite3_close(dbh);
	return ret == SQLITE_OK;
}
