#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "SqliteTableCreator.h"
#include "SqliteSeeder.h"
#include "SqliteCharaStatus.h"
#include "Chara.h"
#include "Resource.h"
#include "SqliteConfig.h"
#include "MagicNumberConfig.h"

// 名前空間の使用の宣言
using namespace UIConfig;

bool ModeGame::Initialize()
{
	if (!base::Initialize()) { return false; }
	_cam.Initialize();

	// マップ
	_handleSkySphere = MV1LoadModel(mv1::SkySphere);
#if 1
	// ダンジョン
	_handleMap = MV1LoadModel(mv1::Dungeon);
	_frameMapCollision = MV1SearchFrame(_handleMap, Collision::DungeonCollision);
#else
	// フィールド
	_handleMap = MV1LoadModel(mv1::Ground);
	_frameMapCollision = MV1SearchFrame(_handleMap, Collision::GroundNavMesh);
#endif
	// コリジョン情報の生成
	MV1SetupCollInfo(_handleMap, _frameMapCollision, 16, 16, 16);
	// コリジョンのフレームを描画しない設定
	MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);

	// マウスポインタを表示する
	SetMouseDispFlag(TRUE);	

	// データベース初期化
	SqliteInitialize();

	deltaTime = Param::time;

	return true;
}

bool ModeGame::Terminate()
{
	base::Terminate();
	return true;
}

// データベースの初期化
bool ModeGame::SqliteInitialize()
{
	// SQLiteの初期化
	sqlite3* dbh = nullptr;
	std::string error;
	if(!OpenSqliteConnection(&dbh, &error)) { return false; }			// データベース接続
	if(!CreateSqliteTables(dbh)) { sqlite3_close(dbh); return false; }	// テーブル作成

	// セーブデータのロード
	_saveData.LoadFromSqlite();
	if(_saveData.GetRows().empty())
	{
		if(!SeedSqliteData(dbh)) { sqlite3_close(dbh); return false; }	// シードデータ投入
	}
	sqlite3_close(dbh);

	// 保存済みデータをロード（なければ無視）
	_saveEquipment.LoadFromSqlite();	// 装備データのロード
	_saveData.LoadFromSqlite();			// アカウントデータのロード

	//ガチャデータの初期化
	if(!_gacha.Initialize("", &error)) { return false; }
	if(!_gachaBasic.Initialize("", &error)) { return false; }
	if(!_gachaArmor.Initialize("", &error)) { return false; }

	// キャラクターステータスの初期化
	CharaStatus base{};
	if(!LoadCharaBaseStatusSqlite(base, &error)) { return false; }
	_charaBase.SetBaseStatus(base);

	// バトルシステムの初期化
	if(!_battleSystem.Initialize("", &error)) { return false; }
	if(!_afterStatus.InitializeSpeedTable("", &error)) { return false; }

	
	// 最終ステータス計算
	_afterStatus.UpdateFrom(_charaBase, _saveEquipment, _saveData);

	return true;
}

// データベースの変更
bool ModeGame::ChangeDatabase(const std::string& newDbPath)
{
	// パスの変更
	SqliteConfig::SetSqliteDbPath(newDbPath);

	// SQLite初期化
	SqliteInitialize();

	return true;
}

bool ModeGame::Process() 
{
	base::Process();

	// マウス入力の更新
	_mouse.Update();

	// ゲームフェーズごとの処理
	if(_gamePhase == GamePhase::Gacha)// ガチャフェーズ
	{
		// ボタンのクリック処理
		ClickButton();	
	}
	else if(_gamePhase == GamePhase::Battle)// バトルフェーズ
	{
		// バトルシステムの処理
		_battleSystem.Process(_mouse, _afterStatus, _saveData, _charaBase, _saveEquipment, deltaTime);

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
	else if(_gamePhase == GamePhase::DBSelect) // データベース選択フェーズ
	{
		// データベース選択の更新
		_dbSelector.Update();

		// データベース選択が完了した場合、またはキャンセルされた場合の処理
		if(_dbSelector.IsFinished())
		{
			std::string prefix = _dbSelector.GetDatabasePrefix();
			if(!prefix.empty())
			{
				ChangeDatabase(prefix + ".sqlite3");	// データベースの変更
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
		_saveEquipment.ClearResults();									// 装備セーブデータの削除
		_afterStatus.UpdateFrom(_charaBase, _saveEquipment, _saveData);	// 最終ステータスの更新
		_saveCharaStatus.SetFromAfterStatus(_afterStatus);				// キャラクターステータスの更新
		_saveCharaStatus.SaveToSqlite();								// キャラクターステータスの保存
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
	if(_gamePhase == GamePhase::Gacha)	// ガチャフェーズ
	{
		_gachaUI.Draw(_gacha, _gachaBasic, _gachaArmor, _saveEquipment, _pendingResult);
		_statusUI.Draw(_afterStatus, _showCharaStatus, _mouse);
		_battleButtonUI.Draw();
		_dbSelectorButtonUI.Draw();
		_saveDataUI.Draw(_saveData, _showSaveData);

		// 現在のデータベース名を表示
		std::string dbMsg = "現在のDB: " + std::string(SqliteConfig::GetSqliteDbPath());
		DrawString(Layout::DataBase::CurrentDB.x, Layout::DataBase::CurrentDB.y, dbMsg.c_str(), Color::DarkGray());
	}
	else if(_gamePhase == GamePhase::Battle)	// バトルフェーズ
	{
		_battleSystem.Render(_afterStatus);
	}
	else if(_gamePhase == GamePhase::DBSelect)	// データベース選択フェーズ
	{
		_dbSelector.Draw();
	}
	return true;
}

void ModeGame::ClickButton() 
{
	// アイコンクリックの処理
	{
		_gachaSystem.Update(gachaCtx);
		_battleButtonUI.Update(_mouse);
		_statusUI.Update(_mouse);
		_saveDataUI.Update(_mouse);
		_dbSelectorButtonUI.Update(_mouse);
	}
	
	//クリック判定
	{
		if(_statusUI.IsCharaClicked())
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
			// データベース選択フェーズへ遷移
			_dbSelector.StartInput();
			_gamePhase = GamePhase::DBSelect;
		}
	}
}