#include "DatabaseSelectorButtonUI.h"
#include "container.h"

const ButtonUI::ButtonRect& DatabaseSelectorButtonUI::GetDbSelectButtonRect() const
{
	return _dbSelectButton;
}

void DatabaseSelectorButtonUI::Update(MouseInput& mouse)
{
	// データベース選択ボタンのクリック判定
	const auto& btn = GetDbSelectButtonRect();
	_isDbSelectClicked = mouse.IsLeftTrig() && mouse.IsInRect(btn.x, btn.y, btn.w, btn.h);
}

void DatabaseSelectorButtonUI::Draw() const
{
	DrawDbSelectButton();
}

void DatabaseSelectorButtonUI::DrawDbSelectButton() const
{
	ButtonUI::DrawButton(_dbSelectButton, "セーブデータ選択");
}