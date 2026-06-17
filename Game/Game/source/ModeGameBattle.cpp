#include "ModeGameBattle.h"
#include "Dxlib.h"
#include <algorithm>

bool ModeGameBattle::Initialize(const std::string& dbPath, std::string* outError)
{
	if(!_gaugeUI.Initialize(dbPath, outError)) { return false; }
	if(!_circleUI.Initialize(dbPath, outError)) { return false; }
	if(!_sqliteEnemy.Initialize(dbPath, outError)) { return false; }
	//Reset();
	SetPhase(BattleTimer::BattlePhase::Start);
	return true;
}

void ModeGameBattle::Reset(const SaveData& saveData)
{
	_isBattleEnd = false;
	_circleUI.Reset();
	_gaugeUI.Reset();

	if(_enemy != nullptr)
	{
		delete _enemy;
		_enemy = nullptr;
	}

	_enemy = new Enemy("ドラゴン");

	if(_enemy != nullptr)
	{
		if(_enemy->LoadEnemy(_sqliteEnemy, saveData))
		{
			// 敵の初期HPを満タンにする
			_enemyCurrentHP = _enemy->GetHP();
		}
	}

	if(_enemy->LoadEnemy(_sqliteEnemy, saveData))
	{
		// 敵の初期HPを満タンにする
		_enemyCurrentHP = _enemy->GetHP();
	}
}
	

void ModeGameBattle::Process(MouseInput& mouse, CharaAfterStatus& afterStatus, SaveData& saveData, double deltaTime)
{
	if(_enemy == nullptr) { return; }

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

	static const std::unordered_map<BattleTimer::BattlePhase, PhaseFunc> PhaseMap = 
	{
		{ BattleTimer::BattlePhase::Defense, [this](MouseInput& m, CharaAfterStatus& s) { UpdateDefense(m, s); } },
		{ BattleTimer::BattlePhase::Attack,  [this](MouseInput& m, CharaAfterStatus& s) { UpdateAttack(m, s); } },
		{ BattleTimer::BattlePhase::Start,{ [this](MouseInput& m, CharaAfterStatus& s) { UpdateStart(m, s); } }}
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
			//if(_resultWaitTime > 0.0)
			//{
			//	_resultWaitTime -= 5.216;
			//	return; // 時間が経つまでは以下のリセット処理をスキップ（表示を維持）
			//}

			if(_gaugeUI.IsSuccess())
			{
				// 敵にダメージを与える、などの処理
				_gaugeUI.Reset();
			}
			else
			{
				// プレイヤーがダメージを受ける、などの処理
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

void ModeGameBattle::Render()
{
	if(_enemy == nullptr) { return; }

	// フェーズに応じて画面の文字やUIの描画を切り替える
	if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Defense)
	{
		SetFontSize(24);
		DrawString(100, 50, "【 敵の攻撃ターン！ 丸を消して時間を進めろ！ 】", GetColor(255, 255, 0));

		// 残り時間の表示（秒数を小数点第1位まで）
		DrawFormatString(100, 80, GetColor(255, 0, 0), "残り消化時間: %.1f 秒", _battleTimer.GetTime());

		_circleUI.Draw();
	}
	else if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Attack)
	{
		SetFontSize(24);
		DrawString(100, 50, "【 自分の攻撃ターン！ ゲージを合わせてダメージを与えろ！ 】", GetColor(255, 50, 50));

		// 残り時間の表示（秒数を小数点第1位まで）
		DrawFormatString(100, 80, GetColor(255, 0, 0), "残り消化時間: %.1f 秒", _battleTimer.GetTime());

		_gaugeUI.Draw();
	}
	else if(_battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Start)
	{
		SetFontSize(24);
		//DrawString(100, 50, "【 戦闘開始！ 】", GetColor(255, 255, 0));
		// 残り時間の表示（秒数を小数点第1位まで）
		DrawFormatString(100, 80, GetColor(255, 0, 0), "戦闘開始まで: %.1f 秒", _battleTimer.GetTime());
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

	if(_isEnemyDefeated)
	{
		SetFontSize(40);
		DrawString(300, 250, "ENEMY DEFEATED!", GetColor(255, 215, 0));
		SetFontSize(20);
		DrawString(300, 310, "次に出現する敵のレベルが上がった！", GetColor(255, 255, 255));
		return;
	}
}