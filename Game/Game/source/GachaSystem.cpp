#include "GachaSystem.h"
#include "MagicNumberConfig.h"

// 名前空間の使用宣言
using namespace UIConfig;

// ガチャシステムの処理
void GachaSystem::Process(GachaContext& ctx)
{
	ProcessRoll(ctx);				// ガチャの抽選処理
	ProcessPendingSelection(ctx);	// ガチャ結果の保存・破棄処理
}

// ガチャの抽選処理
void GachaSystem::ProcessRoll(GachaContext& ctx)
{
	// ガチャボタンのクリック判定
	const auto& btn = ctx.gachaUI.GetGachaButtonRect();
	const bool gachaClicked = ctx.mouse.IsLeftTrig() && ctx.mouse.IsInRect(btn.x, btn.y, btn.w, btn.h);

	// データベースからアカウントデータをロードする
	ctx.saveData.LoadFromSqlite();

	// 現在のアカウントデータを取得する
	auto constRows = ctx.saveData.GetRows();
	SaveData::AccountData account{};
	if(!constRows.empty())
	{
		// 既存のデータをベースにする
		account = constRows[0];
	}

	// ガチャ結果が未表示でガチャボタンがクリックされた場合は抽選を行う
	if(!ctx.pendingResult.hasPending && (CheckHitKey(KEY_INPUT_R) || gachaClicked) && account.coin >= Param::coinCost)
	{
		// ガチャの抽選を行う
		ctx.gacha.Roll();		// サブステータスガチャの抽選
		ctx.gachaBasic.Roll();	// メインステータスガチャの抽選
		ctx.gachaArmor.Roll();	// 装備ガチャの抽選

		// ガチャ結果の装備名を取得して、保存待ち状態にする
		if(ctx.gachaArmor.HasResult() && !ctx.gachaArmor.GetResultRows().empty())
		{
			// ガチャ結果の装備名を取得
			const std::string& armorName = ctx.gachaArmor.GetResultRows().front();
			const std::vector<std::string> Empty;
			const auto& basicRows = ctx.gachaBasic.HasResult() ? ctx.gachaBasic.GetResultRows() : Empty;
			const auto& statusRows = ctx.gacha.HasResult() ? ctx.gacha.GetResultRows() : Empty;

			// ガチャ結果を保存待ち状態にする
			ctx.pendingResult.hasPending = true;								// 保存待ち状態にする
			ctx.pendingResult.part = SaveEquipment::GetPartFromName(armorName);	// 装備部位を取得して保存待ち状態にする
			ctx.pendingResult.armorName = armorName;							// 装備名を保存待ち状態にする
			ctx.pendingResult.basicStatusRows = basicRows;						// 基礎ステータスの行を保存待ち状態にする
			ctx.pendingResult.statusRows = statusRows;							// 装備ステータスの行を保存待ち状態にする
			ctx.pendingResult.statusIsMaxVal = ctx.gacha.GetIsMaxVal();			// 装備ステータスの最大値フラグを保存待ち状態にする
		}

		account.coin -= Param::coinCost;	// ガチャコストを引く
		account.gachaCount += 1;			// ガチャ回数を増やす

		std::vector<SaveData::AccountData> updatedVector;	
		updatedVector.push_back(account); // 編集し終わったデータを格納
		std::string errStr;
		bool success = false;	// データベースに保存するためのフラグ

		// アカウントデータを更新して保存する
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

	// 保存ボタンと破棄ボタンの矩形を取得
	const auto& saveBtn = ctx.gachaUI.GetSaveButtonRect();
	const auto& keepBtn = ctx.gachaUI.GetKeepButtonRect();

	// 保存ボタンと破棄ボタンのクリック判定
	const bool saveClicked = ctx.mouse.IsLeftTrig() && ctx.mouse.IsInRect(saveBtn.x, saveBtn.y, saveBtn.w, saveBtn.h);
	const bool keepClicked = ctx.mouse.IsLeftTrig() && ctx.mouse.IsInRect(keepBtn.x, keepBtn.y, keepBtn.w, keepBtn.h);

	// 保存ボタンがクリックされた場合は装備を保存して最終ステータスを更新
	// 破棄ボタンがクリックされた場合はガチャ結果をクリア
	// どちらかがクリックされた時にガチャ結果の表示をクリア
	if(saveClicked)
	{
		// 装備を保存して最終ステータスを更新
		ctx.saveEquipment.SaveResult(ctx.pendingResult.armorName,
			ctx.pendingResult.basicStatusRows,
			ctx.pendingResult.statusRows,
			ctx.pendingResult.statusIsMaxVal);
		ctx.saveEquipment.SaveToSqlite();

		// 最終ステータスを更新
		ctx.afterStatus.UpdateFrom(ctx.charaBase, ctx.saveEquipment, ctx.saveData);

		// 最終ステータスを保存
		ctx.saveCharaStatus.SetFromAfterStatus(ctx.afterStatus);
		ctx.saveCharaStatus.SaveToSqlite();

		// ガチャ結果をクリア
		ctx.gacha.ClearResult();
		ctx.gachaBasic.ClearResult();
		ctx.gachaArmor.ClearResult();
		ctx.pendingResult = PendingGachaResult{};	// クリア後は保存待ち状態を解除
	}
	else if(keepClicked)
	{
		// ガチャ結果をクリア
		ctx.gacha.ClearResult();
		ctx.gachaBasic.ClearResult();
		ctx.gachaArmor.ClearResult();
		ctx.pendingResult = PendingGachaResult{};	// クリア後は保存待ち状態を解除
	}
}
