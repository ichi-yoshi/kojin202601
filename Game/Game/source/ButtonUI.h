#pragma once
#include "container.h"

class ButtonUI
{
public:
	struct ButtonRect
	{
		int x;
		int y;
		int w;
		int h;
	};

	// ボタンの描画
	static void DrawButton(const ButtonRect& rect, const std::string& label);
};

