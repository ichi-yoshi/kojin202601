#pragma once
#include "ButtonUI.h"
#include "MouseInput.h"

class DatabaseSelectorButtonUI
{
public:
	const ButtonUI::ButtonRect& GetDbSelectButtonRect() const;
	bool IsDbSelectClicked() const { return _isDbSelectClicked; }
	void Update(MouseInput& mouse);
	void Draw() const;
private:
	void DrawDbSelectButton() const;
	ButtonUI::ButtonRect _dbSelectButton{ 700, 40, 160, 40 };
	bool _isDbSelectClicked = false;
};

