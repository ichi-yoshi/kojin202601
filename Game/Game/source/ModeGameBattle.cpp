#include "ModeGameBattle.h"
#include "Dxlib.h"
#include <algorithm>
#include <sstream> // （文字列結合用）
#include <iomanip> // （小数点桁数固定用）
#include <iostream>
#include <algorithm>
#include "EvaluateFormula.h"

extern std::string g_debugFormulaReplaced = "(未計算)";

bool ModeGameBattle::Initialize(const std::string& dbPath, std::string* outError)
{
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

	_damageHistory.clear();

	if(_enemy != nullptr)
	{
		delete _enemy;
		_enemy = nullptr;
	}

	_enemy = new Enemy("ドラゴン");

	if(_enemy != nullptr)
	{
		_enemy->LoadEnemy(_sqliteEnemy, saveData);
		_enemyCurrentHP = _enemy->GetHP();
		SetPhase(BattleTimer::BattlePhase::Start);
	}

	// キャラクターのHPを初期化する
	if(!_isHPInitialized)
	{
		_charaCurrentHP = afterStatus.GetAfterStatus().hp;
		_isHPInitialized = true;
	}
}
	
void ModeGameBattle::Process(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData, double deltaTime)
{
	if(_enemy == nullptr) { return; }

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
		ProcessBattleResult(saveData);
		_isResultProcessed = true;
	}

	_battleTimer.Update(deltaTime);

	if(_phaseUpdateFunc != nullptr)
	{
		_phaseUpdateFunc(mouse, afterStatus);
	}
}

void ModeGameBattle::SetPhase(BattleTimer::BattlePhase nextPhase)
{
	// タイマーのフェーズを切り替え
	_battleTimer.ChangePhase(nextPhase);

	// 防御フェーズが始まったら、リセット
	if(nextPhase == BattleTimer::BattlePhase::Defense)
	{
		_nextDamageTime = 35.0;
	}

	// フェーズに応じた処理関数を設定
	static const std::unordered_map<BattleTimer::BattlePhase, PhaseFunc> PhaseMap = 
	{
		{ BattleTimer::BattlePhase::Defense, [this](MouseInput& m, CharaAfterStatus& s) { UpdateDefense(m, s); } },
		{ BattleTimer::BattlePhase::Attack,  [this](MouseInput& m, CharaAfterStatus& s) { UpdateAttack(m, s); } },
		{ BattleTimer::BattlePhase::Start,{ [this](MouseInput& m, CharaAfterStatus& s) { UpdateStart(m, s); } }},
		{ BattleTimer::BattlePhase::Result, [this](MouseInput& m, CharaAfterStatus& s) { UpdateResult(m, s, saveData); } }
	};

	auto it = PhaseMap.find(nextPhase);
	_phaseUpdateFunc = (it != PhaseMap.end()) ? it->second : nullptr;
}

// スタートフェーズの処理（必要に応じて実装）
void ModeGameBattle::UpdateStart(MouseInput& mouse, CharaAfterStatus& afterStatus)
{
	// 現状は特に処理は行わず、時間が経過したらフェーズに移行する
	if(_battleTimer.IsTimeUp())
	{
		SetPhase(BattleTimer::BattlePhase::Attack); // フェーズ切り替え
	}
}

