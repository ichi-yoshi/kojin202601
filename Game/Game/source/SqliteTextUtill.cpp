#include "SqliteTextUtill.h"
#include <sstream>
#include <windows.h>

namespace
{
	std::string ConvertCodePage(const std::string& text, UINT fromCP, UINT toCP)
	{
		if (text.empty()) { return {}; }

		int wlen = MultiByteToWideChar(fromCP, 0, text.c_str(), -1, nullptr, 0);
		if (wlen <= 0) { return {}; }

		std::wstring wbuf(static_cast<size_t>(wlen), L'\0');
		MultiByteToWideChar(fromCP, 0, text.c_str(), -1, &wbuf[0], wlen);

		int blen = WideCharToMultiByte(toCP, 0, wbuf.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (blen <= 0) { return {}; }

		std::string buf(static_cast<size_t>(blen), '\0');
		WideCharToMultiByte(toCP, 0, wbuf.c_str(), -1, &buf[0], blen, nullptr, nullptr);

		if (!buf.empty() && buf.back() == '\0') { buf.pop_back(); }
		return buf;
	}
}

// 複数行のテキストを'|'区切りの1行テキストに変換する
std::string SqliteTextUtill::JoinLines(const std::vector<std::string>& lines) 
{
	std::ostringstream os;
	for (size_t i = 0; i < lines.size(); ++i) 
	{
		if (i > 0) { os << "|"; }
		os << lines[i];
	}
	return os.str();
}

// '|'区切りのテキストを複数行のテキストに変換する
void SqliteTextUtill::SplitLines(const std::string& text, std::vector<std::string>& outLines)
{
	outLines.clear();
	if (text.empty()) { return; }
	std::istringstream ss(text);
	std::string token;
	while (std::getline(ss, token, '|')) { outLines.push_back(token); }
}

// SQLの文字列リテラルとしてtextをエスケープする（シングルクォートを2つにする）
std::string SqliteTextUtill::EscapeSqlString(const std::string& text) 
{
	std::string out;
	out.reserve(text.size());
	for (char c : text)
	{
		if (c == '\'') { out += "''"; }
		else { out += c; }
	}
	return out;
}

std::string SqliteTextUtill::ToUtf8(const std::string& text)
{
	return ConvertCodePage(text, 932, CP_UTF8);
}

std::string SqliteTextUtill::FromUtf8(const std::string& text)
{
	return ConvertCodePage(text, CP_UTF8, 932);
}