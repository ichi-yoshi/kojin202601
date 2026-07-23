#pragma once
#include "container.h"

// UI用の定数や構造体をまとめた名前空間
namespace UIConfig
{
	struct Point { int x, y; }; // 2D座標を表す構造体
	struct Size { int w, h; };  // 幅と高さを表す構造体

	// フォントサイズの定数を定義する名前空間
    namespace Font
    {
		constexpr int Small = 16;       // 小
		constexpr int Normal = 18;      // 標準
		constexpr int Medium = 20;      // 中
		constexpr int Large = 24;       // 大
		constexpr int Title = 32;       // タイトル用
		constexpr int ExtraLarge = 36;  // さらに大きい
    }

	// 色を取得するための関数をまとめた名前空間
    namespace Color
    {
		inline unsigned int Yellow() { return GetColor(255, 255, 0); }      // 黄色
		inline unsigned int Red() { return GetColor(255, 0, 0); }           // 赤色
		inline unsigned int LightRed() { return GetColor(255, 50, 50); }    // 明るい赤色
        inline unsigned int WeakRed() { return GetColor(255, 120, 120); }   // 弱い赤色
		inline unsigned int Gold() { return GetColor(255, 215, 0); }        // 金色
		inline unsigned int White() { return GetColor(255, 255, 255); }     // 白色
		inline unsigned int Gray() { return GetColor(150, 150, 150); }      // 灰色
		inline unsigned int DarkGray() { return GetColor(100, 100, 100); }  // 濃い灰色
		inline unsigned int TextGray() { return GetColor(200, 200, 200); }  // テキスト用の灰色
		inline unsigned int DimGray() { return GetColor(128, 128, 128); }   // 薄い灰色
		inline unsigned int Green() { return GetColor(0, 255, 0); }         // 緑色
        inline unsigned int Blue() { return GetColor(0, 0, 255); }          // 青色
		inline unsigned int LightBlue() { return GetColor(120, 200, 255); } // 明るい青色		
		inline unsigned int HPBlue() { return GetColor(50, 150, 255); }     // HPバー用の青色
		inline unsigned int Black() { return GetColor(0, 0, 0); }           // 黒色
		inline unsigned int BoxBg() { return GetColor(235, 235, 235); }     // ボックス背景色
    }

	// UIの共通設定値をまとめた名前空間
    namespace Common
    {
		constexpr Size  StandardButton = { 160, 36 };   // 標準的なボタンのサイズ
		constexpr Point ButtonPadding = { 10, 10 };     // ボタン内のテキストの余白
		constexpr Size  HPBar = { 400, 20 };            // HPバーのサイズ

		constexpr int DefaultPadding = 8;       // ボックス内の余白
		constexpr int DefaultIndent = 20;       // インデントの幅
		constexpr int RowHeight = 20;          // 行の高さ
		constexpr int RowSpacingExtra = 2;     // 行間の追加スペース
		constexpr int MarginRight = 20;         // 右側の余白
		constexpr int MarginTop = 20;           // 上側の余白
		constexpr int ButtonTopMargin = 12;     // ボタンの上側の余白
		constexpr int ButtonSpacing = 6;        // ボタン間のスペース

		constexpr int GaugeBarThickness = 1;    // ゲージバーの太さ
		constexpr int GaugeBarOverflow = 2;     // ゲージバーの上下のはみ出し量
		constexpr int TextMarginTop = 20;       // テキストの上側の余白

		constexpr Point ClearMsgOffset = { 50, 150 };   // CLEARメッセージの表示位置オフセット
		constexpr int HistoryTitleSpacing = 25;         // ダメージ履歴タイトルと最初の履歴の間隔
		constexpr int HistoryRowSpacing = 22;			// ダメージ履歴の行間隔
    }

	// UIのレイアウトに関する定数をまとめた名前空間
    namespace Layout
    {
		constexpr Size Screen = { 1920, 1080 }; // 画面サイズを定義

		// バトル画面のUI要素の位置やサイズを定義する名前空間
        namespace Battle
        {
			constexpr Point PhaseMsg = { 100, 50 };     // フェーズメッセージの表示位置
			constexpr Point PhaseTimer = { 100, 80 };   // フェーズタイマーの表示位置
			constexpr Point ResultMsg = { 200, 250 };   // 結果メッセージの表示位置
			constexpr int   ResultTitleY = 250;         // 結果タイトルのY座標
			constexpr int   ResultMaxDmgY = 330;        // 最大ダメージのY座標
			constexpr int   ResultHpBonusY = 370;		// 残りHPボーナスのY座標
			constexpr int   ResultCoinY = 420;			// 獲得コインのY座標
			constexpr int   ResultNextSceneY = 500;		// 次の画面への移動までの時間のY座標

			constexpr Point EnemyPos = { 100, 140 };	// 敵の情報表示の基準位置
			constexpr int   EnemyNameY = 140;			// 敵の名前表示のY座標
			constexpr int   EnemyBarTop = 170;			// 敵のHPバーの上端Y座標
			constexpr int   EnemyBarBottom = 190;		// 敵のHPバーの下端Y座標
			constexpr int   EnemyHpTextY = 200;			// 敵のHPテキスト表示のY座標

			constexpr Point PlayerPos = { 100, 240 };	// プレイヤーの情報表示の基準位置
			constexpr int   PlayerNameY = 240;			// プレイヤーの名前表示のY座標
			constexpr int   PlayerBarTop = 270;			// プレイヤーのHPバーの上端Y座標
			constexpr int   PlayerBarBottom = 290;		// プレイヤーのHPバーの下端Y座標
			constexpr int   PlayerHpTextY = 300;		// プレイヤーのHPテキスト表示のY座標

			constexpr Point HistoryPos = { 900, 100 };	// ダメージ履歴の表示位置
			constexpr int   HistoryMaxY = 650;			// ダメージ履歴の最大Y座標

			constexpr Point SaveDataPos = { 520, 100 };	// セーブデータの表示位置
			constexpr int   SaveDataWidth = 260;		// セーブデータの表示幅
        }

		// ガチャ画面のUI要素の位置やサイズを定義する名前空間
        namespace Gacha
        {
			constexpr Point ResultBase = { 480, 440 };	// ガチャ結果の描画開始位置
			constexpr int   ResultBoxWidth = 320;		// ガチャ結果のボックス幅
			constexpr int   SubTextXOffset = 120;		// サブテキストのX座標オフセット
			constexpr int   SavedAreaWidth = 360;		// 保存済み装備の表示エリアの幅
        }

		constexpr Point StatusBoxPos = { 20, 100 };		// ステータスボックスの表示位置
		constexpr int   StatusBoxWidth = 260;			// ステータスボックスの幅
    }

	// パラメータをまとめた名前空間
    namespace Param
    {
		constexpr double PinchHpRate = 0.2;				// ピンチ状態と判定するHPの割合
		constexpr float  FlashTime = 0.2f;				// 被ダメージ時の画面フラッシュの時間（秒）
		constexpr int    FlashAlpha = 120;				// 被ダメージ時の画面フラッシュの透明度（0～255）
		constexpr int    MaxAlphaByte = 255;			// 透明度の最大値（255）
		constexpr double GaugeSpeedDivider = 1000.0;	// ゲージの速度を調整するための除数
		constexpr float  HpPercentThreshold = 0.2f;		// HPバーの色を赤にする閾値（20%）
		constexpr int    coinCost = 1000;				// ガチャのコスト
		constexpr float  bonusStatus = 30.0f;			// レベルボーナスのステータス加算値
    }	
}