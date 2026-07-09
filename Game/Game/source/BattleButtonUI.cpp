#include "BattleButtonUI.h"
#include "container.h"

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