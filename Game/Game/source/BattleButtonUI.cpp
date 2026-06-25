#include "BattleButtonUI.h"
#include "container.h"

const ButtonUI::ButtonRect& BattleButtonUI::GetBattleButtonRect() const
{
	return _battleButton;
}

void BattleButtonUI::Update(MouseInput& mouse)
{
	// バトルボタンのクリック判定
	const auto& btn = GetBattleButtonRect();
	_isBattleClicked = mouse.IsLeftTrig() && mouse.IsInRect(btn.x, btn.y, btn.w, btn.h);
}

void BattleButtonUI::Draw() const
{
	DrawBattleButton();
}

void BattleButtonUI::DrawBattleButton() const
{
	ButtonUI::DrawButton(_battleButton, "バトル");
}