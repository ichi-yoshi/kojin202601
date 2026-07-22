#pragma once
#include "ButtonUI.h"
#include "MouseInput.h"

class DatabaseSelectorButtonUI
{
public:
	// データベース選択ボタンがクリックされたかどうかを取得する
	bool IsDbSelectClicked() const { return _isDbSelectClicked; }

	// データベース選択ボタンのクリック状態をリセットする
	void Update(MouseInput& mouse);

	// データベース選択ボタンの描画
	void Draw() const;

private:
	// データベース選択ボタンの描画
	void DrawDbSelectButton() const;

	// データベース選択ボタンの矩形
	ButtonUI::ButtonRect _dbSelectButton{ 700, 40, 160, 40 };
	const ButtonUI::ButtonRect& GetDbSelectButtonRect() const;

private:
	bool _isDbSelectClicked = false;	// データベース選択ボタンがクリックされたかどうか
};

