#pragma once
#include "CharaAfterStatus.h"
#include "ButtonUI.h"

class StatusUI
{
public:
	const ButtonUI::ButtonRect& GetCharaButtonRect() const;

	void Draw(const CharaAfterStatus& afterStatus, bool visible) const;
private:
	void DrawCharaButton() const;
	void DrawStatusBox(const CharaAfterStatus& afterStatus) const;

	ButtonUI::ButtonRect _charaButton{ 190,40,160,40 };
};

