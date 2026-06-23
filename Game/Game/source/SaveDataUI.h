#pragma once
#include "SaveData.h"
#include "ButtonUI.h"
#include "MouseInput.h"

class SaveDataUI
{
public:
	const ButtonUI::ButtonRect& GetSaveDataButtonRect() const;
	void Update(MouseInput& mouse);
	void Draw(const SaveData& saveData,bool visible) const;
	bool IsSaveDataClicked() const { return _isSaveDataClicked; }
private:
	void DrawSaveDataButton() const;
	void DrawSaveDataBox(const SaveData& saveData) const;
	ButtonUI::ButtonRect _saveDataButton{ 530, 40, 160, 40 };
	bool _isSaveDataClicked = false;
	bool _prevKeyS = false;
};

