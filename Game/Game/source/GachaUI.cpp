#include "GachaUI.h"
#include "ButtonUI.h"
#include "MagicNumberConfig.h"

// 名前空間の使用
using namespace UIConfig;

// ガチャ結果の下端Y座標を計算する
int GachaUI::GetGachaResultBottomY(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor) const
{
	// ガチャ結果の描画開始位置と行間隔を取得
	int y = Layout::Gacha::ResultBase.y;
	int RowHeight = Common::RowHeight;
	int RowCount = 0;

	// 装備ガチャの結果がある場合、行数を加算
	if(gachaArmor.HasResult() && !gachaArmor.GetResultRows().empty()) 
	{ 
		RowCount += 1; 
	}

	// メインステータスガチャの結果がある場合、行数を加算
	if(gachaBasic.HasResult() && !gachaBasic.GetResultRows().empty())
	{
		RowCount += static_cast<int>(gachaBasic.GetResultRows().size());
	}

	// サブステータスガチャの結果がある場合、行数を加算
	if(gacha.HasResult() && !gacha.GetResultRows().empty())
	{
		RowCount += static_cast<int>(gacha.GetResultRows().size());
	}

	return y + RowCount * RowHeight;
}

// ガチャ結果の保存・破棄ボタンの位置とサイズを更新する
void GachaUI::UpdatePendingButtons(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor,
	const PendingGachaResult& pending)
{
	if(!pending.hasPending) { return; }

	int x = Layout::Gacha::ResultBase.x;
	int y = GetGachaResultBottomY(gacha, gachaBasic, gachaArmor) + Common::ButtonTopMargin;

	_saveButton.x = x;
	_saveButton.y = y;
	_saveButton.w = Common::StandardButton.w;
	_saveButton.h = Common::StandardButton.h;

	_keepButton.x = x;
	_keepButton.y = y + _saveButton.h + Common::ButtonSpacing;
	_keepButton.w = Common::StandardButton.w;
	_keepButton.h = Common::StandardButton.h;
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
	SetFontSize(Font::Normal);
	int x = Layout::Gacha::ResultBase.x;
	int y = Layout::Gacha::ResultBase.y;
	int subTextXOffset = Layout::Gacha::SubTextXOffset;
	int RowCount = 0;

	// 装備ガチャの結果がある場合、行数を加算
	if(gachaArmor.HasResult() && !gachaArmor.GetResultRows().empty()) 
	{ 
		RowCount += 1; 
	}

	// メインステータスガチャの結果がある場合、行数を加算
	if(gachaBasic.HasResult() && !gachaBasic.GetResultRows().empty())
	{
		RowCount += static_cast<int>(gachaBasic.GetResultRows().size());
	}

	// サブステータスガチャの結果がある場合、行数を加算
	if(RowCount > 0)
	{
		int RowHeight = Layout::Gacha::SubTextXOffset;	// 行の高さを取得
		int padding = Common::DefaultPadding;		// ボックスの内側の余白を取得
		int boxW = Layout::Gacha::ResultBoxWidth;				// ボックスの幅を取得
		/**/int boxH = RowCount * RowHeight + padding * 2;	// ボックスの高さを計算
		DrawBox(x - padding, y - padding,
			x - padding + boxW,	y - padding + boxH, 
			Color::BoxBg(), TRUE);
	}

	// 装備ガチャの結果がある場合、装備名を描画
	if(gachaArmor.HasResult() && !gachaArmor.GetResultRows().empty())
	{
		const std::string& armorName = gachaArmor.GetResultRows().front();
		DrawString(x, y, armorName.c_str(), Color::Black());
	}

	// メインステータスガチャの結果がある場合、結果行を描画
	if(gachaBasic.HasResult() && !gachaBasic.GetResultRows().empty())
	{
		for(const auto& row : gachaBasic.GetResultRows())
		{
			DrawString(x + subTextXOffset, y, row.c_str(), Color::Red());
			y += Common::RowHeight;
		}
	}

	// サブステータスガチャの結果がある場合、結果行を描画
	if(gacha.HasResult())
	{
		for(const auto& row : gacha.GetResultRows())
		{
			DrawString(x + subTextXOffset, y, row.c_str(), Color::Black());
			y += Common::RowHeight;
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

	int fontSize = Font::Normal;
	int RowHeight = fontSize + Common::RowSpacingExtra;
	int padding = Common::DefaultPadding;
	int rowSpace = Common::RowSpacingExtra;
	int boxW = Layout::Gacha::SavedAreaWidth;
	int totalRows = 0;

	// 保存済み装備の行数を計算
	for(int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);

		totalRows += 1;
		if(result.hasResult)
		{
			totalRows += 1;
			totalRows += static_cast<int>(result.basicStatusRows.size());
			totalRows += static_cast<int>(result.statusRows.size());
		}
		else
		{
			totalRows += 1;
		}
	}

	if(totalRows <= 0) { return; }

	int boxH = totalRows * RowHeight + padding * rowSpace;	// ボックスの高さを計算
	int marginRight = Common::MarginRight;				// 右側の余白を取得
	int x = screenW - boxW - marginRight;				// ボックスのX座標を計算
	int y = marginRight;								// ボックスのY座標を計算
	int indentX = Common::DefaultIndent;				// インデントのX座標を取得

	DrawBox(x, y, x + boxW, y + boxH, Color::BoxBg(), TRUE);

	SetFontSize(fontSize);

	int textY = y + padding;

	// 各装備部位の結果を描画
	for(int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);

		DrawString(x + padding, textY, saveEquipment.GetPartLabel(part), Color::Black());
		textY += RowHeight;

		// 結果がない場合は「未取得」と表示
		if(!result.hasResult)
		{
			DrawString(x + padding + indentX, textY, "未取得", Color::TextGray());
			textY += RowHeight;
			continue;
		}

		DrawString(x + padding + indentX, textY, result.armorName.c_str(), Color::Black());
		textY += RowHeight;

		// 基礎ステータスの行を描画
		for(const auto& row : result.basicStatusRows)
		{
			DrawString(x + padding + indentX, textY, row.c_str(), Color::Blue());
			textY += RowHeight;
		}

		// 装備ステータスの行を描画
		for(const auto& row : result.statusRows)
		{
			DrawString(x + padding + indentX, textY, row.c_str(), Color::Black());
			textY += RowHeight;
		}
	}
}

void GachaUI::DrawGachaButton() const
{
	ButtonUI::DrawButton(_gachaButton, "錬成[R]1回-3000");
}