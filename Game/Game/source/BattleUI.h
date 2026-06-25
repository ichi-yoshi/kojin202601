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
        CharaAfterStatus& afterStatus,
        Enemy* enemy,
        double enemyCurrentHP,
        double charaCurrentHP,
        BattleTimer& battleTimer,
        CircleUI& circleUI,
        GaugeUI& gaugeUI,
        double maxDamageDealt,
        const std::vector<double>& damageHistory,
        double damageFlashTimer
    );
};

