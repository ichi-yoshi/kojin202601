#pragma once
#include "CharaAfterStatus.h"
#include "ButtonUI.h"
#include "MouseInput.h"
#include "container.h"

class StatusUI
{
public:
	const ButtonUI::ButtonRect& GetCharaButtonRect() const;
	void Update(MouseInput& mouse);
	void Draw(const CharaAfterStatus& afterStatus, bool visible) const;
	bool IsCharaClicked() const { return _isCharaClicked; }
private:
	void DrawCharaButton() const;
	void DrawStatusBox(const CharaAfterStatus& afterStatus) const;

	ButtonUI::ButtonRect _charaButton{ 190,40,160,40 };

	bool _isCharaClicked = false;
};

