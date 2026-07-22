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

	// 初期化
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);

	// 敵情報のロード
	bool LoadEnemy(SqliteEnemy& sqliteEnemy, const SaveData& saveData);

	// モデル関連
	void SetupModel(const std::string& modelPath);	// モデルのセットアップ
	void UpdateAnimation(double deltaTime);			// アニメーションの更新
	void DrawModel();								// モデルの描画

	// 敵情報の取得
	std::string GetName() const { return _enemyName; }				// 敵の名前
	int GetLevel() const { return _currentLevel; }					// 敵のレベル
	double GetHP() const { return _finalHP; }						// 敵のHP
	double GetAttack() const { return _finalAttack; }				// 敵の攻撃力	
	void SetStatus(EnemyStatus newStatus) { _status = newStatus; }	// 敵の状態を設定

private:
	std::string _enemyName;	// 敵の名前
	int _currentLevel;		// 敵の現在のレベル
	double _finalHP;		// 敵の最終HP
	double _finalAttack;	// 敵の最終攻撃力
	int _modelHandle = -1;	// モデルハンドル
	int _animId = -1;		// アニメーションID

private:
	// 敵の状態管理
	EnemyStatus _status = EnemyStatus::idle;
	EnemyStatus _prevStatus = EnemyStatus::none;
};

