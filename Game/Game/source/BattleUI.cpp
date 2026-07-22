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
    using namespace UIConfig;

    if(enemy == nullptr) { return; }

	// 敵のモデルを描画する
    enemy->DrawModel();

    // フェーズに応じて画面の文字やUIの描画を切り替える
    if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Defense) 
    {
        //防御フェーズ
        SetFontSize(Font::Large);
        DrawString(Layout::Battle::PhaseMsg.x, Layout::Battle::PhaseMsg.y, "【 敵の防御ターン！ 丸を消して時間を進めろ！ 】", Color::Yellow());
        DrawFormatString(Layout::Battle::PhaseMsg.x, Layout::Battle::PhaseTimer.y, Color::Red(), "攻撃フェーズまで: %.1f 秒", battleTimer.GetTime());

        circleUI.Draw();
    }
	else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Attack)  
    {
        //攻撃フェーズ
        SetFontSize(Font::Large);
        DrawString(Layout::Battle::PhaseMsg.x, Layout::Battle::PhaseMsg.y, "【 自分の攻撃ターン！ ゲージを合わせてダメージを与えろ！ 】", Color::LightRed());
        DrawFormatString(Layout::Battle::PhaseMsg.x, Layout::Battle::PhaseTimer.y, Color::Red(), "防御フェーズまで: %.1f 秒", battleTimer.GetTime());

        gaugeUI.Draw();
    }
	else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Start)  
    {
        //戦闘開始前のカウントダウン
        SetFontSize(Font::Large);
        DrawFormatString(Layout::Battle::PhaseMsg.x, Layout::Battle::PhaseTimer.y, Color::Red(), "戦闘開始まで: %.1f 秒", battleTimer.GetTime());
    }
	else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Result)  
    {
        //戦闘結果表示
        SetFontSize(Font::ExtraLarge);
        
		// 勝敗に応じてメッセージを表示する
        if(enemyCurrentHP <= 0.0)
        {
            DrawString(Layout::Battle::ResultMsg.x, Layout::Battle::ResultTitleY, "【 勝利!!! 】", Color::Gold());
        }
        else
        {
            DrawString(Layout::Battle::ResultMsg.x, Layout::Battle::ResultTitleY, "【 敗北... 】", Color::Red());
        }

		// 最大ダメージと残りHPボーナスを表示する
        SetFontSize(Font::Large);
        DrawFormatString(Layout::Battle::ResultMsg.x, Layout::Battle::ResultMaxDmgY, Color::White(), "最大ダメージ: %.0f ダメージ", maxDamageDealt);
        DrawFormatString(Layout::Battle::ResultMsg.x, Layout::Battle::ResultHpBonusY, Color::White(), "残りHPボーナス: %.0f", (std::max)(0.0, charaCurrentHP));

		// 獲得コインを計算して表示する
        int finalGain = static_cast<int>((std::max)(0.0, charaCurrentHP) + maxDamageDealt);
        DrawFormatString(Layout::Battle::ResultMsg.x, Layout::Battle::ResultCoinY, Color::White(), "獲得コイン: + %d !", finalGain);

		// 次の画面への移動までの時間を表示する
        SetFontSize(Font::Small);
        DrawFormatString(Layout::Battle::ResultMsg.x, Layout::Battle::ResultNextSceneY, Color::Gray(), "間もなく次の画面へ移動します... (%.1f)", battleTimer.GetTime());
        return;
    }

    // 敵が存在すれば、画面上部に敵の情報とHPバーを表示する
    if(enemy)
    {
        // 敵の名前とレベルを描画
        SetFontSize(Font::Medium);
        DrawFormatString(Layout::Battle::EnemyPos.x, Layout::Battle::EnemyNameY, Color::TextGray(), "%s  (Lv.%d)", enemy->GetName().c_str(), enemy->GetLevel());

        // HPバーの枠を描画
        DrawBox(Layout::Battle::EnemyPos.x, Layout::Battle::EnemyBarTop, Layout::Battle::EnemyPos.x + Common::HPBar.w, Layout::Battle::EnemyBarBottom, Color::Gray(), FALSE);

        // 敵の残りHPの割合に応じて緑色のバーを描画
        double hpRate = enemyCurrentHP / (std::max)(1.0, enemy->GetHP());
        int barWidth = static_cast<int>(Common::HPBar.w * hpRate);
        DrawBox(Layout::Battle::EnemyPos.x, Layout::Battle::EnemyBarTop, Layout::Battle::EnemyPos.x + barWidth, Layout::Battle::EnemyBarBottom, Color::Green(), TRUE);

        // HPの数値テキスト表示
        DrawFormatString(Layout::Battle::EnemyPos.x, Layout::Battle::EnemyHpTextY, Color::TextGray(), "HP: %.0f / %.0f", enemyCurrentHP, enemy->GetHP());
    }

    // 右側：最終ダメージ履歴
    SetFontSize(Font::Small);    
    int rightY = Layout::Battle::HistoryPos.y;
    DrawString(Layout::Battle::HistoryPos.x, rightY, "--- 最終ダメージ履歴 ---", Color::Gray());
    rightY += Common::HistoryTitleSpacing;

	// ダメージ履歴を上から順に描画する
    for(size_t i = 0; i < damageHistory.size(); ++i)
    {
        if(i == damageHistory.size() - 1)
        {
            DrawFormatString(Layout::Battle::HistoryPos.x, rightY, Color::White(), " %.0f ダメージ!", damageHistory[i]);
        }
        else
        {
            DrawFormatString(Layout::Battle::HistoryPos.x, rightY, Color::LightBlue(), " %.0f ダメージ!", damageHistory[i]);
        }
        rightY += Common::HistoryLineSpacing;

        if(rightY > Layout::Battle::HistoryMaxY) { break; }
    }

    // プレイヤーのHPバーを描画する
    {
        double maxHp = afterStatus.GetAfterStatus().hp;
        if(maxHp < 1.0) { maxHp = 1.0; }

        SetFontSize(Font::Medium);
        DrawString(Layout::Battle::PlayerPos.x, Layout::Battle::PlayerNameY, "プレイヤー", Color::LightBlue());
        DrawBox(Layout::Battle::PlayerPos.x, Layout::Battle::PlayerBarTop, Layout::Battle::PlayerPos.x + Common::HPBar.w, Layout::Battle::PlayerBarBottom, Color::Gray(), FALSE);

        double playerHpRate = charaCurrentHP / maxHp;
        int playerBarWidth = static_cast<int>(Common::HPBar.w * playerHpRate);

        unsigned int barColor = (playerHpRate > 0.2) ? Color::HPBlue() : Color::Red();
        DrawBox(Layout::Battle::PlayerPos.x, Layout::Battle::PlayerBarTop, Layout::Battle::PlayerPos.x + playerBarWidth, Layout::Battle::PlayerBarBottom, barColor, TRUE);

        DrawFormatString(Layout::Battle::PlayerPos.x, Layout::Battle::PlayerHpTextY, Color::White(), "HP: %.0f / %.0f", charaCurrentHP, maxHp);
    }

    // 被ダメージ時の画面フラッシュ
    if(damageFlashTimer > 0.0)
    {
		// フラッシュの透明度を計算し、0から最大値までの範囲に制限する
        int alpha = static_cast<int>((damageFlashTimer / Param::FlashTime) * Param::FlashAlpha);
        alpha = std::clamp(alpha, 0, Param::MaxAlphaByte);

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        int screenW, screenH;
        GetDrawScreenSize(&screenW, &screenH);
        DrawBox(0, 0, screenW, screenH, Color::Red(), TRUE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}