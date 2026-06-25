#pragma once
#include "container.h"
#include "CharaBase.h"
#include "CharaAfterStatus.h"

class Chara
{
public:
	// 基礎 + 装備合計 で最終ステータスを作成
	static CharaAfterStatus CalculateAfterStatus(const CharaBase& base, const CharaStatus& equipTotal);

	// ステータスの行からステータスを作成する
	//"攻撃: 12.0" 形式の文字列配列を数値化
	static CharaStatus ParseStatusLines(const std::vector<std::string>& lines);
};

