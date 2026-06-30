#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "SqliteTableCreator.h"
#include "SqliteSeeder.h"
#include "SqliteCharaStatus.h"
#include "Chara.h"
#include "Resource.h"
#include "SqliteConfig.h"

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

bool ModeGame::ChangeDatabase(const std::string& newDbPath)
{
	printf("データベースを %s に切り替えます...\n", newDbPath.c_str());

	// 1. パスの変更
	SqliteConfig::SetSqliteDbPath(newDbPath);

	// 2. 新しいデータベースに対するテーブル作成およびシードデータの確認
	{
		sqlite3 * dbh = nullptr;
		std::string error;

		if(!OpenSqliteConnection(&dbh, &error)) { return false; }
		if(!CreateSqliteTables(dbh)) { sqlite3_close(dbh); return false; }

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

		// 3. マスタ・設定データの再ロード
		if(!_gacha.Initialize("", &error)) { return false; }
		if(!_gachaBasic.Initialize("", &error)) { return false; }
		if(!_gachaArmor.Initialize("", &error)) { return false; }

		CharaStatus base{};
		if(!LoadCharaBaseStatusSqlite(base, &error)) { return false; }
		_charaBase.SetBaseStatus(base);
		if(!_battleSystem.Initialize("", &error)) { return false; }
		if(!_afterStatus.InitializeSpeedTable("", &error)) { return false; }
	}
	
	// 4. セーブデータの再ロード
	_saveEquipment.LoadFromSqlite();
	_saveData.LoadFromSqlite();
	
		 	// 5. ステータス再計算
	_afterStatus.UpdateFrom(_charaBase, _saveEquipment);
	
	printf("データベースの切り替えが完了しました。\n");
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
		_battleButtonUI.Update(_mouse);
		_statusUI.Update(_mouse);
		_saveDataUI.Update(_mouse);
		_dbSelectorButtonUI.Update(_mouse);

		if (_statusUI.IsCharaClicked())
		{
			_showCharaStatus = !_showCharaStatus;
		}

		if(_battleButtonUI.IsBattleClicked())
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

		if(_dbSelectorButtonUI.IsDbSelectClicked())
		{
			_dbSelector.StartInput();
			_gamePhase = GamePhase::DbSelect;
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
	else if(_gamePhase == GamePhase::DbSelect) // データベース選択フェーズ
	{
		_dbSelector.Update();
		if(_dbSelector.IsFinished())
		{
			std::string prefix = _dbSelector.GetDatabasePrefix();
			if(!prefix.empty())
			{
				ChangeDatabase(prefix + ".sqlite3");
			}
			_gamePhase = GamePhase::Gacha;
		}
		else if(_dbSelector.IsCancelled())
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
		_battleButtonUI.Draw();
		_dbSelectorButtonUI.Draw();
		_saveDataUI.Draw(_saveData, _showSaveData);
		std::string dbMsg = "Active DB: " + std::string(SqliteConfig::GetSqliteDbPath());
		DrawString(10,10, dbMsg.c_str(), GetColor(100,100,100));
	}
	else if (_gamePhase == GamePhase::Battle)
	{
		_battleSystem.Render(_afterStatus);
	}
	else if(_gamePhase == GamePhase::DbSelect)
	{
		_dbSelector.Draw();
	}

	return true;
}

