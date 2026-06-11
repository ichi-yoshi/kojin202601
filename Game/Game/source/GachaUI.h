#pragma once
#include "Gacha.h"
#include "GachaBasicStatus.h"
#include "GachaArmor.h"
#include "SaveEquipment.h"
#include "ButtonUI.h"
#include <string>
#include <vector>

struct PendingGachaResult
{
	bool hasPending = false;
	SaveEquipment::EquipPart part = SaveEquipment::EquipPart::Head;
	std::string armorName;
	std::vector<std::string> basicStatusLines;
	std::vector<std::string> statusLines;
};

class GachaUI
{
public:
	// ボタンの矩形を取得する
	const ButtonUI::ButtonRect& GetGachaButtonRect() const;
	const ButtonUI::ButtonRect& GetSaveButtonRect() const;
	const ButtonUI::ButtonRect& GetKeepButtonRect() const;

	void UpdatePendingButtons(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor,
		const PendingGachaResult& pending);

	void Draw(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor,
		const SaveEquipment& saveEquipment,
		const PendingGachaResult& pending) const;

private:
	void DrawGachaResult(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor) const;

	void DrawPendingSelection(const PendingGachaResult& pending,
		const SaveEquipment& saveEquipment) const;

	void DrawSavedEquipment(const SaveEquipment& saveEquipment) const;

	void DrawGachaButton() const;

	int GetGachaResultBottomY(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor) const;

	// ボタンの矩形
	ButtonUI::ButtonRect _gachaButton{ 20, 40, 160, 40 };
	ButtonUI::ButtonRect _saveButton{ 20, 100, 160, 40 };
	ButtonUI::ButtonRect _keepButton{ 20, 150, 160, 40 };
};