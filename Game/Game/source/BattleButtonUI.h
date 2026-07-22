#pragma once
#include "ButtonUI.h"
#include "MouseInput.h"

class BattleButtonUI
{
public:
	const ButtonUI::ButtonRect& GetBattleButtonRect() const { return _battleButton; }
	bool IsBattleClicked() const { return _isBattleClicked; }
	void Update(MouseInput& mouse);
	void Draw() const;

private:
	void DrawBattleButton() const;

	// バトルボタンの矩形
	ButtonUI::ButtonRect _battleButton{ 360, 40, 160, 40 };

private:
	bool _isBattleClicked = false;	// バトルボタンがクリックされたか
};

