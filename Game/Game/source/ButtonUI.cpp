#include "ButtonUI.h"
#include "DxLib.h"

// ボタンの描画
void ButtonUI::DrawButton(const ButtonRect& rect, const std::string& label)
{
	DrawBox(rect.x, rect.y,
		rect.x + rect.w, rect.y + rect.h,
		GetColor(200, 200, 200), TRUE);

	DrawString(rect.x + 10, rect.y + 10, label.c_str(), GetColor(0, 0, 0));
}

