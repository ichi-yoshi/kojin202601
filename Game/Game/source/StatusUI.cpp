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
	const auto rows = afterStatus.ToRows();
	if(rows.empty()) { return; }

	int fontSize = Font::Normal;
	int rowHeight = fontSize + Common::RowSpacingExtra;
	int padding = Common::DefaultPadding;
	int rowSpace = Common::RowSpacingExtra;
	int boxW = Layout::StatusBoxWidth;
	int boxH = static_cast<int>(rows.size()) * rowHeight + padding * rowSpace;	// ボックスの高さを計算

	int x = Layout::StatusBoxPos.x;
	int y = Layout::StatusBoxPos.y;

	DrawBox(x, y, x + boxW, y + boxH, Color::BoxBg(), TRUE);

	SetFontSize(fontSize);

	// テキストの描画開始位置を計算
	int textY = y + padding;

	// 各行の文字列を描画
	for(const auto& row : rows)
	{
		DrawString(x + padding, textY, row.c_str(), Color::Black());
		textY += rowHeight;
	}
}
