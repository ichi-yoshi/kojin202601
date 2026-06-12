#pragma once
#include "ButtonUI.h"
#include "MouseInput.h"
#include "SqliteGauge.h"

class GaugeUI
{
public:
	const ButtonUI::ButtonRect& GetGaugeButtonRect() const;
	void SetPattern(int targetX,int targetW,double speed);
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	void Update(MouseInput& mouse);
	bool CheckHit();
	void Draw();

	void SetPos(int x, int y, int w, int h);

	void Reset();
	
	bool IsSuccess() const { return _isSuccess; };
	bool HasResult() const { return _hasResult; };
private:
	ButtonUI::ButtonRect _gaugeButton{500,600,400,100};

	int _targetX;
	int _targetW;
	double _speed;
	double _currentValue;

	bool _isMoving;
	bool _isSuccess;
	bool _hasResult;

	bool _prevKeyF = false;

	SqliteGauge _sqliteGauge;
};

