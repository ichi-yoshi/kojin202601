#pragma once
#include "container.h"

//inline constexpr const char* kSqliteDbPath = "f.sqlite3";
namespace SqliteConfig
{
	inline std::string g_SqliteDbPath = "f.sqlite3";
	inline const char* GetSqliteDbPath()
	{
		return g_SqliteDbPath.c_str();
	}
	inline void SetSqliteDbPath(const std::string& path)
	{
		g_SqliteDbPath = path;
	}
}