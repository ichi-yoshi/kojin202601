#include "BattleUI.h"
#include "MagicNumberConfig.h"

// 名前空間の使用宣言
using namespace UIConfig;

void BattleUI::Render(
    CharaAfterStatus& afterStatus,
    Enemy* enemy,
    double enemyCurrentHP,
    double charaCurrentHP,
    BattleTimer& battleTimer,
    CircleUI& circleUI,
    GaugeUI& gaugeUI,
    double maxDamageDealt,
    const std::vector<double>& damageHistory,
    double damageFlashTimer)
{
	// 敵が存在しない場合は描画を行わない
    if(enemy == nullptr) { return; }

    // 敵が存在すれば、画面上部に敵の情報とHPバーを表示する
    if(enemy)
    {
        // 敵のモデルを描画する
        enemy->DrawModel();

        int enemyPosX = Layout::Battle::EnemyPos.x;             // 敵の情報表示の基準位置X座標
        int enemyBarTop = Layout::Battle::EnemyBarTop;          // 敵のHPバーの上端Y座標
        int enemyBarBottom = Layout::Battle::EnemyBarBottom;    // 敵のHPバーの下端Y座標

        // 敵の名前とレベルを描画
        SetFontSize(Font::Medium);
        DrawFormatString(enemyPosX, Layout::Battle::EnemyNameY,
            Color::TextGray(), "%s  (Lv.%d)",
            enemy->GetName().c_str(), enemy->GetLevel());

        // HPバーの枠を描画
        DrawBox(enemyPosX, enemyBarTop, enemyPosX + Common::HPBar.w,
            enemyBarBottom, Color::Gray(), FALSE);

        // 敵の残りHPの割合に応じて緑色のバーを描画
        double hpRate = enemyCurrentHP / (std::max)(1.0, enemy->GetHP());
        int barWidth = static_cast<int>(Common::HPBar.w * hpRate);
        DrawBox(enemyPosX, enemyBarTop, enemyPosX + barWidth,
            enemyBarBottom, Color::Green(), TRUE);

        // HPの数値テキスト表示
        DrawFormatString(enemyPosX, Layout::Battle::EnemyHpTextY,
            Color::TextGray(), "HP: %.0f / %.0f", enemyCurrentHP, enemy->GetHP());
    }

	// バトルフェーズの描画
    {
        int phaseMsgX = Layout::Battle::PhaseMsg.x;     // フェーズメッセージの表示位置X座標
        int phaseMsgY = Layout::Battle::PhaseMsg.y;     // フェーズメッセージの表示位置Y座標
        int phaseTimerY = Layout::Battle::PhaseTimer.y; // フェーズタイマーの表示位置Y座標
        int resultMsgX = Layout::Battle::ResultMsg.x;   // 結果メッセージの表示位置X座標

        // フェーズに応じて画面の文字やUIの描画を切り替える
        if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Defense)
        {
            // 防御フェーズ
            // 防御フェーズのメッセージを描画
            SetFontSize(Font::Large);
            DrawString(phaseMsgX, phaseMsgY,
                "【 敵の防御ターン！ 丸を消して時間を進めろ！ 】",
                Color::LightRed());

            // 防御フェーズの残り時間を描画
            DrawFormatString(phaseMsgX, phaseTimerY,
                Color::Red(),
                "攻撃フェーズまで: %.1f 秒",
                battleTimer.GetTime());

            circleUI.Draw();
        }
        else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Attack)
        {
            // 攻撃フェーズ
            // 攻撃フェーズのメッセージを描画
            SetFontSize(Font::Large);
            DrawString(phaseMsgX, phaseMsgY,
                "【 自分の攻撃ターン！ ゲージを合わせてダメージを与えろ！ 】",
                Color::LightRed());

            // 攻撃フェーズの残り時間を描画
            DrawFormatString(phaseMsgX, phaseTimerY,
                Color::Red(),
                "防御フェーズまで: %.1f 秒",
                battleTimer.GetTime());

            gaugeUI.Draw();
        }
        else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Start)
        {
            // 戦闘開始前のカウントダウン
            // 戦闘開始前のメッセージを描画
            SetFontSize(Font::Large);
            DrawFormatString(phaseMsgX, phaseTimerY,
                Color::Red(),
                "戦闘開始まで: %.1f 秒",
                battleTimer.GetTime());
        }
        else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Result)
        {
            // 戦闘結果表示
            // 勝敗に応じてメッセージを表示する
            SetFontSize(Font::ExtraLarge);
            if(enemyCurrentHP <= 0.0)
            {
                DrawString(resultMsgX, Layout::Battle::ResultTitleY,
                    "【 勝利!!! 】",
                    Color::Gold());
            }
            else
            {
                DrawString(resultMsgX, Layout::Battle::ResultTitleY,
                    "【 敗北... 】",
                    Color::Red());
            }

            SetFontSize(Font::Large);

            // 最大ダメージを表示する
            DrawFormatString(resultMsgX, Layout::Battle::ResultMaxDmgY,
                Color::White(), "最大ダメージ: %.0f ダメージ",
                maxDamageDealt);

            // 残りHPボーナスを表示する
            DrawFormatString(resultMsgX, Layout::Battle::ResultHpBonusY,
                Color::White(), "残りHPボーナス: %.0f",
                (std::max)(0.0, charaCurrentHP));

            // 獲得コインを表示する
            int finalGain = static_cast<int>((std::max)(0.0, charaCurrentHP) + maxDamageDealt);
            DrawFormatString(resultMsgX, Layout::Battle::ResultCoinY,
                Color::White(), "獲得コイン: + %d !",
                finalGain);

            // 次の画面への移動までの時間を表示する
            SetFontSize(Font::Small);
            DrawFormatString(resultMsgX, Layout::Battle::ResultNextSceneY,
                Color::Gray(), "間もなく次の画面へ移動します... (%.1f)",
                battleTimer.GetTime());
            return;
        }
    }

    // 右側：最終ダメージ履歴
    {
        SetFontSize(Font::Small);
        int historyX = Layout::Battle::HistoryPos.x;
        int historyY = Layout::Battle::HistoryPos.y;

        DrawString(historyX, historyY, "--- 最終ダメージ履歴 ---", Color::Gray());
        historyY += Common::RowSpacing;

        // ダメージ履歴を上から順に描画する
        for(size_t i = 0; i < damageHistory.size(); ++i)
        {
			// 最後のダメージ履歴は白色で強調表示する
            if(i == damageHistory.size() - 1)
            {
                DrawFormatString(historyX, historyY,
                    Color::White(), " %.0f ダメージ!", damageHistory[i]);
            }
            else
            {
                DrawFormatString(historyX, historyY,
                    Color::LightBlue(), " %.0f ダメージ!", damageHistory[i]);
            }
            historyY += Common::RowSpacing;

            if(historyY > Layout::Battle::HistoryMaxY) { break; }
        }
    }

    // プレイヤーのHPバーを描画する
    {
        int playerPosX = Layout::Battle::PlayerPos.x;           // プレイヤーの情報表示の基準位置X座標
        int playerBarTop = Layout::Battle::PlayerBarTop;        // プレイヤーのHPバーの上端Y座標
        int playerBarBottom = Layout::Battle::PlayerBarBottom;  // プレイヤーのHPバーの下端Y座標

		// プレイヤーの最大HPを取得する
        double maxHp = afterStatus.GetAfterStatus().hp;

		// プレイヤーの名前を描画する
        SetFontSize(Font::Medium);
        DrawString(playerPosX, Layout::Battle::PlayerNameY, 
            "プレイヤー", Color::LightBlue());

        DrawBox(playerPosX, playerBarTop, playerPosX + Common::HPBar.w,
            playerBarBottom, Color::Gray(), FALSE);

		// プレイヤーのHPの割合を計算し、HPバーの幅を決定する
        double playerHpRate = charaCurrentHP / maxHp;
        int playerBarWidth = static_cast<int>(Common::HPBar.w * playerHpRate);

        // HPが一定%以下なら赤色にする
		unsigned int barColor = (playerHpRate > Param::HpPercentThreshold) ? Color::HPBlue() : Color::Red();  
        DrawBox(playerPosX, playerBarTop, playerPosX + playerBarWidth, 
            playerBarBottom, barColor, TRUE);

        DrawFormatString(playerPosX, Layout::Battle::PlayerHpTextY,
            Color::White(), "HP: %.0f / %.0f", charaCurrentHP, maxHp);
    }

    // 被ダメージ時の画面フラッシュ
    if(damageFlashTimer > 0.0)
    {
		// フラッシュの透明度を計算し、0から最大値までの範囲に制限する
        int alpha = static_cast<int>((damageFlashTimer / Param::FlashTime) * Alpha::Semi);
        alpha = std::clamp(alpha, 0, Alpha::Max);

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        int screenW, screenH;
        GetDrawScreenSize(&screenW, &screenH);
        DrawBox(0, 0, screenW, screenH, Color::Red(), TRUE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, Alpha::Min);
    }
}