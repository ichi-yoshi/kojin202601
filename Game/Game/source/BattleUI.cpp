#include "BattleUI.h"
#include "MagicNumberConfig.h"

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
    // 名前空間の使用宣言
    using namespace FontSize;
    using namespace Color;
    using namespace Layout;
    using namespace SystemParam;

    if(enemy == nullptr) { return; }

	// 敵のモデルを描画する
    enemy->DrawModel();

    // フェーズに応じて画面の文字やUIの描画を切り替える
    if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Defense) 
    {
        //防御フェーズ
        SetFontSize(FontSize::Large);
        DrawString(Layout::PhaseMsgX, Layout::PhaseMsgY, "【 敵の防御ターン！ 丸を消して時間を進めろ！ 】", Color::Yellow());
        DrawFormatString(Layout::PhaseMsgX, Layout::PhaseTimerY, Color::Red(), "攻撃フェーズまで: %.1f 秒", battleTimer.GetTime());

        circleUI.Draw();
    }
	else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Attack)  
    {
        //攻撃フェーズ
        SetFontSize(FontSize::Large);
        DrawString(Layout::PhaseMsgX, Layout::PhaseMsgY, "【 自分の攻撃ターン！ ゲージを合わせてダメージを与えろ！ 】", Color::LightRed());
        DrawFormatString(Layout::PhaseMsgX, Layout::PhaseTimerY, Color::Red(), "防御フェーズまで: %.1f 秒", battleTimer.GetTime());

        gaugeUI.Draw();
    }
	else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Start)  
    {
        //戦闘開始前のカウントダウン
        SetFontSize(FontSize::Large);
        DrawFormatString(Layout::PhaseMsgX, Layout::PhaseTimerY, Color::Red(), "戦闘開始まで: %.1f 秒", battleTimer.GetTime());
    }
	else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Result)  
    {
        //戦闘結果表示
        SetFontSize(FontSize::ExtraLarge);
        
		// 勝敗に応じてメッセージを表示する
        if(enemyCurrentHP <= 0.0)
        {
            DrawString(Layout::ResultMsgX, Layout::ResultTitleY, "【 勝利!!! 】", Color::Gold());
        }
        else
        {
            DrawString(Layout::ResultMsgX, Layout::ResultTitleY, "【 敗北... 】", Color::Red());
        }

		// 最大ダメージと残りHPボーナスを表示する
        SetFontSize(FontSize::Large);
        DrawFormatString(Layout::ResultMsgX, Layout::ResultMaxDmgY, Color::White(), "最大ダメージ: %.0f ダメージ", maxDamageDealt);
        DrawFormatString(Layout::ResultMsgX, Layout::ResultHpBonusY, Color::White(), "残りHPボーナス: %.0f", (std::max)(0.0, charaCurrentHP));

		// 獲得コインを計算して表示する
        int finalGain = static_cast<int>((std::max)(0.0, charaCurrentHP) + maxDamageDealt);
        DrawFormatString(Layout::ResultMsgX, Layout::ResultCoinY, Color::White(), "獲得コイン: + %d !", finalGain);

		// 次の画面への移動までの時間を表示する
        SetFontSize(FontSize::Small);
        DrawFormatString(Layout::ResultMsgX, Layout::ResultNextSceneY, Color::Gray(), "間もなく次の画面へ移動します... (%.1f)", battleTimer.GetTime());
        return;
    }

    // 敵が存在すれば、画面上部に敵の情報とHPバーを表示する
    if(enemy)
    {
        // 敵の名前とレベルを描画
        SetFontSize(FontSize::Medium);
        DrawFormatString(Layout::EnemyInfoX, Layout::EnemyNameY, Color::TextGray(), "%s  (Lv.%d)", enemy->GetName().c_str(), enemy->GetLevel());

        // HPバーの枠を描画
        DrawBox(Layout::EnemyInfoX, Layout::EnemyBarTop, Layout::EnemyInfoX + Layout::EnemyBarWidth, Layout::EnemyBarBottom, Color::Gray(), FALSE);

        // 敵の残りHPの割合に応じて緑色のバーを描画
        double hpRate = enemyCurrentHP / (std::max)(1.0, enemy->GetHP());
        int barWidth = static_cast<int>(Layout::EnemyBarWidth * hpRate);
        DrawBox(Layout::EnemyInfoX, Layout::EnemyBarTop, Layout::EnemyInfoX + barWidth, Layout::EnemyBarBottom, Color::Green(), TRUE);

        // HPの数値テキスト表示
        DrawFormatString(Layout::EnemyInfoX, Layout::EnemyHpTextY, Color::TextGray(), "HP: %.0f / %.0f", enemyCurrentHP, enemy->GetHP());
    }

    // 右側：最終ダメージ履歴
    SetFontSize(FontSize::Small);    
    int rightY = Layout::DHistoryStartY;
    DrawString(Layout::DHistoryX, rightY, "--- 最終ダメージ履歴 ---", Color::Gray());
    rightY += Layout::DHistoryTitleSpacing;

	// ダメージ履歴を上から順に描画する
    for(size_t i = 0; i < damageHistory.size(); ++i)
    {
        if(i == damageHistory.size() - 1)
        {
            DrawFormatString(Layout::DHistoryX, rightY, Color::White(), " %.0f ダメージ!", damageHistory[i]);
        }
        else
        {
            DrawFormatString(Layout::DHistoryX, rightY, Color::LightBlue(), " %.0f ダメージ!", damageHistory[i]);
        }
        rightY += Layout::DHistoryLineSpacing;

        if(rightY > Layout::DHistoryMaxY) { break; }
    }

    // プレイヤーのHPバーを描画する
    {
        double maxHp = afterStatus.GetAfterStatus().hp;
        if(maxHp < 1.0) { maxHp = 1.0; }

        SetFontSize(FontSize::Medium);
        DrawString(Layout::PlayerInfoX, Layout::PlayerNameY, "プレイヤー", Color::LightBlue());
        DrawBox(Layout::PlayerInfoX, Layout::PlayerBarTop, Layout::PlayerInfoX + Layout::PlayerBarWidth, Layout::PlayerBarBottom, Color::Gray(), FALSE);

        double playerHpRate = charaCurrentHP / maxHp;
        int playerBarWidth = static_cast<int>(Layout::PlayerBarWidth * playerHpRate);

        unsigned int barColor = (playerHpRate > 0.2) ? Color::HPBlue() : Color::Red();
        DrawBox(Layout::PlayerInfoX, Layout::PlayerBarTop, Layout::PlayerInfoX + playerBarWidth, Layout::PlayerBarBottom, barColor, TRUE);

        DrawFormatString(Layout::PlayerInfoX, Layout::PlayerHpTextY, Color::White(), "HP: %.0f / %.0f", charaCurrentHP, maxHp);
    }

    // 被ダメージ時の画面フラッシュ
    if(damageFlashTimer > 0.0)
    {
		// フラッシュの透明度を計算し、0から最大値までの範囲に制限する
        int alpha = static_cast<int>((damageFlashTimer / SystemParam::FlashMaxTime) * SystemParam::FlashMaxAlpha);
        alpha = std::clamp(alpha, 0, SystemParam::MaxAlphaByte);

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        int screenW, screenH;
        GetDrawScreenSize(&screenW, &screenH);
        DrawBox(0, 0, screenW, screenH, Color::Red(), TRUE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}