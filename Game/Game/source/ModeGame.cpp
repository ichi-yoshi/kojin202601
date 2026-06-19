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

static int prevMouseX = -1, prevMouseY = -1;
static float camYaw = 0.0f, camPitch = 0.0f;

ModeGame _modeGame;

bool ModeGame::Initialize()
{
	if (!base::Initialize()) { return false; }
	_cam.Initialize();

	// 位置,向きの初期化
	_vPos = VGet(0, 0, 0);
	_vDir = VGet(0, 0, -1);		// キャラモデルはデフォルトで-Z方向を向いている

	// マップ
	_handleSkySphere = MV1LoadModel(mv1::SkySphere);
#if 0
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

	// その他初期化
	_bViewCollision = TRUE;
	SetMouseDispFlag(TRUE);	// マウスポインタを表示する

	// 保存済み装備をロード（なければ無視）
	_saveEquipment.LoadFromSqlite();

	_saveData.LoadFromSqlite();

	// SQLite初期化
	{
		sqlite3* dbh = nullptr;
		std::string error;
		if (!OpenSqliteConnection(&dbh, &error)) { return false; }
		if (!CreateSqliteTables(dbh)) { sqlite3_close(dbh); return false; }
		if (!SeedSqliteData(dbh)) { sqlite3_close(dbh); return false; }
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

	// 最終ステータス計算
	_afterStatus.UpdateFrom(_charaBase, _saveEquipment);

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
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();

	// マウス入力の更新
	_mouse.Update();

	// ホイール利用
	int wheel = _mouse.GetWheel();

	if (_gamePhase == GamePhase::Gacha)
	{
		// ガチャシステムの処理
		_gachaSystem.Process(gachaCtx);

		// キャラステータス表示の切り替え
		const auto& charaBtn = _statusUI.GetCharaButtonRect();
		const bool charaClicked = _mouse.IsLeftTrig() && _mouse.IsInRect(charaBtn.x, charaBtn.y, charaBtn.w, charaBtn.h);
		if (charaClicked)
		{
			_showCharaStatus = !_showCharaStatus;
		}

		// バトルボタンのクリック判定
		const auto& btn = _battleUI.GetBattleButtonRect();
		const bool battleClicked = _mouse.IsLeftTrig() && _mouse.IsInRect(btn.x, btn.y, btn.w, btn.h);
		if (battleClicked)
		{
			// クリックされたら戦闘フェーズへ遷移
			_saveData.LoadFromSqlite();
			_battleSystem.Reset(_saveData, _afterStatus);
			_gamePhase = GamePhase::Battle;
		}
	}
	else if (_gamePhase == GamePhase::Battle)
	{
		float deltaTime = 1.0f / 60.0f; // お使いのフレームデルタタイム

		// バトルクラスへ丸投げして処理させる
		_battleSystem.Process(_mouse, _afterStatus, _saveData, deltaTime);

		// もしバトル終了フラグなどが立てば、ここでガチャフェーズに戻すなどの処理
		if(_battleSystem.IsBattleEnd())
		{
			_gamePhase = GamePhase::Gacha;
		}

		// 【デバッグ用】例えばBキーなどを押したらガチャ画面に戻る処理があるとテストしやすいです
		if (CheckHitKey(KEY_INPUT_B) == 1)
		{
			_gamePhase = GamePhase::Gacha;
		}
	}

	// 処理前のステータスを保存しておく
	STATUS oldStatus = _status;

	_cam.Update(_vPos, key);

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

	// ライト設定
	SetUseLighting(TRUE);
#if 0	// 平行ライト
	SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
	ChangeLightTypeDir(VGet(-1, -1, 0));
#endif
#if 1	// ポイントライト
	SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
	ChangeLightTypePoint(VAdd(_vPos,VGet(0,50.f,0)), 1000.f, 0.f, 0.005f, 0.f);
#endif

	// カメラ設定更新
	_cam.ApplyCamera();

	// 再生時間をセットする
	MV1SetAttachAnimTime(_handle, _attach_index, _play_time);

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
	}
	else if (_gamePhase == GamePhase::Battle)
	{
		_battleSystem.Render(_afterStatus);
	}

	return true;
}

