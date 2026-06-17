#pragma once
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
	};

	const std::vector<AccountData>& GetRows()const;

	bool SaveToSqlite(std::string* outError = nullptr) const;
	bool LoadFromSqlite(std::string* outError = nullptr);

	bool HasResult() const;
	bool IncrementEnemyLevelAndSave(std::string* outError = nullptr);
private:
	std::vector<AccountData> _accountData;

	struct LoadContext
	{
		std::vector<AccountData>* accountData = nullptr;
	};

	static int LoadCallback(void* param, int col_cnt, char** row_txt, char** col_name);

	bool _hasResult = false;
};

