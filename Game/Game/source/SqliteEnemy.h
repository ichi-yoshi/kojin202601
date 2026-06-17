#pragma once
#include <string>
#include <vector>

struct EnemyBaseRow
{
	std::string  enemyName;
	double hp;
	double attack;
	std::string lv;
	int startLevel;
	double levelBonus;
};

class SqliteEnemy
{
public:
	bool Initialize(const std::string& Path, std::string* outError);
	bool GetEnemyBase(EnemyBaseRow& outRow);
private:
	std::vector<EnemyBaseRow> _rows;

	bool LoadEnemyBaseSqlite(std::vector<EnemyBaseRow>& outRows, std::string* outError = nullptr);
};