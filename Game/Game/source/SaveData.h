#pragma once
#include "SqliteUtill.h"
#include <string>
#include <vector>

class SaveData
{
public:
	struct AccountData
	{
		int uid;
		int level;
		int exp;
		int coin;
		int enemylevel;
		int gachaCount;
	};

	const std::vector<AccountData>& GetRows()const;
	bool SaveToSqlite(std::string* outError = nullptr) const;
	bool LoadFromSqlite(std::string* outError = nullptr);

	bool HasResult() const;
	bool IncrementEnemyLevelAndSave(std::string* outError = nullptr);
	
	bool UpdateAccountAndSave(const AccountData& data, std::string* outError = nullptr);

	int GetPlayerLevel() const;
	/*int GetGachaCount(AccountData& accountData) const;
	int GetGachaCost(AccountData& accountData) const;*/
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

