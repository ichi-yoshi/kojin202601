#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "SqliteUtill.h"
#include "SqliteTableCreator.h"
#include "SqliteSeeder.h"
#include "SqliteCharaStatus.h"
#include "Chara.h"
#include "Resource.h"
#include "SaveData.h"
#include <random>

ModeGame _modeGame;

bool ModeGame::Initialize()
{
	if (!base::Initialize()) { return false; }
	_cam.Initialize();

	// マップ
	_handleSkySphere = MV1LoadModel(mv1::SkySphere);
#if 1
	// ダンジョン
	_handleMap = MV1LoadModel(mv1::Dungeon);
	_frameMapCollision = MV1SearchFrame(_handleMap, "dungeon_collision");
#else
	// フィールド
	_handleMap = MV1LoadModel(mv1::Ground);
	_frameMapCollision = MV1SearchFrame(_handleMap, "ground_navmesh");
#endif
	// コリジョン情報の生成
	MV1SetupCollInfo(_handleMap, _frameMapCollision, 16, 16, 16);
	// コリジョンのフレームを描画しない設定
	MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);

	SetMouseDispFlag(TRUE);	// マウスポインタを表示する

	

	// SQLite初期化
	{
		sqlite3* dbh = nullptr;
		std::string error;
		if (!OpenSqliteConnection(&dbh, &error)) { return false; }
		if (!CreateSqliteTables(dbh)) { sqlite3_close(dbh); return false; }
		_saveData.LoadFromSqlite();
		if(_saveData.GetRows().empty())
		{
			if(!SeedSqliteData(dbh)) { sqlite3_close(dbh); return false; }
			printf("新規データベースのため、初期値を投入しました。\n");
		}
		else
		{
			printf("既存のセーブデータを発見したため、初期化をスキップしました。\n");
		}
		sqlite3_close(dbh);

		if (!_gacha.Initialize("", &error)) { return false; }
		if (!_gachaBasic.Initialize("", &error)) { return false; }
		if (!_gachaArmor.Initialize("", &error)) { return false; }

		CharaStatus base{};
		if (!LoadCharaBaseStatusSqlite(base, &error)) { return false; }
		_charaBase.SetBaseStatus(base);

		if(!_battleSystem.Initialize("", &error)) { return false; }
		if(!_afterStatus.InitializeSpeedTable("", &error)) { return false; }
	}
	
	// 保存済みデータをロード（なければ無視）
	_saveEquipment.LoadFromSqlite();
	_saveData.LoadFromSqlite();

	// 最終ステータス計算
	_afterStatus.UpdateFrom(_charaBase, _saveEquipment);

	deltaTime = 1.0f / 60.0f;

	return true;
}

bool ModeGame::Terminate()
{
	base::Terminate();
	return true;
}

bool ModeGame::Process() 
{
	base::Process();

	// マウス入力の更新
	_mouse.Update();

	// ホイール利用
	int wheel = _mouse.GetWheel();

	// ゲームフェーズごとの処理
	if(_gamePhase == GamePhase::Gacha)// ガチャフェーズ
	{
		// アイコンクリックの処理
		_gachaSystem.Process(gachaCtx);
		_battleUI.Update(_mouse);
		_statusUI.Update(_mouse);
		_saveDataUI.Update(_mouse);

		auto constRows = _saveData.GetRows();
		SaveData::AccountData account;

		if(constRows.empty())
		{
			// データベースが万が一空だった場合の初期化ガード
			account.uid = 1;
			account.level = 1;
			account.exp = 0;
			account.coin = 30000; // ガチャ突入時のデフォルトコイン（任意に調整してください）
			account.enemylevel = 0;
		}
		else
		{
			// 既存のデータをベースにする
			account = constRows[0];
		}

		if (_statusUI.IsCharaClicked())
		{
			_showCharaStatus = !_showCharaStatus;
		}

		if(_battleUI.IsBattleClicked())
		{
			// バトルフェーズへ遷移
			_saveData.LoadFromSqlite();
			_battleSystem.Reset(_saveData, _afterStatus);
			_gamePhase = GamePhase::Battle;
		}

		if(_saveDataUI.IsSaveDataClicked()) 
		{
			_showSaveData = !_showSaveData;
		}
	}
	else if(_gamePhase == GamePhase::Battle)// バトルフェーズ
	{
		_battleSystem.Process(_mouse, _afterStatus, _saveData, deltaTime);

		// 戦闘終了判定
		if(_battleSystem.IsBattleEnd())
		{
			_gamePhase = GamePhase::Gacha;
		}

		// 【デバッグ用】Bキーを押したらガチャ画面に戻る
		if (CheckHitKey(KEY_INPUT_B) == 1)
		{
			_gamePhase = GamePhase::Gacha;
		}
	}

	// デバッグ機能
	// Deleteキーで装備のセーブデータを削除
	if (CheckHitKey(KEY_INPUT_DELETE)==1) 
	{
		_saveEquipment.ClearResults();							// 装備セーブデータの削除
		_afterStatus.UpdateFrom(_charaBase, _saveEquipment);	// 最終ステータスの更新

		_saveCharaStatus.SetFromAfterStatus(_afterStatus);
		_saveCharaStatus.SaveToSqlite();
	}
	return true;
}

bool ModeGame::Render()
{
	base::Render();

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// カメラ設定更新
	_cam.ApplyCamera();

	// マップモデルを描画する
	{
		MV1DrawModel(_handleMap);
		MV1DrawModel(_handleSkySphere);
	}

	//UI描画
	if (_gamePhase == GamePhase::Gacha)
	{
		_gachaUI.Draw(_gacha, _gachaBasic, _gachaArmor, _saveEquipment, _pendingResult);
		_statusUI.Draw(_afterStatus, _showCharaStatus);
		_battleUI.Draw();
		_saveDataUI.Draw(_saveData, _showSaveData);
	}
	else if (_gamePhase == GamePhase::Battle)
	{
		_battleSystem.Render(_afterStatus);
	}

	return true;
}

