#pragma once
#include "Gacha.h"
#include "GachaBasicStatus.h"
#include "GachaArmor.h"
#include "SaveEquipment.h"
#include "SaveCharaStatus.h"
#include "GachaUI.h"
#include "MouseInput.h"
#include "CharaAfterStatus.h"
#include "CharaBase.h"
#include "SaveData.h"

// 引数リスト
struct GachaContext
{
	MouseInput& mouse;
	GachaUI& gachaUI;
	Gacha& gacha;
	GachaBasicStatus& gachaBasic;
	GachaArmor& gachaArmor;
	SaveEquipment& saveEquipment;
	SaveCharaStatus& saveCharaStatus;
	PendingGachaResult& pendingResult;
	CharaAfterStatus& afterStatus;
	const CharaBase& charaBase;
	SaveData& saveData;
};

class GachaSystem
{
public:
	void Process(GachaContext& ctx);
private:
	//SaveData saveData;
	int gachaCount;
	int gachaCost;
	// ガチャの抽選処理
	void ProcessRoll(GachaContext& ctx);

	// ガチャ結果の保存・破棄処理
	void ProcessPendingSelection(GachaContext& ctx);
};