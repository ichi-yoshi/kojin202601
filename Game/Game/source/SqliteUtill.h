#pragma once
#include <string>
#include "../sqlite3/sqlite3.h"

//	SQLiteのデータベースに接続する
bool OpenSqliteConnection(sqlite3** outDb, std::string* outError = nullptr);