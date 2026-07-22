#pragma once
#include "container.h"

// フォントサイズの定数を定義
namespace FontSize 
{
	constexpr int Small = 16;
	constexpr int History = 18;
	constexpr int Medium = 20;
	constexpr int Large = 24;
	constexpr int ExtraLarge = 36;
}

// 色の定数を定義
namespace Color
{
    inline unsigned int Yellow() { return GetColor(255, 255, 0); }
    inline unsigned int Red() { return GetColor(255, 0, 0); }
    inline unsigned int LightRed() { return GetColor(255, 50, 50); }
    inline unsigned int Gold() { return GetColor(255, 215, 0); }
    inline unsigned int White() { return GetColor(255, 255, 255); }
    inline unsigned int Gray() { return GetColor(150, 150, 150); }
    inline unsigned int DarkGray() { return GetColor(100, 100, 100); }
    inline unsigned int LightBlue() { return GetColor(120, 200, 255); }
    inline unsigned int Green() { return GetColor(0, 255, 0); }
    inline unsigned int TextGray() { return GetColor(200, 200, 200); }
    inline unsigned int HPBlue() { return GetColor(50, 150, 255); }
	inline unsigned int Black() { return GetColor(0, 0, 0); }
}

// レイアウトの定数を定義
namespace Layout
{
    // フェーズメッセージ関連
    constexpr int PhaseMsgX = 100;
    constexpr int PhaseMsgY = 50;
    constexpr int PhaseTimerY = 80;

    // リザルト画面
    constexpr int ResultMsgX = 200;
    constexpr int ResultTitleY = 250;
    constexpr int ResultMaxDmgY = 330;
    constexpr int ResultHpBonusY = 370;
    constexpr int ResultCoinY = 420;
    constexpr int ResultNextSceneY = 500;

    // 敵UI
    constexpr int EnemyInfoX = 100;
    constexpr int EnemyNameY = 140;
    constexpr int EnemyBarTop = 170;
    constexpr int EnemyBarBottom = 190;
    constexpr int EnemyBarWidth = 400;
    constexpr int EnemyHpTextY = 200;

    // プレイヤーUI
    constexpr int PlayerInfoX = 100;
    constexpr int PlayerNameY = 240;
    constexpr int PlayerBarTop = 270;
    constexpr int PlayerBarBottom = 290;
    constexpr int PlayerBarWidth = 400;
    constexpr int PlayerHpTextY = 300;

    // ダメージ履歴
    constexpr int DHistoryX = 900;
    constexpr int DHistoryStartY = 100;
    constexpr int DHistoryTitleSpacing = 25;
    constexpr int DHistoryLineSpacing = 22;
    constexpr int DHistoryMaxY = 650;

	constexpr int ScreenWidth = 1920;
	constexpr int ScreenHeight = 1080;
	constexpr int ScreenCenterX = ScreenWidth / 2;
	constexpr int ScreenCenterY = ScreenHeight / 2;
}

// ゲーム内のシステムパラメータを定義
namespace SystemParam
{
    constexpr double PlayerPinchHpRate = 0.2;  // HPが20%以下でピンチ色に変更
    constexpr double FlashMaxTime = 0.2;       // フラッシュの最大時間
    constexpr int FlashMaxAlpha = 120;         // フラッシュの最大透明度
	constexpr int MaxAlphaByte = 255;          // 最大のアルファ値
}