// 防御フェーズの個別処理
void ModeGameBattle::UpdateDefense(MouseInput& mouse, CharaAfterStatus& afterStatus)
{
	if(!_battleTimer.IsTimeUp())
	{
		// バトルタイマーの残り時間を監視して秒ごとにダメージを発生
		double currentTimer = _battleTimer.GetTime();
		if(currentTimer <= _nextDamageTime)
		{
			// 敵の攻撃ダメージを計算して適用
			double enemyDamage = _charaFormula.CalculateEnemyDamage(afterStatus, *_enemy);
			_charaCurrentHP -= enemyDamage;
			if(_charaCurrentHP < 0.0) { _charaCurrentHP = 0.0; }

			// 次の秒刻みの閾値に更新
			_nextDamageTime -= 5.0;
		}

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
void ModeGameBattle::UpdateAttack(MouseInput& mouse, CharaAfterStatus& afterStatus)
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

void ModeGameBattle::UpdateResult(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData)
{
	// リザルトの表示時間（5秒）が終了したら、自動的にバトルを終了して次の画面へ
	if(_battleTimer.IsTimeUp())
	{
		_isBattleEnd = true;
	}
}

void ModeGameBattle::ProcessBattleResult(SaveData& saveData)
{
	// 現在のセーブデータを一度ローカル（一時的）にコピーして取得
	auto constRows = saveData.GetRows();

	SaveData::AccountData account;
	if(constRows.empty())
	{
		// データベースが万が一空だった場合の初期化ガード
		account.uid = 0;
		account.level = 1;
		account.exp = 0;
		account.coin = 30000;
		account.enemylevel = 0;
	}
	else
	{
		// 既存のデータをベースにする
		account = constRows[0];
	}

	// 勝敗に応じたステータスの計算
	if(_enemyCurrentHP <= 0.0) // 敵を倒した場合
	{
		// 敵レベルを +1
		account.enemylevel += 1;

		// 経験値(EXP)を付与（例：敵のレベル×100 EXP）
		int gainExp = (_enemy ? _enemy->GetLevel() : 1) * 100;
		account.exp += gainExp;

		// レベルアップ判定（必要経験値 = 現在レベル * 100 の簡易仕様例）
		while(account.exp >= (account.level * 100))
		{
			account.exp -= (account.level * 100);
			account.level += 1;
		}
	}

	// コインの計算（条件：残りHP ＋ 与えた最大ダメージ）
	double RemainingHP = (std::max)(0.0, _charaCurrentHP);
	int gainCoin = static_cast<int>(RemainingHP + _maxDamageDealt);
	account.coin += gainCoin;

	std::vector<SaveData::AccountData> updatedVector;
	updatedVector.push_back(account); // 編集し終わったデータを格納

	std::string errStr;
	bool success = false;
	success = saveData.UpdateAccountAndSave(account, &errStr);
}

void ModeGameBattle::Render(CharaAfterStatus& afterStatus)
{
	if(_enemy == nullptr) { return; }

	// フェーズに応じて画面の文字やUIの描画を切り替える
	if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Defense)
	{
		SetFontSize(24);
		DrawString(100, 50, "【 敵の防御ターン！ 丸を消して時間を進めろ！ 】", GetColor(255, 255, 0));
		DrawFormatString(100, 80, GetColor(255, 0, 0), "攻撃フェーズまで: %.1f 秒", _battleTimer.GetTime());

		_circleUI.Draw();
	}
	else if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Attack)
	{
		SetFontSize(24);
		DrawString(100, 50, "【 自分の攻撃ターン！ ゲージを合わせてダメージを与えろ！ 】", GetColor(255, 50, 50));
		DrawFormatString(100, 80, GetColor(255, 0, 0), "防御フェーズまで: %.1f 秒", _battleTimer.GetTime());

		_gaugeUI.Draw();
	}
	else if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Start)
	{
		SetFontSize(24);
		DrawFormatString(100, 80, GetColor(255, 0, 0), "戦闘開始まで: %.1f 秒", _battleTimer.GetTime());
	}
	else if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Result)
	{
		SetFontSize(36);
		if(_enemyCurrentHP <= 0.0) 
		{ 
			DrawString(200, 150, "【 勝利!!! 】", GetColor(255, 215, 0));
		}
		else 
		{
			DrawString(200, 150, "【 敗北... 】", GetColor(255, 50, 50));
		}

		SetFontSize(24);
		DrawFormatString(200, 230, GetColor(100, 100, 100), "今回の最大一撃ダメージ: %.0f DMG", _maxDamageDealt);
		DrawFormatString(200, 270, GetColor(100, 100, 100), "残りHPボーナス: %.0f", (std::max)(0.0, _charaCurrentHP)); 

		int finalGain = static_cast<int>((std::max)(0.0, _charaCurrentHP) + _maxDamageDealt); 
		DrawFormatString(200, 320, GetColor(100, 100, 100), "獲得コイン: + %d COIN !", finalGain);

		SetFontSize(18);
		DrawFormatString(200, 400, GetColor(150, 150, 150), "間もなく次の画面へ移動します... (%.1f)", _battleTimer.GetTime()); 
		return;
	}

	// 敵が存在すれば、画面上部に敵の情報とHPバーを表示する
	if(_enemy)
	{
		// 敵の名前とレベルを描画
		SetFontSize(20);
		DrawFormatString(200, 140, GetColor(100, 100, 100), "%s  (Lv.%d)", _enemy->GetName().c_str(), _enemy->GetLevel());

		// HPバーの枠を描画
		DrawBox(200, 170, 600, 190, GetColor(100, 100, 100), FALSE);

		// 敵の残りHPの割合に応じて緑色のバーを描画
		double hpRate = _enemyCurrentHP / max(1.0, _enemy->GetHP());
		int barWidth = static_cast<int>(400 * hpRate);
		DrawBox(200, 170, 200 + barWidth, 190, GetColor(0, 255, 0), TRUE);

		// HPの数値テキスト表示
		DrawFormatString(200, 200, GetColor(200, 200, 200), "HP: %.0f / %.0f", _enemyCurrentHP, _enemy->GetHP());
	}

	SetFontSize(16);
	const int LEFT_X = 50;   // 左側座標
	const int RIGHT_X = 900; // 右側座標（画面サイズに合わせて調整してください）

	//// ----------------------------------------------------------------
	//// 【左側】数式展開デバッグ表示（確率抽選・Poop減衰 対応版）
	//// ----------------------------------------------------------------
	//int leftY = 300;
	//DrawString(LEFT_X, leftY, "--- [左側] 数式展開デバッグ ---", GetColor(100, 100, 100));

	//leftY += 25;
	//std::string repStr = "［代入状態］: " + g_debugFormulaReplaced;
	//DrawString(LEFT_X, leftY, repStr.c_str(), GetColor(100, 100, 100)); // リアルタイムな計算履歴を緑色で展開

	//leftY += 25;
	//DrawFormatString(LEFT_X, leftY, GetColor(100, 100, 100), "・ダメージ減衰率の仕様: 1+(Poop / 5000)");

	//leftY += 25;
	//// 外枠の公式のガイドラインを今回の最新仕様に書き換え
	//DrawFormatString(LEFT_X, leftY, GetColor(255, 200, 0), "最終公式: (ベース計算) * 会心倍率(抽選) * ゲージ倍率 * 運値2倍(抽選)");


	// 右側：最終ダメージ（新しいものが古いものの下へ追加される）
	int rightY = 100;
	DrawString(RIGHT_X, rightY, "--- [右側] 最終ダメージ履歴 ---", GetColor(100, 100, 100));
	rightY += 25;

	for(size_t i = 0; i < _damageHistory.size(); ++i)
	{
		// 新しい結果ほど下に描画される（iが大きくなる＝ループが後に回る＝Y座標が下に下がる）
		if(i == _damageHistory.size() - 1)
		{
			// 最新のダメージは赤色で強調
			DrawFormatString(RIGHT_X, rightY, GetColor(255, 50, 50), "Hit %02d: %.0f ダメージ!", i + 1, _damageHistory[i]);
		}
		else
		{
			DrawFormatString(RIGHT_X, rightY, GetColor(100, 100, 100), "Hit %02d: %.0f ダメージ!", i + 1, _damageHistory[i]);
		}
		rightY += 22; // 下にずらしていく

		// 画面外にはみ出さないための簡易セーフティ
		if(rightY > 650) { break; }
	}

	// プレイヤーのHPバーを描画する
	{
		double maxHp = afterStatus.GetAfterStatus().hp;

		if(maxHp < 1.0)
		{
			maxHp = 1.0;
		}

		// プレイヤーの名前とHPバーの枠を描画（敵のバーの下側に配置 X=200, Y=240付近）
		SetFontSize(20);
		DrawString(200, 240, "プレイヤー", GetColor(120, 200, 255));
		DrawBox(200, 270, 600, 290, GetColor(100, 100, 100), FALSE);

		// プレイヤーの残りHPの割合に応じて青色（または緑）のバーを描画
		double playerHpRate = _charaCurrentHP / maxHp;
		int playerBarWidth = static_cast<int>(400 * playerHpRate);

		// HPが低くなったらバーの色を赤に変える演出
		unsigned int barColor = (playerHpRate > 0.2) ? GetColor(50, 150, 255) : GetColor(255, 50, 50);
		DrawBox(200, 270, 200 + playerBarWidth, 290, barColor, TRUE);

		// プレイヤーのHP数値テキスト表示
		DrawFormatString(200, 300, GetColor(100, 100, 100), "HP: %.0f / %.0f", _charaCurrentHP, maxHp);
	}	
}