#include "ModeGameBattle.h"
#include "EvaluateFormula.h"
#include "Resource.h"
#include "MagicNumberConfig.h"

// 名前空間の使用宣言
using namespace UIConfig;

bool ModeGameBattle::Initialize(const std::string& dbPath, std::string* outError)
{
	// 各UIとデータベースの初期化
	if(!_gaugeUI.Initialize(dbPath, outError)) { return false; }
	if(!_circleUI.Initialize(dbPath, outError)) { return false; }
	if(!_sqliteEnemy.Initialize(dbPath, outError)) { return false; }
	if(!_charaFormula.Initialize(dbPath, outError)) { return false; }
	if(!_SqliteCharaFormula.Initialize(dbPath, outError)) { return false; }
	SetPhase(BattleTimer::BattlePhase::Start);
	return true;
}

void ModeGameBattle::Reset(const SaveData& saveData, CharaAfterStatus& afterStatus)
{
	_isBattleEnd = false;
	_circleUI.Reset();
	_gaugeUI.Reset();
	_isHPInitialized = false;
	_isResultProcessed = false; 
	_maxDamageDealt = 0.0;
	_damageFlashTimer = 0.0;

	_damageHistory.clear();

	if(_enemy != nullptr)
	{
		delete _enemy;
		_enemy = nullptr;
	}

	// 新しい敵を生成してロードする
	_enemy = new Enemy("ゴエポン");

	if(_enemy != nullptr)
	{
		_enemy->LoadEnemy(_sqliteEnemy, saveData);
		_enemyCurrentHP = _enemy->GetHP();
		_enemy->SetupModel(mv1::Tanuki);
		SetPhase(BattleTimer::BattlePhase::Start);
	}

	// キャラクターのHPを初期化する
	if(!_isHPInitialized)
	{
		_charaCurrentHP = afterStatus.GetAfterStatus().hp;
		_isHPInitialized = true;
	}
}
	
void ModeGameBattle::Process(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData, CharaBase& Charabase, SaveEquipment& saveEquipment, float deltaTime)
{
	if(_enemy == nullptr) { return; }

	// 敵のアニメーションを更新
	_enemy->UpdateAnimation(deltaTime);

	// ダメージフラッシュのタイマーを更新
	if(_damageFlashTimer > 0.0)
	{
		_damageFlashTimer -= deltaTime;
	}

	// バトルフェーズの更新
	if(_battleTimer.GetCurrentPhase() != BattleTimer::BattlePhase::Result) 
	{
		if(_enemyCurrentHP <= 0.0 || _charaCurrentHP <= 0.0) 
		{
			SetPhase(BattleTimer::BattlePhase::Result); // リザルトフェーズへ移行
		}
	}

	// リザルトフェーズに入った瞬間、一度だけSQLiteデータの更新を実行
	if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Result && !_isResultProcessed) 
	{
		ProcessBattleResult(saveData, afterStatus, Charabase, saveEquipment);
		_isResultProcessed = true;
	}

	_battleTimer.Update(deltaTime);

	// フェーズに応じた処理関数を呼び出す
	if(_phaseUpdateFunc != nullptr)
	{
		_phaseUpdateFunc(mouse, afterStatus, deltaTime);
	}
}

