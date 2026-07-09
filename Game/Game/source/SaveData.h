#pragma once
#include "SqliteUtill.h"
#include "container.h"

class SaveData
{
public:
	struct AccountData
	{
		int uid;		// ユーザーID
		int level;		// プレイヤーレベル
		int exp;		// 経験値
		int coin;		// コイン
		int enemylevel;	// 敵レベル
		int gachaCount;	// ガチャ回数
	};

	const std::vector<AccountData>& GetRows()const { return _accountData; }
	bool SaveToSqlite(std::string* outError = nullptr) const;	// データベースに保存
	bool LoadFromSqlite(std::string* outError = nullptr);		// データベースからロード

	bool HasResult() const { return _hasResult; }				// データベースに結果があるかどうか
	bool IncrementEnemyLevelAndSave(std::string* outError = nullptr);	// 敵レベルを +1 して保存する
	bool UpdateAccountAndSave(const AccountData& data, std::string* outError = nullptr);	// アカウントデータを更新して保存する

	int GetPlayerLevel() const;

	std::vector<std::string> ToLines() const;
private:
	std::vector<AccountData> _accountData;

	struct LoadContext
	{
		std::vector<AccountData>* accountData = nullptr;
	};

	static int LoadCallback(void* param, int col_cnt, char** row_txt, char** col_name);

	bool _hasResult = false;
};

