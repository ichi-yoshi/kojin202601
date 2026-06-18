#include "ModeGameBattle.h"
#include "Dxlib.h"
#include <algorithm>
#include <sstream> // （文字列結合用）
#include <iomanip> // （小数点桁数固定用）
#include "EvaluateFormula.h"

static std::string g_debugFormulaRaw = "(未計算)";
static std::string g_debugFormulaReplaced = "(未計算)";

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
			if(!_gaugeUI.HasResult())
			{
				_gaugeUI.Update(mouse);
			}
			else
			{
				// ゲージの結果が出て、まだダメージ履歴に追加されていなければ計算する
				// ※何度も同じダメージが適用されないように、この攻撃ターンで1度だけ処理するための判定
				bool isSuccess = _gaugeUI.IsSuccess();

				CharaFormulasRow formulaRow;
				if(_SqliteCharaFormula.GetCharaFormula("最終ダメージ", formulaRow))
				{
					g_debugFormulaRaw = formulaRow.formula; // 生の公式を保存

					std::string finalExpr = formulaRow.formula;
					// 各ステータスワードを数値文字列に置換
					finalExpr = _charaFormula.ReplaceVar(finalExpr, "攻撃", afterStatus.GetAfterStatus().attack);
					finalExpr = _charaFormula.ReplaceVar(finalExpr, "会心倍率", _charaFormula.GetCriticalMultiplier(afterStatus));
					finalExpr = _charaFormula.ReplaceVar(finalExpr, "敵防御倍率", _charaFormula.GetDefenseMultiplier(afterStatus, *_enemy));
					finalExpr = _charaFormula.ReplaceVar(finalExpr, "ダメージ減衰率", _charaFormula.GetDecayRate(afterStatus));
					finalExpr = _charaFormula.ReplaceVar(finalExpr, "運値期待値", _charaFormula.GetLuckExpectation(afterStatus));

					// ゲージ倍率を末尾に掛け算
					double gaugeBonus = isSuccess ? formulaRow.successValue : formulaRow.failureValue;
					std::ostringstream ss;
					ss << "*" << std::fixed << std::setprecision(4) << gaugeBonus;
					finalExpr += ss.str();

					g_debugFormulaReplaced = finalExpr; // 画面表示用に保存

					double calculatedResult = EvaluateFormula::Evaluate(finalExpr);

					// 計算した結果を一時保存する（CharaFormula側に値を渡す）
					_charaFormula.SetFinalDamage(calculatedResult);
				}

				// もし前回の「ログ用数式文字列」をBattleCalculator側から取得できるメンバ関数（GetLastCriticalExpr等）を作ってあれば、
				// ここで `_logCriticalExpr = _calculator.GetLastCriticalExpr();` のように同期させると左側が完全に連動します。
				double damage = _charaFormula.CalculateFinalDamage(afterStatus, *_enemy, isSuccess);

				// 右側用の履歴に追加
				_damageHistory.push_back(damage);

				if(isSuccess)
				{
					// 敵のHPを実際に減らす
					_enemyCurrentHP -= damage;
					if(_enemyCurrentHP < 0) { _enemyCurrentHP = 0; }

					_gaugeUI.Reset();
				}
				else
				{
					// 失敗時のペナルティ処理など（必要であれば）
					_enemyCurrentHP -= damage; // 失敗でも一応低ダメージが入る仕様なら
					if(_enemyCurrentHP < 0) { _enemyCurrentHP = 0; }

					_gaugeUI.Reset();
				}
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

	SetFontSize(16);
	const int LEFT_X = 50;   // 左側座標
	const int RIGHT_X = 800; // 右側座標（画面サイズに合わせて調整してください）

	// ----------------------------------------------------------------
	// 🌟 【左側】数式展開デバッグ表示のリアルタイム書き換え
	// ----------------------------------------------------------------
	int leftY = 300;
	DrawString(LEFT_X, leftY, "--- [左側] 数式展開デバッグ ---", GetColor(255, 255, 255));

	leftY += 25;
	std::string rawStr = "［元の公式］: " + g_debugFormulaRaw;
	DrawString(LEFT_X, leftY, rawStr.c_str(), GetColor(180, 180, 180));

	leftY += 25;
	std::string repStr = "［代入状態］: " + g_debugFormulaReplaced;
	DrawString(LEFT_X, leftY, repStr.c_str(), GetColor(100, 255, 100)); // 置き換わった数字を緑色で分かりやすく

	leftY += 25;
	DrawFormatString(LEFT_X, leftY, GetColor(255, 200, 0), "最終公式: (攻撃 * 会心倍率 * 敵防御倍率 * ダメージ減衰率 * 運値期待値) * ゲージ倍率");


	// 👉 右側：最終ダメージ（新しいものが古いものの下へ追加される）
	int rightY = 300;
	DrawString(RIGHT_X, rightY, "--- [右側] 最終ダメージ履歴 ---", GetColor(255, 255, 255));
	rightY += 25;

	for(size_t i = 0; i < _damageHistory.size(); ++i)
	{
		// 新しい結果ほど下に描画される（iが大きくなる＝ループが後に回る＝Y座標が下に下がる）
		if(i == _damageHistory.size() - 1)
		{
			// 最新のダメージは赤色で強調
			DrawFormatString(RIGHT_X, rightY, GetColor(255, 50, 50), "Hit %02d: %.0f DMG NEW!", i + 1, _damageHistory[i]);
		}
		else
		{
			DrawFormatString(RIGHT_X, rightY, GetColor(100, 100, 100), "Hit %02d: %.0f DMG", i + 1, _damageHistory[i]);
		}
		rightY += 22; // 下にずらしていく

		// 画面外にはみ出さないための簡易セーフティ
		if(rightY > 650) { break; }
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