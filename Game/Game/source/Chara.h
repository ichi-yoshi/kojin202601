#pragma once
#include "container.h"
#include "CharaBase.h"
#include "CharaAfterStatus.h"

namespace Status
{
	// ステータスを加算する（％は加算後に反映される前提）
	void AddStatus(CharaStatus& dst, const CharaStatus& src);
}

class Chara
{
public:
	// 基礎 + 装備合計 で最終ステータスを作成
	static CharaAfterStatus CalculateAfterStatus(const CharaBase& base, const CharaStatus& equipTotal);

	// ステータスの行からステータスを作成する
	//"攻撃: 12.0" 形式の文字列配列を数値化
	static CharaStatus ParseStatusRows(const std::vector<std::string>& rows);
};

