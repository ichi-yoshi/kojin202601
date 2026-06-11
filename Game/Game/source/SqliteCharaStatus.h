#pragma once
#include <string>
#include "CharaBase.h"

// SQLiteからキャラの基礎ステータスを読み込む
bool LoadCharaBaseStatusSqlite(CharaStatus& outStatus, std::string* outError = nullptr);