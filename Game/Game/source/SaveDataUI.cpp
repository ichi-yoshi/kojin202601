#include "SaveDataUI.h"
#include "DxLib.h"

const ButtonUI::ButtonRect& SaveDataUI::GetSaveDataButtonRect() const
{
	return _saveDataButton;
}

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
	ButtonUI::DrawButton(_saveDataButton, "セーブデータ[S]");
}

void SaveDataUI::DrawSaveDataBox(const SaveData& saveData) const
{
	const auto lines = saveData.ToLines();
	if(lines.empty()) { return; }

	int fontSize = 18;
	int lineHeight = fontSize + 4;
	int padding = 12;
	int boxW = 260; 
	int boxH = static_cast<int>(lines.size()) * lineHeight + padding * 2;

	int x = 520;	
	int y = 100;    

	// 背景ボックスの描画
	DrawBox(x, y, x + boxW, y + boxH, GetColor(235, 235, 235), TRUE);
	SetFontSize(fontSize);

	int textY = y + padding;
	for(const auto& line : lines)
	{
		DrawString(x + padding, textY, line.c_str(), GetColor(0, 0, 0));
		textY += lineHeight;
	}
}