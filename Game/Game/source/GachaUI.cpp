#include "GachaUI.h"
#include "ButtonUI.h"
#include "MagicNumberConfig.h"

// 名前空間の使用
using namespace FontSize;
using namespace Color;
using namespace LayoutGacha;

// ガチャ結果の下端Y座標を計算する
int GachaUI::GetGachaResultBottomY(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor) const
{
	// ガチャ結果の描画開始位置と行間隔を取得
	int y = LayoutGacha::ResultBaseY;
	int lineHeight = LayoutGacha::ResultLineHeight;
	int lineCount = 0;

	// 装備ガチャの結果がある場合、行数を加算
	if(gachaArmor.HasResult() && !gachaArmor.GetResultLines().empty()) 
	{ 
		lineCount += 1; 
	}

	// メインステータスガチャの結果がある場合、行数を加算
	if(gachaBasic.HasResult() && !gachaBasic.GetResultLines().empty())
	{
		lineCount += static_cast<int>(gachaBasic.GetResultLines().size());
	}

	// サブステータスガチャの結果がある場合、行数を加算
	if(gacha.HasResult() && !gacha.GetResultLines().empty())
	{
		lineCount += static_cast<int>(gacha.GetResultLines().size());
	}

	return y + lineCount * lineHeight;
}

// ガチャ結果の保存・破棄ボタンの位置とサイズを更新する
void GachaUI::UpdatePendingButtons(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor,
	const PendingGachaResult& pending)
{
	if(!pending.hasPending) { return; }

	int x = LayoutGacha::ResultBaseX;
	int y = GetGachaResultBottomY(gacha, gachaBasic, gachaArmor) + LayoutGacha::ButtonTopMargin;

	_saveButton.x = x;
	_saveButton.y = y;
	_saveButton.w = LayoutGacha::ButtonWidth;
	_saveButton.h = LayoutGacha::ButtonHeight;

	_keepButton.x = x;
	_keepButton.y = y + _saveButton.h + LayoutGacha::ButtonSpacing;
	_keepButton.w = LayoutGacha::ButtonWidth;
	_keepButton.h = LayoutGacha::ButtonHeight;
}

void GachaUI::Draw(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor,
	const SaveEquipment& saveEquipment,
	const PendingGachaResult& pending) const
{
	DrawGachaResult(gacha, gachaBasic, gachaArmor);	// ガチャ結果の描画
	DrawPendingSelection(pending, saveEquipment);	// 保存・破棄ボタンの描画
	DrawSavedEquipment(saveEquipment);				// 保存済み装備の描画
	DrawGachaButton();								// ガチャボタンの描画
}

void GachaUI::DrawGachaResult(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor) const
{
	SetFontSize(FontSize::History);
	int x = LayoutGacha::ResultBaseX;
	int y = LayoutGacha::ResultBaseY;
	int subTextXOffset = LayoutGacha::ResultSubTextXOffset;
	int lineCount = 0;

	// 装備ガチャの結果がある場合、行数を加算
	if(gachaArmor.HasResult() && !gachaArmor.GetResultLines().empty()) 
	{ 
		lineCount += 1; 
	}

	// メインステータスガチャの結果がある場合、行数を加算
	if(gachaBasic.HasResult() && !gachaBasic.GetResultLines().empty())
	{
		lineCount += static_cast<int>(gachaBasic.GetResultLines().size());
	}

	// サブステータスガチャの結果がある場合、行数を加算
	if(lineCount > 0)
	{
		int lineHeight = LayoutGacha::ResultSubTextXOffset;	// 行の高さを取得
		int padding = LayoutGacha::ResultBoxPadding;		// ボックスの内側の余白を取得
		int boxW = LayoutGacha::ResultBoxWidth;				// ボックスの幅を取得
		int boxH = lineCount * lineHeight + padding * 2;	// ボックスの高さを計算
		DrawBox(x - padding, y - padding, x - padding + boxW, y - padding + boxH, Color::BoxBg(), TRUE);
	}

	// 装備ガチャの結果がある場合、装備名を描画
	if(gachaArmor.HasResult() && !gachaArmor.GetResultLines().empty())
	{
		const std::string& armorName = gachaArmor.GetResultLines().front();
		DrawString(x, y, armorName.c_str(), Color::Black());
	}

	// メインステータスガチャの結果がある場合、結果行を描画
	if(gachaBasic.HasResult() && !gachaBasic.GetResultLines().empty())
	{
		for(const auto& line : gachaBasic.GetResultLines())
		{
			DrawString(x + subTextXOffset, y, line.c_str(), Color::Red());
			y += LayoutGacha::ResultLineHeight;
		}
	}

	// サブステータスガチャの結果がある場合、結果行を描画
	if(gacha.HasResult())
	{
		for(const auto& line : gacha.GetResultLines())
		{
			DrawString(x + subTextXOffset, y, line.c_str(), Color::Black());
			y += LayoutGacha::ResultLineHeight;
		}
	}
}

void GachaUI::DrawPendingSelection(const PendingGachaResult& pending,
	const SaveEquipment& saveEquipment) const
{
	if(!pending.hasPending) { return; }

	ButtonUI::DrawButton(_saveButton, "更新する");
	ButtonUI::DrawButton(_keepButton, "更新しない");
}

void GachaUI::DrawSavedEquipment(const SaveEquipment& saveEquipment) const
{
	// 画面サイズを取得
	int screenW = 0, screenH = 0;
	GetDrawScreenSize(&screenW, &screenH);

	int fontSize = FontSize::History;
	int lineHeight = fontSize + 2;
	int padding = LayoutGacha::Padding;
	int boxW = LayoutGacha::Width;
	int totalLines = 0;

	// 保存済み装備の行数を計算
	for(int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);

		totalLines += 1;
		if(result.hasResult)
		{
			totalLines += 1;
			totalLines += static_cast<int>(result.basicStatusLines.size());
			totalLines += static_cast<int>(result.statusLines.size());
		}
		else
		{
			totalLines += 1;
		}
	}

	if(totalLines <= 0) { return; }

	int boxH = totalLines * lineHeight + padding * 2;	// ボックスの高さを計算
	int marginRight = LayoutGacha::MarginRight;			// 右側の余白を取得
	int x = screenW - boxW - marginRight;				// ボックスのX座標を計算
	int y = marginRight;								// ボックスのY座標を計算
	int indentX = LayoutGacha::IndentX;					// インデントのX座標を取得

	DrawBox(x, y, x + boxW, y + boxH, Color::BoxBg(), TRUE);
	SetFontSize(fontSize);

	int textY = y + padding;
	for(int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);

		DrawString(x + padding, textY, saveEquipment.GetPartLabel(part), Color::Black());
		textY += lineHeight;

		if(!result.hasResult)
		{
			DrawString(x + padding + indentX, textY, "未取得", Color::TextGray());
			textY += lineHeight;
			continue;
		}

		DrawString(x + padding + indentX, textY, result.armorName.c_str(), Color::Black());
		textY += lineHeight;

		for(const auto& line : result.basicStatusLines)
		{
			DrawString(x + padding + indentX, textY, line.c_str(), Color::Blue());
			textY += lineHeight;
		}

		for(const auto& line : result.statusLines)
		{
			DrawString(x + padding + indentX, textY, line.c_str(), Color::Black());
			textY += lineHeight;
		}
	}
}

void GachaUI::DrawGachaButton() const
{
	ButtonUI::DrawButton(_gachaButton, "錬成[R]1回-3000");
}