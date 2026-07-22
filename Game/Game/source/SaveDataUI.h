#pragma once
#include "SaveData.h"
#include "ButtonUI.h"
#include "MouseInput.h"
#include "container.h"

class SaveDataUI
{
public:
	void Update(MouseInput& mouse);
	void Draw(const SaveData& saveData,bool visible) const;

	// セーブデータボタンがクリックされたかどうかを取得
	bool IsSaveDataClicked() const { return _isSaveDataClicked; }

private:
	// セーブデータボタンの描画
	void DrawSaveDataButton() const;

	// セーブデータボックスの描画
	void DrawSaveDataBox(const SaveData& saveData) const;

	// セーブデータボタンの矩形
	const ButtonUI::ButtonRect& GetSaveDataButtonRect() const { return _saveDataButton; }
	ButtonUI::ButtonRect _saveDataButton{ 530, 40, 160, 40 };

private:
	bool _isSaveDataClicked = false;	// セーブデータボタンがクリックされたかどうか
	bool _prevKeyS = false;				// 前回のSキーの状態
};

