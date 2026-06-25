#pragma once
#include "container.h"

// SQLiteのテキストデータを扱うためのユーティリティクラス
class SqliteTextUtill
{
public:
	// 複数行のテキストを'|'区切りの1行テキストに変換する
	static std::string JoinLines(const std::vector<std::string>& lines);

	// '|'区切りのテキストを複数行のテキストに変換する
	static void SplitLines(const std::string& text, std::vector<std::string>& outLines);

	// '|'区切りのテキストを複数行のテキストに変換する
	static std::string EscapeSqlString(const std::string& text);

	// CP932 -> UTF-8
	static std::string ToUtf8(const std::string& text);

	// UTF-8 -> CP932
	static std::string FromUtf8(const std::string& text);
};

