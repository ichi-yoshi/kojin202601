#pragma once
class MouseInput
{
public:
	void Update();

	int GetX() const { return _x; }
	int GetY() const { return _y; }
	int GetWheel() const { return _wheel; }

	bool IsLeftDown() const { return _leftDown; }
	bool IsLeftTrig() const { return _leftTrig; }

	bool IsInRect(int x, int y, int w, int h) const;

private:
	int _x = 0;
	int _y = 0;
	int _wheel = 0;

	bool _leftDown = false;
	bool _leftTrig = false;
	bool _prevLeftDown = false;
};

