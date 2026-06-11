#pragma once
#include "ButtonUI.h"
#include "MouseInput.h"

class BattleUI
{
public:
	const ButtonUI::ButtonRect& GetBattleButtonRect() const;

	void Update(MouseInput& mouse);
	void Draw() const;
private:
	void DrawBattleButton() const;
	ButtonUI::ButtonRect _battleButton{ 360, 40, 160, 40 };
};

