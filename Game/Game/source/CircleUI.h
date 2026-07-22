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
		int x;				// 円の中心のX座標
		int y;				// 円の中心のY座標
		int radius;			// 円の半径
		unsigned int color;	// 円の色
		bool isAlive;		// 円が生きているかどうか
	};

	// 円のパターンを設定
	void SetPattern(int minX, int maxX, int minY, int maxY, int radius, int count);

	bool Initialize(const std::string& dbPath, std::string* outError = nullptr);
	bool Update(MouseInput& mouse);
	
	void Draw();

	// 円のボタンの矩形を設定
	void SetPos(int x, int y, int w, int h);

	// 結果をリセット
	void Reset();

	// 結果を取得
	bool IsSuccess() const { return _isSuccess; };	
	bool HasResult() const { return _hasResult; };	

private:
	// 円のボタンの矩形
	ButtonUI::ButtonRect _circleButton{ 500,100,400,400 };

	// 円のボタンの矩形を取得
	const ButtonUI::ButtonRect& GetGaugeButtonRect() const { return _circleButton; }

private:
	int _targetX;		// 円のボタンの矩形のX座標
	int _targetW;		// 円のボタンの矩形の幅
	int _targetY;		// 円のボタンの矩形のY座標
	int _targetH;		// 円のボタンの矩形の高さ
	int _radius;		// 円の半径
	int _spawnCount;	// 円の数
	bool _isSuccess;	// 成功かどうか
	bool _hasResult;	// 結果があるかどうか

private:
	SqliteCircle _sqliteCircle;
	std::vector<CircleInstance> _circles;
};

