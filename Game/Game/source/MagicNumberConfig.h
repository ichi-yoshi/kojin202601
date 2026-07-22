#pragma once
#include "container.h"

namespace UIConfig
{
    struct Point { int x, y; };
    struct Size { int w, h; };

    namespace Font
    {
        constexpr int Small = 16;
        constexpr int Normal = 18;
        constexpr int Medium = 20;
        constexpr int Large = 24;
        constexpr int Title = 32;
        constexpr int ExtraLarge = 36;
    }

    namespace Color
    {
        inline unsigned int Yellow() { return GetColor(255, 255, 0); }
        inline unsigned int Red() { return GetColor(255, 0, 0); }
        inline unsigned int LightRed() { return GetColor(255, 50, 50); }
        inline unsigned int Gold() { return GetColor(255, 215, 0); }
        inline unsigned int White() { return GetColor(255, 255, 255); }
        inline unsigned int Gray() { return GetColor(150, 150, 150); }
        inline unsigned int DarkGray() { return GetColor(100, 100, 100); }
        inline unsigned int TextGray() { return GetColor(200, 200, 200); }
        inline unsigned int DimGray() { return GetColor(128, 128, 128); }
        inline unsigned int Green() { return GetColor(0, 255, 0); }
        inline unsigned int LightBlue() { return GetColor(120, 200, 255); }
        inline unsigned int Blue() { return GetColor(0, 0, 255); }
        inline unsigned int HPBlue() { return GetColor(50, 150, 255); }
        inline unsigned int Black() { return GetColor(0, 0, 0); }
        inline unsigned int BoxBg() { return GetColor(235, 235, 235); }
        inline unsigned int WeakRed() { return GetColor(255, 120, 120); }
    }

    namespace Common
    {
        constexpr Size  StandardButton = { 160, 36 };
        constexpr Point ButtonPadding = { 10, 10 };
        constexpr Size  HPBar = { 400, 20 };

        constexpr int DefaultPadding = 8;
        constexpr int DefaultIndent = 20;
        constexpr int LineHeight = 20;
        constexpr int LineSpacingExtra = 2;
        constexpr int MarginRight = 20;
        constexpr int MarginTop = 20;
        constexpr int ButtonTopMargin = 12;
        constexpr int ButtonSpacing = 6;

        constexpr int GaugeBarThickness = 1;
        constexpr int GaugeBarOverflow = 2;
        constexpr int TextMarginTop = 20;

        constexpr Point ClearMsgOffset = { 50, 150 };
        constexpr int HistoryTitleSpacing = 25;
        constexpr int HistoryLineSpacing = 22;
    }

    namespace Layout
    {
        constexpr Size Screen = { 1920, 1080 };

        namespace Battle
        {
            constexpr Point PhaseMsg = { 100, 50 };
            constexpr Point PhaseTimer = { 100, 80 };
            constexpr Point ResultMsg = { 200, 250 };
            constexpr int   ResultTitleY = 250;
            constexpr int   ResultMaxDmgY = 330;
            constexpr int   ResultHpBonusY = 370;
            constexpr int   ResultCoinY = 420;
            constexpr int   ResultNextSceneY = 500;

            constexpr Point EnemyPos = { 100, 140 };
            constexpr int   EnemyNameY = 140;
            constexpr int   EnemyBarTop = 170;
            constexpr int   EnemyBarBottom = 190;
            constexpr int   EnemyHpTextY = 200;

            constexpr Point PlayerPos = { 100, 240 };
            constexpr int   PlayerNameY = 240;
            constexpr int   PlayerBarTop = 270;
            constexpr int   PlayerBarBottom = 290;
            constexpr int   PlayerHpTextY = 300;

            constexpr Point HistoryPos = { 900, 100 };
            constexpr int   HistoryMaxY = 650;
        }

        namespace Gacha
        {
            constexpr Point ResultBase = { 480, 440 };
            constexpr int   ResultBoxWidth = 320;
            constexpr int   SubTextXOffset = 120;
            constexpr int   SavedAreaWidth = 360;
        }

        constexpr Point StatusBoxPos = { 20, 100 };
        constexpr int   StatusBoxWidth = 260;
    }

    namespace Param
    {
        constexpr double PinchHpRate = 0.2;
        constexpr double FlashTime = 0.2;
        constexpr int    FlashAlpha = 120;
        constexpr int    MaxAlphaByte = 255;
        constexpr double GaugeSpeedDivider = 1000.0;
        constexpr float  DefaultMouseSensitivity = 0.005f;
        constexpr int    MousePosUninitialized = -1;
    }
}