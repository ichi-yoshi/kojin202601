#include "StatusUI.h"
#include "MagicNumberConfig.h"

// 名前空間の使用の宣言
using namespace UIConfig;

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

	int fontSize = Font::Normal;
	int lineHeight = fontSize + Common::LineSpacingExtra;
	int padding = Common::DefaultPadding;
	int boxW = Layout::StatusBoxWidth;
	int boxH = static_cast<int>(lines.size()) * lineHeight + padding * 2;

	int x = Layout::StatusBoxPos.x;
	int y = Layout::StatusBoxPos.y;

	DrawBox(x, y, x + boxW, y + boxH, Color::BoxBg(), TRUE);
	SetFontSize(fontSize);

	int textY = y + padding;
	for(const auto& line : lines)
	{
		DrawString(x + padding, textY, line.c_str(), Color::Black());
		textY += lineHeight;
	}
}
