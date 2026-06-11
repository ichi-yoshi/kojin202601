#include "GachaUI.h"
#include "ButtonUI.h"
#include "DxLib.h"

const ButtonUI::ButtonRect& GachaUI::GetGachaButtonRect() const
{
	return _gachaButton;
}

const ButtonUI::ButtonRect& GachaUI::GetSaveButtonRect() const
{
	return _saveButton;
}

const ButtonUI::ButtonRect& GachaUI::GetKeepButtonRect() const
{
	return _keepButton;
}

// ガチャ結果の下端Y座標を計算する
int GachaUI::GetGachaResultBottomY(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor) const
{
	int y = 440;
	int lineHeight = 20;

	int lineCount = 0;
	if (gachaArmor.HasResult() && !gachaArmor.GetResultLines().empty()) { lineCount += 1; }
	if (gachaBasic.HasResult() && !gachaBasic.GetResultLines().empty())
	{
		lineCount += static_cast<int>(gachaBasic.GetResultLines().size());
	}
	if (gacha.HasResult() && !gacha.GetResultLines().empty())
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
	if (!pending.hasPending) { return; }

	int x = 480;
	int y = GetGachaResultBottomY(gacha, gachaBasic, gachaArmor) + 12;

	_saveButton.x = x;
	_saveButton.y = y;
	_saveButton.w = 160;
	_saveButton.h = 36;

	_keepButton.x = x;
	_keepButton.y = y + _saveButton.h + 6;
	_keepButton.w = 160;
	_keepButton.h = 36;
}

void GachaUI::Draw(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor,
	const SaveEquipment& saveEquipment,
	const PendingGachaResult& pending) const
{
	DrawGachaResult(gacha, gachaBasic, gachaArmor);
	DrawPendingSelection(pending, saveEquipment);
	DrawSavedEquipment(saveEquipment);
	DrawGachaButton();
}

void GachaUI::DrawGachaResult(const Gacha& gacha,
	const GachaBasicStatus& gachaBasic,
	const GachaArmor& gachaArmor) const
{
	SetFontSize(18);
	int x = 480;
	int y = 440;

	int lineCount = 0;
	if (gachaArmor.HasResult() && !gachaArmor.GetResultLines().empty()) { lineCount += 1; }
	if (gachaBasic.HasResult() && !gachaBasic.GetResultLines().empty())
	{
		lineCount += static_cast<int>(gachaBasic.GetResultLines().size());
	}

	if (lineCount > 0)
	{
		int lineHeight = 120;
		int padding = 8;
		int boxW = 320;
		int boxH = lineCount * lineHeight + padding * 2;
		DrawBox(x - padding, y - padding, x - padding + boxW, y - padding + boxH, GetColor(235, 235, 235), TRUE);
	}

	if (gachaArmor.HasResult() && !gachaArmor.GetResultLines().empty())
	{
		const std::string& armorName = gachaArmor.GetResultLines().front();
		DrawString(x, y, armorName.c_str(), GetColor(0, 0, 0));
	}

	if (gachaBasic.HasResult() && !gachaBasic.GetResultLines().empty())
	{
		for (const auto& line : gachaBasic.GetResultLines())
		{
			DrawString(x + 120, y, line.c_str(), GetColor(255, 0, 0));
			y += 20;
		}
	}

	if (gacha.HasResult())
	{
		for (const auto& line : gacha.GetResultLines())
		{
			DrawString(x + 120, y, line.c_str(), GetColor(0, 0, 0));
			y += 20;
		}
	}
}

void GachaUI::DrawPendingSelection(const PendingGachaResult& pending,
	const SaveEquipment& saveEquipment) const
{
	if (!pending.hasPending) { return; }

	ButtonUI::DrawButton(_saveButton, "更新する");
	ButtonUI::DrawButton(_keepButton, "更新しない");
}

void GachaUI::DrawSavedEquipment(const SaveEquipment& saveEquipment) const
{
	int screenW = 0, screenH = 0;
	GetDrawScreenSize(&screenW, &screenH);

	int fontSize = 18;
	int lineHeight = fontSize + 2;
	int padding = 8;
	int boxW = 360;

	int totalLines = 0;
	for (int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);

		totalLines += 1;
		if (result.hasResult)
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

	if (totalLines <= 0) { return; }

	int boxH = totalLines * lineHeight + padding * 2;
	int x = screenW - boxW - 20;
	int y = 20;

	DrawBox(x, y, x + boxW, y + boxH, GetColor(235, 235, 235), TRUE);
	SetFontSize(fontSize);

	int textY = y + padding;
	for (int i = 0; i < static_cast<int>(SaveEquipment::EquipPart::_EOT_); ++i)
	{
		auto part = static_cast<SaveEquipment::EquipPart>(i);
		const auto& result = saveEquipment.GetResult(part);

		DrawString(x + padding, textY, saveEquipment.GetPartLabel(part), GetColor(0, 0, 0));
		textY += lineHeight;

		if (!result.hasResult)
		{
			DrawString(x + padding + 20, textY, "未取得", GetColor(128, 128, 128));
			textY += lineHeight;
			continue;
		}

		DrawString(x + padding + 20, textY, result.armorName.c_str(), GetColor(0, 0, 0));
		textY += lineHeight;

		for (const auto& line : result.basicStatusLines)
		{
			DrawString(x + padding + 20, textY, line.c_str(), GetColor(0, 0, 255));
			textY += lineHeight;
		}

		for (const auto& line : result.statusLines)
		{
			DrawString(x + padding + 20, textY, line.c_str(), GetColor(0, 0, 0));
			textY += lineHeight;
		}
	}
}

void GachaUI::DrawGachaButton() const
{
	ButtonUI::DrawButton(_gachaButton, "錬成");
}