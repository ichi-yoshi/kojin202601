#include "BattleUI.h"
#include "DxLib.h"

const ButtonUI::ButtonRect& BattleUI::GetBattleButtonRect() const
{
	return _battleButton;
}

void BattleUI::Update(MouseInput& mouse)
{
	// バトルボタンのクリック判定
	const auto& btn = GetBattleButtonRect();
	_isBattleClicked = mouse.IsLeftTrig() && mouse.IsInRect(btn.x, btn.y, btn.w, btn.h);
}

void BattleUI::Draw() const
{
	DrawBattleButton();
}

void BattleUI::DrawBattleButton() const
{
	ButtonUI::DrawButton(_battleButton, "バトル");
}