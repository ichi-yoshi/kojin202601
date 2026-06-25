#pragma once
#include "container.h"
#include "CharaAfterStatus.h"

class SaveCharaStatus
{
public:
	struct StatusRow 
	{
		std::string name;
		double value;
	};

	void SetFromAfterStatus(const CharaAfterStatus& afterStatus);
	const std::vector<StatusRow>& GetRows()const;

	bool SaveToSqlite(std::string* outError = nullptr) const;
	bool LoadFromSqlite(std::string* outError = nullptr);

	void ClearStatus();
	bool HasResult() const;

private:
	std::vector<StatusRow> _rows;

	struct LoadContext
	{
		SaveCharaStatus* self = nullptr;
	};

	static int LoadCallback(void* param, int col_cnt, char** row_txt, char** col_name);

	bool _hasResult = false;
};

