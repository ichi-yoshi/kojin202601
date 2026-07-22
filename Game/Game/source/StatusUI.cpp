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

	int fontSize = Font::Normal;			// フォントサイズを取得
	int rowSpace = Common::RowSpacingExtra;	// 行間の追加スペースを取得
	int rowHeight = fontSize + rowSpace;	// 行の高さを計算
	int padding = Common::DefaultPadding;	// ボックス内の余白を取得
	int boxW = Layout::StatusBoxWidth;		// ボックスの幅を取得
	int boxH = static_cast<int>(rows.size()) * rowHeight + padding * rowSpace;	// ボックスの高さを計算

	int x = Layout::StatusBoxPos.x;	// ボックスのX座標を取得
	int y = Layout::StatusBoxPos.y;	// ボックスのY座標を取得

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
