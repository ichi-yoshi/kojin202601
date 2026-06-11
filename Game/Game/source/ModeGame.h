#include "appframe.h"
#include "CameraBase.h"
#include <string>
#include "MouseInput.h"
#include "Gacha.h"
#include "GachaBasicStatus.h"
#include "GachaArmor.h"
#include "GachaSystem.h"
#include "SaveEquipment.h"
#include "GachaUI.h"
#include "StatusUI.h"
#include "CharaAfterStatus.h"
#include "CharaBase.h"
#include "SaveCharaStatus.h"
#include "BattleUI.h"
#include "GaugeUI.h"
#include "CircleUI.h"
#include "BattleTimer.h"
#include "ModeGameBattle.h"


// 計算用マクロ
#define	PI	(3.1415926535897932386f)
#define	DEG2RAD(x)			( ((x) / 180.0f ) * PI )
#define	RAD2DEG(x)			( ((x) * 180.0f ) / PI )

// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

protected:

	// カメラ
	CameraBase	_cam;

	// 3Dモデル描画用
	int _handle;
	int _attach_index;
	float _total_time;
	float _play_time;
	VECTOR _vPos;	// 位置
	VECTOR _vDir;	// 向き
	float _colSubY;	// コリジョン判定時のY補正(腰位置）

	enum class STATUS
	{
		NONE,
		WAIT,
		WALK,
		_EOT_
	};
	STATUS _status;

	enum class GamePhase
	{
		Gacha,
		Battle,
		_EOT_
	};
	GamePhase _gamePhase= GamePhase::Gacha;

	// マップ用
	int _handleMap;
	int _handleSkySphere;
	int _frameMapCollision;

	// デバッグ用
	bool _bViewCollision;

	Gacha _gacha;
	GachaBasicStatus _gachaBasic;
	GachaArmor _gachaArmor;
	GachaSystem _gachaSystem;

	GachaContext gachaCtx
	{
		_mouse,
		_gachaUI,
		_gacha,
		_gachaBasic,
		_gachaArmor,
		_saveEquipment,
		_saveCharaStatus,
		_pendingResult,
		_afterStatus,
		_charaBase
	};

	SaveEquipment _saveEquipment;
	SaveCharaStatus _saveCharaStatus;

	MouseInput _mouse;

	GachaUI _gachaUI;
	PendingGachaResult _pendingResult;

	StatusUI _statusUI;
	CharaAfterStatus _afterStatus;
	bool _showCharaStatus = false;

	CharaBase _charaBase;

	BattleUI _battleUI;

	ModeGameBattle _battleSystem;
}; 
