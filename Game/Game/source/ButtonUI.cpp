#include "ButtonUI.h"
#include "MagicNumberConfig.h"

// ボタンの描画
void ButtonUI::DrawButton(const ButtonRect& rect, const std::string& label)
{
	// 名前空間の使用宣言
	using namespace Color;

	// ボタンの枠を描画
	DrawBox(rect.x, rect.y,
		rect.x + rect.w, rect.y + rect.h,
		Color::TextGray(), TRUE);

	// ボタンのラベルを描画
	DrawString(rect.x + 10, rect.y + 10, label.c_str(), Color::Black());
}

