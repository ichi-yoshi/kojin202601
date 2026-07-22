#include "SqliteTextUtill.h"

namespace
{
	// 文字コード変換（fromCP -> toCP）
	std::string ConvertCodePage(const std::string& text, UINT fromCP, UINT toCP)
	{
		if(text.empty()) { return {}; }

		// まず、fromCPからワイド文字列に変換するためのバッファサイズを取得
		int wlen = MultiByteToWideChar(fromCP, 0, text.c_str(), -1, nullptr, 0);
		if(wlen <= 0) { return {}; }

		// ワイド文字列に変換するためのバッファを確保
		std::wstring wbuf(static_cast<size_t>(wlen), L'\0');
		MultiByteToWideChar(fromCP, 0, text.c_str(), -1, &wbuf[0], wlen);

		// 次に、ワイド文字列からtoCPに変換するためのバッファサイズを取得
		int blen = WideCharToMultiByte(toCP, 0, wbuf.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if(blen <= 0) { return {}; }

		// toCPに変換するためのバッファを確保
		std::string buf(static_cast<size_t>(blen), '\0');
		WideCharToMultiByte(toCP, 0, wbuf.c_str(), -1, &buf[0], blen, nullptr, nullptr);

		// 末尾のヌル文字を削除して返す
		if(!buf.empty() && buf.back() == '\0') { buf.pop_back(); }
		return buf;
	}
}

// 複数行のテキストを'|'区切りの1行テキストに変換する
std::string SqliteTextUtill::JoinRows(const std::vector<std::string>& rows) 
{
	std::ostringstream os;
	for(size_t i = 0; i < rows.size(); ++i) 
	{
		if (i > 0) { os << "|"; }
		os << rows[i];
	}
	return os.str();
}

// '|'区切りのテキストを複数行のテキストに変換する
void SqliteTextUtill::SplitRows(const std::string& text, std::vector<std::string>& outRows)
{
	outRows.clear();
	if(text.empty()) { return; }
	std::istringstream ss(text);
	std::string token;
	while(std::getline(ss, token, '|')) { outRows.push_back(token); }
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

// CP932 -> UTF-8
std::string SqliteTextUtill::ToUtf8(const std::string& text)
{
	return ConvertCodePage(text, 932, CP_UTF8);
}

// UTF-8 -> CP932
std::string SqliteTextUtill::FromUtf8(const std::string& text)
{
	return ConvertCodePage(text, CP_UTF8, 932);
}