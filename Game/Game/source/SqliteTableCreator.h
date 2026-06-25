#pragma once
#include "SqliteUtill.h"
#include "container.h"

// SQLiteのテーブルを生成する
bool CreateSqliteTables(sqlite3* dbh);