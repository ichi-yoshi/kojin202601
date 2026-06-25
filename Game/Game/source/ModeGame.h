#include "appframe.h"
#include "CameraBase.h"
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
#include "BattleButtonUI.h"
#include "GaugeUI.h"
#include "CircleUI.h"
#include "BattleTimer.h"
#include "ModeGameBattle.h"
#include "SaveData.h" 
#include "SaveDataUI.h"
#include "container.h"

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
	float deltaTime;

	// ゲームフェーズ
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

	CharaBase _charaBase;
	Gacha _gacha;
	GachaBasicStatus _gachaBasic;
	GachaArmor _gachaArmor;
	GachaSystem _gachaSystem;
	GachaUI _gachaUI;
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
		_charaBase,
		_saveData
	};
	PendingGachaResult _pendingResult;
	StatusUI _statusUI;
	CharaAfterStatus _afterStatus;
	bool _showCharaStatus = false;

	MouseInput _mouse;
	CameraBase	_cam;	
	
	BattleButtonUI _battleButtonUI;
	ModeGameBattle _battleSystem;

	SaveData _saveData;
	SaveDataUI _saveDataUI;
	bool _showSaveData = false;
	SaveEquipment _saveEquipment;
	SaveCharaStatus _saveCharaStatus;
}; 
