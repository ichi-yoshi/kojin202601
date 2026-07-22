#pragma once
#include "CharaAfterStatus.h"
#include "Enemy.h"
#include "BattleTimer.h"
#include "CircleUI.h"
#include "GaugeUI.h"
#include "container.h"

class BattleUI
{
public:
    void Render(
		CharaAfterStatus& afterStatus,              // 計算後のプレイヤーのステータス
		Enemy* enemy,                               // 敵
		double enemyCurrentHP,                      // 敵の現在HP
		double charaCurrentHP,                      // プレイヤーの現在HP
		BattleTimer& battleTimer,                   // 戦闘タイマー
		CircleUI& circleUI,                         // 円UI
		GaugeUI& gaugeUI,							// ゲージUI
		double maxDamageDealt,						// 最大ダメージ
		const std::vector<double>& damageHistory,	// ダメージ履歴
		double damageFlashTimer						// 攻撃を受けたときの点滅タイマー
    );
};

