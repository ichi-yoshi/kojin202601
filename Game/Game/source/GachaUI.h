#pragma once
#include "Gacha.h"
#include "GachaBasicStatus.h"
#include "GachaArmor.h"
#include "SaveEquipment.h"
#include "ButtonUI.h"
#include "container.h"

// ガチャ結果の保存待ち情報を表す構造体
struct PendingGachaResult
{
	bool hasPending = false;										// ガチャ結果があるかどうか
	SaveEquipment::EquipPart part = SaveEquipment::EquipPart::Head;	// 装備部位
	std::string armorName;											// 装備名
	std::vector<std::string> basicStatusRows;						// 基礎ステータスの行
	std::vector<std::string> statusRows;							// 装備ステータスの行
	std::vector<bool> statusIsMaxVal;								// 装備ステータスの値が最大値に近いかどうか
};

class GachaUI
{
public:
	// ボタンの矩形を取得する
	const ButtonUI::ButtonRect& GetGachaButtonRect() const { return _gachaButton; }
	const ButtonUI::ButtonRect& GetSaveButtonRect() const { return _saveButton; }
	const ButtonUI::ButtonRect& GetKeepButtonRect() const { return _keepButton; }

	// ガチャ結果の保存・破棄ボタンの位置とサイズを更新する
	void UpdatePendingButtons(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor,
		const PendingGachaResult& pending);

	// ガチャ結果の描画
	void Draw(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor,
		const SaveEquipment& saveEquipment,
		const PendingGachaResult& pending) const;

private:
	// ガチャ結果の描画
	void DrawGachaResult(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor) const;

	// 保存待ちのガチャ結果の描画
	void DrawPendingSelection(const PendingGachaResult& pending,
		const SaveEquipment& saveEquipment) const;

	// 保存済み装備の描画
	void DrawSavedEquipment(const SaveEquipment& saveEquipment) const;

	// ガチャボタンの描画
	void DrawGachaButton() const;

	// ガチャ結果の下端のY座標を取得する
	int GetGachaResultBottomY(const Gacha& gacha,
		const GachaBasicStatus& gachaBasic,
		const GachaArmor& gachaArmor) const;

	// ボタンの矩形
	ButtonUI::ButtonRect _gachaButton{ 20, 40, 160, 40 };
	ButtonUI::ButtonRect _saveButton{ 20, 100, 160, 40 };
	ButtonUI::ButtonRect _keepButton{ 20, 150, 160, 40 };
};