void ModeGameBattle::SetPhase(BattleTimer::BattlePhase nextPhase)
{
	// タイマーのフェーズを切り替え
	_battleTimer.ChangePhase(nextPhase);

	if(nextPhase == BattleTimer::BattlePhase::Defense)
	{
		_nextDamageTime = 0.0;
		if(_enemy) _enemy->SetStatus(EnemyStatus::attack); // 敵が攻撃してくるフェーズ
	}
	else if(nextPhase == BattleTimer::BattlePhase::Attack)
	{
		if(_enemy) _enemy->SetStatus(EnemyStatus::defend); // 敵が防御（プレイヤーが攻撃）するフェーズ
	}
	else if(nextPhase == BattleTimer::BattlePhase::Result)
	{
		if(_enemy)
		{
			// リザルトフェーズに入ったときの敵のステータスを設定
			if(_enemyCurrentHP <= 0.0) 
			{
				_enemy->SetStatus(EnemyStatus::idle);	// 負けモーションがなければidleなど
			}
			else 
			{
				_enemy->SetStatus(EnemyStatus::winner); // 敵の勝利
			}
		}
	}

	// フェーズに応じた処理関数を設定
	static const std::unordered_map<BattleTimer::BattlePhase, PhaseFunc> PhaseMap = 
	{
		{ BattleTimer::BattlePhase::Defense, [this](MouseInput& m, CharaAfterStatus& s, float dt) { UpdateDefense(m, s, dt); } },
		{ BattleTimer::BattlePhase::Attack,  [this](MouseInput& m, CharaAfterStatus& s, float dt) { UpdateAttack(m, s, dt); } },
		{ BattleTimer::BattlePhase::Start, [this](MouseInput& m, CharaAfterStatus& s, float dt) { UpdateStart(m, s, dt); } },
		{ BattleTimer::BattlePhase::Result, [this](MouseInput& m, CharaAfterStatus& s, float dt) { UpdateResult(m, s, saveData, dt); } }
	};

	auto it = PhaseMap.find(nextPhase);
	_phaseUpdateFunc = (it != PhaseMap.end()) ? it->second : nullptr;
}

// スタートフェーズの処理（必要に応じて実装）
void ModeGameBattle::UpdateStart(MouseInput& mouse, CharaAfterStatus& afterStatus, float deltaTime)
{
	// 現状は特に処理は行わず、時間が経過したらフェーズに移行する
	if(_battleTimer.IsTimeUp())
	{
		SetPhase(BattleTimer::BattlePhase::Attack); // フェーズ切り替え
	}
}

// 防御フェーズの個別処理
void ModeGameBattle::UpdateDefense(MouseInput& mouse, CharaAfterStatus& afterStatus, float deltaTime)
{
	if(!_battleTimer.IsTimeUp())
	{
		// バトルタイマーの残り時間を監視して秒ごとにダメージを発生
		double currentTimer = _battleTimer.GetTime();
		_nextDamageTime += deltaTime;
		if(_nextDamageTime >= EnemyDamageInterval)
		{
			// 敵の攻撃ダメージを計算して適用
			double enemyDamage = _charaFormula.CalculateEnemyDamage(afterStatus, *_enemy);
			_charaCurrentHP -= enemyDamage;
			if(_charaCurrentHP < 0.0) { _charaCurrentHP = 0.0; }

			// ダメージを受けたことを示すフラッシュタイマーをリセット
			_damageFlashTimer = Param::FlashTime;

			// 次の秒刻みの閾値に更新
			_nextDamageTime -= EnemyDamageInterval;
		}

		// 円UIの更新と、成功したかどうかの判定
		if(_circleUI.Update(mouse))
		{
			_battleTimer.IsClearCircle(afterStatus);

			if(_circleUI.HasResult() && _circleUI.IsSuccess())
			{
				_circleUI.Reset();
			}
		}
	}
	else
	{
		_gaugeUI.Reset();
		SetPhase(BattleTimer::BattlePhase::Attack); // フェーズ切り替え
	}
}

