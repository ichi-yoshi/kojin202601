#pragma once
#include "SqliteEnemy.h"
#include "SqliteUtill.h"
#include "SaveData.h"
#include "Resource.h"
#include "container.h"

// 敵の状態
enum class EnemyStatus
{
	none,	// 初期
	idle,	// 待機
	attack,	// 攻撃
	defend,	// 防御
	winner,	// 勝利
	_EOT_
};

class Enemy
{
public:
	Enemy(const std::string& name);
	~Enemy();
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	bool LoadEnemy(SqliteEnemy& sqliteEnemy, const SaveData& saveData);

	// モデルのセットアップ、アニメーションの更新、描画
	void SetupModel(const std::string& modelPath); 
	void UpdateAnimation(double deltaTime);        
	void DrawModel();                              

	// 敵情報の取得
	std::string GetName() const { return _enemyName; }
	int GetLevel() const { return _currentLevel; }
	double GetHP() const { return _finalHP; }
	double GetAttack() const { return _finalAttack; }
	void SetStatus(EnemyStatus newStatus) { _status = newStatus; }
private:
	std::string _enemyName;
	int _currentLevel;
	double _finalHP;
	double _finalAttack;

	EnemyStatus _status = EnemyStatus::idle;
	EnemyStatus _prevStatus = EnemyStatus::none;

	int _modelHandle = -1;  
	int _animId = -1;       
};

