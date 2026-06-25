#include "BattleUI.h"

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
    if(enemy == nullptr) { return; }

    enemy->DrawModel();

    // フェーズに応じて画面の文字やUIの描画を切り替える
    if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Defense)
    {
        SetFontSize(24);
        DrawString(100, 50, "【 敵の防御ターン！ 丸を消して時間を進めろ！ 】", GetColor(255, 255, 0));
        DrawFormatString(100, 80, GetColor(255, 0, 0), "攻撃フェーズまで: %.1f 秒", battleTimer.GetTime());

        circleUI.Draw();
    }
    else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Attack)
    {
        SetFontSize(24);
        DrawString(100, 50, "【 自分の攻撃ターン！ ゲージを合わせてダメージを与えろ！ 】", GetColor(255, 50, 50));
        DrawFormatString(100, 80, GetColor(255, 0, 0), "防御フェーズまで: %.1f 秒", battleTimer.GetTime());

        gaugeUI.Draw();
    }
    else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Start)
    {
        SetFontSize(24);
        DrawFormatString(100, 80, GetColor(255, 0, 0), "戦闘開始まで: %.1f 秒", battleTimer.GetTime());
    }
    else if(battleTimer.GetCurrentPhase() == BattleTimer::BattlePhase::Result)
    {
        SetFontSize(36);
        if(enemyCurrentHP <= 0.0)
        {
            DrawString(200, 250, "【 勝利!!! 】", GetColor(255, 215, 0));
        }
        else
        {
            DrawString(200, 250, "【 敗北... 】", GetColor(255, 50, 50));
        }

        SetFontSize(24);
        DrawFormatString(200, 330, GetColor(255, 255, 255), "最大ダメージ: %.0f ダメージ", maxDamageDealt);
        DrawFormatString(200, 370, GetColor(255, 255, 255), "残りHPボーナス: %.0f", (std::max)(0.0, charaCurrentHP));

        int finalGain = static_cast<int>((std::max)(0.0, charaCurrentHP) + maxDamageDealt);
        DrawFormatString(200, 420, GetColor(255, 255, 255), "獲得コイン: + %d !", finalGain);

        SetFontSize(18);
        DrawFormatString(200, 500, GetColor(150, 150, 150), "間もなく次の画面へ移動します... (%.1f)", battleTimer.GetTime());
        return;
    }

    // 敵が存在すれば、画面上部に敵の情報とHPバーを表示する
    if(enemy)
    {
        // 敵の名前とレベルを描画
        SetFontSize(20);
        DrawFormatString(100, 140, GetColor(100, 100, 100), "%s  (Lv.%d)", enemy->GetName().c_str(), enemy->GetLevel());

        // HPバーの枠を描画
        DrawBox(100, 170, 500, 190, GetColor(100, 100, 100), FALSE);

        // 敵の残りHPの割合に応じて緑色のバーを描画
        double hpRate = enemyCurrentHP / (std::max)(1.0, enemy->GetHP());
        int barWidth = static_cast<int>(400 * hpRate);
        DrawBox(100, 170, 100 + barWidth, 190, GetColor(0, 255, 0), TRUE);

        // HPの数値テキスト表示
        DrawFormatString(100, 200, GetColor(200, 200, 200), "HP: %.0f / %.0f", enemyCurrentHP, enemy->GetHP());
    }

    SetFontSize(16);
    const int LEFT_X = 50;
    const int RIGHT_X = 900;

    // 右側：最終ダメージ履歴
    int rightY = 100;
    DrawString(RIGHT_X, rightY, "--- [右側] 最終ダメージ履歴 ---", GetColor(100, 100, 100));
    rightY += 25;

    for(size_t i = 0; i < damageHistory.size(); ++i)
    {
        if(i == damageHistory.size() - 1)
        {
            DrawFormatString(RIGHT_X, rightY, GetColor(255, 255, 255), "Hit %02d: %.0f ダメージ!", i + 1, damageHistory[i]);
        }
        else
        {
            DrawFormatString(RIGHT_X, rightY, GetColor(120, 200, 255), "Hit %02d: %.0f ダメージ!", i + 1, damageHistory[i]);
        }
        rightY += 22;

        if(rightY > 650) { break; }
    }

    // プレイヤーのHPバーを描画する
    {
        double maxHp = afterStatus.GetAfterStatus().hp;
        if(maxHp < 1.0) { maxHp = 1.0; }

        SetFontSize(20);
        DrawString(100, 240, "プレイヤー", GetColor(120, 200, 255));
        DrawBox(100, 270, 500, 290, GetColor(100, 100, 100), FALSE);

        double playerHpRate = charaCurrentHP / maxHp;
        int playerBarWidth = static_cast<int>(400 * playerHpRate);

        unsigned int barColor = (playerHpRate > 0.2) ? GetColor(50, 150, 255) : GetColor(255, 50, 50);
        DrawBox(100, 270, 100 + playerBarWidth, 290, barColor, TRUE);

        DrawFormatString(100, 300, GetColor(255, 255, 255), "HP: %.0f / %.0f", charaCurrentHP, maxHp);
    }

    // 被ダメージ時の画面フラッシュ
    if(damageFlashTimer > 0.0)
    {
        int alpha = static_cast<int>((damageFlashTimer / 0.2) * 120);
        if(alpha > 255) alpha = 255;
        if(alpha < 0)   alpha = 0;

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        int screenW, screenH;
        GetDrawScreenSize(&screenW, &screenH);
        DrawBox(0, 0, screenW, screenH, GetColor(255, 0, 0), TRUE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}