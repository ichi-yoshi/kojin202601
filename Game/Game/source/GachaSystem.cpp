#include "GachaSystem.h"
#include "DxLib.h"

// ガチャシステムの処理
void GachaSystem::Process(GachaContext& ctx)
{
	ProcessRoll(ctx);
	ProcessPendingSelection(ctx);
}

// ガチャの抽選処理
void GachaSystem::ProcessRoll(GachaContext& ctx)
{
	// ガチャボタンのクリック判定
	const auto& btn = ctx.gachaUI.GetGachaButtonRect();
	const bool gachaClicked = ctx.mouse.IsLeftTrig() && ctx.mouse.IsInRect(btn.x, btn.y, btn.w, btn.h);

	ctx.saveData.LoadFromSqlite();

	auto constRows = ctx.saveData.GetRows();
	SaveData::AccountData account{};
	if(constRows.empty())
	{
	
	}
	else
	{
		// 既存のデータをベースにする
		account = constRows[0];
	}

	// ガチャ結果が未表示でガチャボタンがクリックされた場合は抽選を行う
	if(!ctx.pendingResult.hasPending && (CheckHitKey(KEY_INPUT_R) || gachaClicked) && account.coin >= 3000)
	{
		ctx.gacha.Roll();
		ctx.gachaBasic.Roll();
		ctx.gachaArmor.Roll();

		// ガチャ結果の装備名を取得して、保存待ち状態にする
		if(ctx.gachaArmor.HasResult() && !ctx.gachaArmor.GetResultLines().empty())
		{
			const std::string& armorName = ctx.gachaArmor.GetResultLines().front();
			const std::vector<std::string> Empty;
			const auto& basicLines = ctx.gachaBasic.HasResult() ? ctx.gachaBasic.GetResultLines() : Empty;
			const auto& statusLines = ctx.gacha.HasResult() ? ctx.gacha.GetResultLines() : Empty;

			ctx.pendingResult.hasPending = true;
			ctx.pendingResult.part = SaveEquipment::GetPartFromName(armorName);
			ctx.pendingResult.armorName = armorName;
			ctx.pendingResult.basicStatusLines = basicLines;
			ctx.pendingResult.statusLines = statusLines;
		}

		account.coin -= 3000; // ガチャコストを引く
		account.gachaCount += 1;
		std::vector<SaveData::AccountData> updatedVector;
		updatedVector.push_back(account); // 編集し終わったデータを格納
		std::string errStr;
		bool success = false;

		success = ctx.saveData.UpdateAccountAndSave(account, &errStr);
	}
}

// ガチャ結果の保存・破棄処理
void GachaSystem::ProcessPendingSelection(GachaContext& ctx)
{
	// ガチャ結果の保存・破棄ボタンのクリック判定
	if(!ctx.pendingResult.hasPending) { return; }

	// ガチャ結果の保存・破棄ボタンは、保存待ち状態でのみ表示されるため、ここでボタンの有効/無効を更新する
	ctx.gachaUI.UpdatePendingButtons(ctx.gacha, ctx.gachaBasic, ctx.gachaArmor, ctx.pendingResult);

	const auto& saveBtn = ctx.gachaUI.GetSaveButtonRect();
	const auto& keepBtn = ctx.gachaUI.GetKeepButtonRect();

	const bool saveClicked = ctx.mouse.IsLeftTrig() && ctx.mouse.IsInRect(saveBtn.x, saveBtn.y, saveBtn.w, saveBtn.h);
	const bool keepClicked = ctx.mouse.IsLeftTrig() && ctx.mouse.IsInRect(keepBtn.x, keepBtn.y, keepBtn.w, keepBtn.h);

	// 保存ボタンがクリックされた場合は装備を保存して最終ステータスを更新
	// 破棄ボタンがクリックされた場合はガチャ結果をクリア
	// どちらもクリックされた場合はガチャ結果の表示をクリア
	if(saveClicked)
	{
		ctx.saveEquipment.SaveResult(ctx.pendingResult.armorName,
			ctx.pendingResult.basicStatusLines,
			ctx.pendingResult.statusLines);
		ctx.saveEquipment.SaveToSqlite();

		ctx.afterStatus.UpdateFrom(ctx.charaBase, ctx.saveEquipment);

		ctx.saveCharaStatus.SetFromAfterStatus(ctx.afterStatus);
		ctx.saveCharaStatus.SaveToSqlite();

		ctx.gacha.ClearResult();
		ctx.gachaBasic.ClearResult();
		ctx.gachaArmor.ClearResult();
		ctx.pendingResult = PendingGachaResult{};
	}
	else if(keepClicked)
	{
		ctx.gacha.ClearResult();
		ctx.gachaBasic.ClearResult();
		ctx.gachaArmor.ClearResult();
		ctx.pendingResult = PendingGachaResult{};
	}
}
