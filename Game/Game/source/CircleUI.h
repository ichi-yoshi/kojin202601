#pragma once
#include "ButtonUI.h"
#include "MouseInput.h"
#include "SqliteCircle.h"
#include "container.h"

class CircleUI
{
public:
	struct CircleInstance
	{
		int x;
		int y;
		int radius;
		unsigned int color;
		bool isAlive;
	};

	const ButtonUI::ButtonRect& GetGaugeButtonRect() const;
	void SetPattern(int minX, int maxX, int minY, int maxY, int radius, int count);
	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	bool Update(MouseInput& mouse);
	
	void Draw();
	void SetPos(int x, int y, int w, int h);
	void Reset();

	bool IsSuccess() const { return _isSuccess; };
	bool HasResult() const { return _hasResult; };
private:
	ButtonUI::ButtonRect _circleButton{ 500,100,400,400 };

	int _targetX;
	int _targetW;
	int _targetY;
	int _targetH;
	int _radius;
	
	int _spawnCount;

	bool _isSuccess;
	bool _hasResult;

	SqliteCircle _sqliteCircle;
	std::vector<CircleInstance> _circles;
};

