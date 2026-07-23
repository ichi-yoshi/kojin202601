#pragma once
#include "CharaAfterStatus.h"
#include "ButtonUI.h"
#include "MouseInput.h"
#include "container.h"

class StatusUI
{
public:
	
	void Update(MouseInput& mouse);
	void Draw(const CharaAfterStatus& afterStatus, bool visible, const MouseInput& mouse) const;
	bool IsCharaClicked() const { return _isCharaClicked; }

private:
	// キャラクターボタンの描画
	void DrawCharaButton() const;

	// ステータスボックスの描画
	void DrawStatusBox(const CharaAfterStatus& afterStatus, const MouseInput& mouse) const;

	// ツールチップの描画
	void DrawTooltip(int x, int y, const std::string& text) const;

	// キャラクターボタンの矩形を取得
	const ButtonUI::ButtonRect& GetCharaButtonRect() const { return _charaButton; }	
	ButtonUI::ButtonRect _charaButton{ 190,40,160,40 };

private:
	bool _isCharaClicked = false;	// キャラボタンがクリックされたかどうか
};

