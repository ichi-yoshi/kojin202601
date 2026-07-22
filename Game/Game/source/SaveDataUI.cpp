#include "SaveDataUI.h"
#include "MagicNumberConfig.h"

// 名前空間の使用宣言
using namespace UIConfig;

void SaveDataUI::Update(MouseInput& mouse)
{
	const auto& btn = GetSaveDataButtonRect();
	_isSaveDataClicked = mouse.IsLeftTrig() && mouse.IsInRect(btn.x, btn.y, btn.w, btn.h);
}

void SaveDataUI::Draw(const SaveData& saveData, bool visible) const
{
	DrawSaveDataButton();

	if(visible)
	{
		DrawSaveDataBox(saveData);
	}
}

void SaveDataUI::DrawSaveDataButton() const
{
	ButtonUI::DrawButton(_saveDataButton, "セーブデータ");
}

void SaveDataUI::DrawSaveDataBox(const SaveData& saveData) const
{
	// セーブデータの行を取得
	const auto rows = saveData.ToRows();
	if(rows.empty()) { return; }

	int fontSize = Font::Normal;
	int rowSpace = Common::RowSpacingExtra;
	int rowHeight = fontSize + rowSpace;
	int padding = Common::DefaultPadding;
	int boxW = Layout::Battle::SaveDataWidth;
	int boxH = static_cast<int>(rows.size()) * rowHeight + padding * rowSpace;

	int x = Layout::Battle::SaveDataPos.x;
	int y = Layout::Battle::SaveDataPos.y;

	// 背景ボックスの描画
	DrawBox(x, y, x + boxW, y + boxH, Color::BoxBg(), TRUE);
	SetFontSize(fontSize);

	int textY = y + padding;
	for(const auto& row : rows)
	{
		DrawString(x + padding, textY, row.c_str(), Color::Black());
		textY += rowHeight;
	}
}