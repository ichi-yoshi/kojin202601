#include "StatusUI.h"

const ButtonUI::ButtonRect& StatusUI::GetCharaButtonRect() const
{
	return _charaButton;
}

void StatusUI::Update(MouseInput& mouse)
{
	const auto& charaBtn = GetCharaButtonRect();
	_isCharaClicked = mouse.IsLeftTrig() && mouse.IsInRect(_charaButton.x, _charaButton.y, _charaButton.w, _charaButton.h);
}

void StatusUI::Draw(const CharaAfterStatus& afterStatus, bool visible) const
{
	DrawCharaButton();
	if(visible)
	{
		DrawStatusBox(afterStatus);
	}
}

void StatusUI::DrawCharaButton() const
{
	ButtonUI::DrawButton(_charaButton, "キャラ");
}

void StatusUI::DrawStatusBox(const CharaAfterStatus& afterStatus) const
{
	const auto lines = afterStatus.ToLines();
	if(lines.empty()) { return; }

	int fontSize = 18;
	int lineHeight = fontSize + 2;
	int padding = 8;
	int boxW = 260;
	int boxH = static_cast<int>(lines.size()) * lineHeight + padding * 2;

	int x = 20;	// 左側
	int y = 100;

	DrawBox(x, y, x + boxW, y + boxH, GetColor(235, 235, 235), TRUE);
	SetFontSize(fontSize);

	int textY = y + padding;
	for(const auto& line : lines)
	{
		DrawString(x + padding, textY, line.c_str(), GetColor(0, 0, 0));
		textY += lineHeight;
	}
}