// 攻撃フェーズの個別処理
void ModeGameBattle::UpdateAttack(MouseInput& mouse, CharaAfterStatus& afterStatus, float deltaTime)
{
	if(!_battleTimer.IsTimeUp())
	{
		if(!_gaugeUI.HasResult())
		{
			_gaugeUI.Update(mouse);
		}
		else
		{
			if(!_gaugeUI.HasResult())
			{
				_gaugeUI.Update(mouse);
			}
			else
			{
				// ゲージの結果が出て、まだダメージ履歴に追加されていなければ計算する
				// ※何度も同じダメージが適用されないように、この攻撃ターンで1度だけ処理するための判定
				bool isSuccess = _gaugeUI.IsSuccess();

				// この中で SQLiteの読み込み、確率抽選、ゲージ補正がすべて完結します。
				double damage = _charaFormula.CalculateFinalDamage(afterStatus, *_enemy, isSuccess);

				if(damage > _maxDamageDealt)
				{
					_maxDamageDealt = damage;
				}

				// 履歴に追加
				_damageHistory.push_back(damage);

				// HP減少処理
				_enemyCurrentHP -= damage;
				if(_enemyCurrentHP < 0) { _enemyCurrentHP = 0; }

				_gaugeUI.Reset();
			}
		}
	}
	else
	{
		_circleUI.Reset();
		SetPhase(BattleTimer::BattlePhase::Defense); // フェーズ切り替え
	}
}

void ModeGameBattle::UpdateResult(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData, float deltaTime)
{
	// リザルトの表示時間が終了したら、自動的にバトルを終了して次の画面へ
	if(_battleTimer.IsTimeUp())
	{
		_isBattleEnd = true;
	}
}

void ModeGameBattle::ProcessBattleResult(SaveData& saveData, CharaAfterStatus& afterStatus, CharaBase& Charabase, SaveEquipment& saveEquipment)
{
	// 現在のセーブデータを一度ローカル（一時的）にコピーして取得
	auto constRows = saveData.GetRows();

	SaveData::AccountData account;
	if(!constRows.empty())
	{
		account = constRows[0];
	}

	// 勝敗に応じたステータスの計算
	if(_enemyCurrentHP <= 0.0) // 敵を倒した場合
	{
		// 敵レベルを +1
		account.enemylevel += 1;

		// 経験値(EXP)を付与（例：敵のレベル×100 EXP）
		int gainExp = (_enemy ? _enemy->GetLevel() : 1) * ExpMultipler;
		account.exp += gainExp;

		// レベルアップ判定（必要経験値 = 現在レベル * 100 の簡易仕様）
		while(account.exp >= (account.level * ExpMultipler))
		{
			account.exp -= (account.level * ExpMultipler);
			account.level += 1;
		}
	}
	else 
	{
		// 経験値(EXP)を付与（例：敵のレベル×100 EXP）
		int gainExp = (_enemy ? _enemy->GetLevel() : 1) * ExpMultipler;
		account.exp += gainExp;

		// レベルアップ判定（必要経験値 = 現在レベル * 100 の簡易仕様）
		while(account.exp >= account.level)
		{
			account.exp -= account.level;
			account.level += 1;
		}
	}

	// コインの計算（条件：残りHP ＋ 与えた最大ダメージ）
	double RemainingHP = (std::max)(0.0, _charaCurrentHP);
	int gainCoin = static_cast<int>(RemainingHP + _maxDamageDealt);
	account.coin += gainCoin;

	std::vector<SaveData::AccountData> updatedVector;
	updatedVector.push_back(account); // 編集し終わったデータを格納

	// SQLiteに保存
	std::string errStr;
	bool success = false;
	success = saveData.UpdateAccountAndSave(account, &errStr);

	// SQLiteの保存が成功した場合、キャラクターのステータスを更新する
	if(success)
	{
		afterStatus.UpdateFrom(Charabase, saveEquipment, saveData);
	}
}

void ModeGameBattle::Render(CharaAfterStatus& afterStatus)
{
	if(_enemy == nullptr) { return; }

	_enemy->DrawModel();

	_battleUI.Render(
		afterStatus,
		_enemy,
		_enemyCurrentHP,
		_charaCurrentHP,
		_battleTimer,
		_circleUI,
		_gaugeUI,
		_maxDamageDealt,
		_damageHistory,
		_damageFlashTimer
	);
}