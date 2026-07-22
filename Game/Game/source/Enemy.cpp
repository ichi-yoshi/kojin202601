#include "Enemy.h"
#include "animationmanager.h"

Enemy::Enemy(const std::string& name)
{
	_enemyName = name;
	_currentLevel = 0;
	_finalHP = 0.0;
	_finalAttack = 0.0;
	_modelHandle = -1;
	_animId = -1;
}

Enemy::~Enemy()
{
	// アニメーションを停止
	if(_animId != -1)
	{
		AnimationManager::GetInstance()->Stop(_animId);
	}

	// モデルを解放
	if(_modelHandle != -1)
	{
		MV1DeleteModel(_modelHandle);
	}
}

bool Enemy::LoadEnemy(SqliteEnemy& sqliteEnemy, const SaveData& saveData)
{
	// 敵の基本情報を取得する
	EnemyBaseRow baseRow;
	if(!sqliteEnemy.GetEnemyBase(baseRow)) { return false; }

	// SaveDataから敵レベルの追加値を取得する
	int additionalEnemyLevel = 0;					// デフォルトは0
	const auto& accountRows = saveData.GetRows();	// SaveDataからアカウントデータを取得

	// アカウントデータが存在する場合、最初の行の敵レベルを取得
	if(!accountRows.empty())
	{
		additionalEnemyLevel = accountRows[0].enemylevel;
	}
	_currentLevel = baseRow.startLevel + additionalEnemyLevel;	// 現在の敵レベルを計算

	// レベル差を計算し、負の値にならないようにする
	int levelDiff = _currentLevel - baseRow.startLevel;
	if(levelDiff < 0) levelDiff = 0;

	// HPと攻撃力を計算する
	_finalHP = baseRow.hp + (200 * levelDiff * baseRow.levelBonus);
	_finalAttack = baseRow.attack + (levelDiff * baseRow.levelBonus);

	return true;
}

void Enemy::SetupModel(const std::string& modelPath)
{
	// 既に読み込まれていれば念のため解放
	if(_modelHandle != -1)
	{
		AnimationManager::GetInstance()->Stop(_animId);
		MV1DeleteModel(_modelHandle);
		_modelHandle = -1;
		_animId = -1;
	}

	// モデルのロード
	_modelHandle = MV1LoadModel(modelPath.c_str());
	if(_modelHandle != -1)
	{
		// 3D空間に配置
		MV1SetPosition(_modelHandle, VGet(0.0f, 0.0f, 0.0f));

		_status = EnemyStatus::idle;
		_prevStatus = EnemyStatus::none;
	}
}

void Enemy::UpdateAnimation(double deltaTime)
{
	// ステータスに変化があったかチェック
	if(_status != _prevStatus)
	{
		// 古いアニメーションを停止
		if(_animId != -1)
		{
			AnimationManager::GetInstance()->Stop(_animId);
			_animId = -1;
		}

		// 新しいステータスに応じたアニメーション名を決定
		std::string animName = "Idle"; // デフォルト

		// ステータスに応じてアニメーション名を設定
		switch(_status)
		{
		case EnemyStatus::idle:
			animName = "walk";    // 待機
			break;
		case EnemyStatus::attack:
			animName = "henge";   // 攻撃
			break;
		case EnemyStatus::defend:
			animName = "idle";	  // 防御
			break;
		case EnemyStatus::winner:
			animName = "takarabako_open";  // 勝利
			break;
		default:
			animName = "walk";
			break;
		}

		// 新しいアニメーションを再生 (モデルハンドルが有効な場合のみ)
		if(_modelHandle != -1)
		{
			_animId = AnimationManager::GetInstance()->Play(_modelHandle, animName, true, 21.0f);
		}

		// 前回のステータスを更新
		_prevStatus = _status;
	}

	// 既存のアニメーションマネージャーの更新処理
	AnimationManager::GetInstance()->Update(static_cast<float>(deltaTime));
}

// モデルの描画
void Enemy::DrawModel()
{
	if(_modelHandle != -1)
	{
		MV1DrawModel(_modelHandle);
	}
}