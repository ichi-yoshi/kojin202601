#pragma once
#include "SqliteEnemy.h"
#include <string>
#include <vector>

class Enemy
{
public:
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	bool GetEnemyBase(EnemyBaseRow& outRow);
};

