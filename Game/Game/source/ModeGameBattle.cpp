#include "ModeGameBattle.h"
#include "Dxlib.h"

bool ModeGameBattle::Initialize(const std::string& dbPath, std::string* outError)
{
	if(!_gaugeUI.Initialize(dbPath, outError)) { return false; }
	if(!_circleUI.Initialize(dbPath, outError)) { return false; }
	Reset();
	SetPhase(BattleTimer::BattlePhase::Defense);
	return true;
}

void ModeGameBattle::Reset()
{
	_isBattleEnd = false;
	_battleTimer.Initialize();
	_circleUI.Reset();
	_gaugeUI.Reset();
}
	

void ModeGameBattle::Process(MouseInput& mouse, CharaAfterStatus& afterStatus, double deltaTime)
{
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
		{ BattleTimer::BattlePhase::Attack,  [this](MouseInput& m, CharaAfterStatus& s) { UpdateAttack(m, s); } }
	};

	auto it = PhaseMap.find(nextPhase);
	_phaseUpdateFunc = (it != PhaseMap.end()) ? it->second : nullptr;


}

// --- 防御フェーズの個別処理 ---
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

// --- 攻撃フェーズの個別処理 ---
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
	// 攻撃ターンか防御ターンかによって画面の文字やUIの描画を切り替える
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
}