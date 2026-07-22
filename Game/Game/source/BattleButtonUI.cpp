#include "BattleButtonUI.h"
#include "container.h"

// バトルボタンのクリック判定
void BattleButtonUI::Update(MouseInput& mouse)
{
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