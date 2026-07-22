#pragma once
#include "container.h"

namespace SqliteConfig
{
	// SQLiteデータベースのパスを保持する変数
	inline std::string g_SqliteDbPath = "f.sqlite3";

	// SQLiteデータベースのパスを取得する関数
	inline const char* GetSqliteDbPath()
	{
		return g_SqliteDbPath.c_str();
	}

	// SQLiteデータベースのパスを設定する関数
	inline void SetSqliteDbPath(const std::string& path)
	{
		g_SqliteDbPath = path;
	}
}