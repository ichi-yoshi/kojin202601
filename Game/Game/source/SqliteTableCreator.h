#pragma once
#include "../sqlite3/sqlite3.h"

// SQLiteのテーブルを生成する
bool CreateSqliteTables(sqlite3* dbh);