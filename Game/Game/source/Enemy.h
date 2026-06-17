#pragma once
#include "SqliteEnemy.h"
#include "SqliteUtill.h"
#include "SaveData.h"
#include <string>
#include <vector>

class Enemy
{
public:
	Enemy(const std::string& name);
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	bool LoadEnemy(SqliteEnemy& sqliteEnemy, const SaveData& saveData);

	std::string GetName() const { return _enemyName; }
	int GetLevel() const { return _currentLevel; }
	double GetHP() const { return _finalHP; }
	double GetAttack() const { return _finalAttack; }

private:
	std::string _enemyName;
	int _currentLevel;
	double _finalHP;
	double _finalAttack;
};

