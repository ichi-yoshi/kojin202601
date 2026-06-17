#include "Enemy.h"
#include "SqliteEnemy.h"
#include "SqliteTextUtill.h"
#include <iostream>

Enemy::Enemy(const std::string& name)
{
	_enemyName = name;
	_currentLevel = 0;

	_finalHP = 0.0;
	_finalAttack = 0.0;
}

bool Enemy::LoadEnemy(SqliteEnemy& sqliteEnemy, const SaveData& saveData)
{
	EnemyBaseRow baseRow;
	if(!sqliteEnemy.GetEnemyBase(baseRow))
	{
		std::cerr << "エラー: マスタデータがありません。" << std::endl;
		return false;
	}

	// SaveDataから敵レベルの追加値を取得する
	int additionalEnemyLevel = 0;
	const auto& accountRows = saveData.GetRows();
	if(!accountRows.empty())
	{
		additionalEnemyLevel = accountRows[0].enemylevel;
	}

	_currentLevel = baseRow.startLevel + additionalEnemyLevel;

	// レベル差を計算し、負の値にならないようにする
	int levelDiff = _currentLevel - baseRow.startLevel;
	if(levelDiff < 0) levelDiff = 0;

	// HPと攻撃力を計算する
	_finalHP = baseRow.hp + (levelDiff * baseRow.levelBonus);
	_finalAttack = baseRow.attack + (levelDiff * baseRow.levelBonus);

	return true;
}