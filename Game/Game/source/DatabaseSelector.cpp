#include "DatabaseSelector.h"
#include "MagicNumberConfig.h"

// 名前空間の使用の宣言
using namespace UIConfig;

DatabaseSelector::DatabaseSelector()
{
	_inputHandle = -1;
	_isFinished = false;
	_isCancelled = false;
	_databasePrefix.clear();
}

DatabaseSelector::~DatabaseSelector()
{
	if(_inputHandle != -1)
	{
		DeleteKeyInput(_inputHandle);
	}
}

void DatabaseSelector::StartInput()
{
	if(_inputHandle != -1)
	{
		DeleteKeyInput(_inputHandle);
	}

	// 最大30文字、半角英数字のみ（IME無効）
	_inputHandle = MakeKeyInput(30, FALSE, FALSE, FALSE);
	SetActiveKeyInput(_inputHandle);
	_isFinished = false;
	_isCancelled = false;
	_databasePrefix.clear();
}

void DatabaseSelector::Update()
{
	if(_inputHandle == -1) { return; }

	int status = CheckKeyInput(_inputHandle);

	if(CheckHitKey(KEY_INPUT_RETURN) == 1) // Enterキー（確定）
	{
		char buf[256] = "";		// 入力文字列を取得
		GetKeyInputString(buf, _inputHandle);
		_databasePrefix = buf;
		_isFinished = true;
	}
	else if(CheckHitKey(KEY_INPUT_ESCAPE) == 1) // Escキー（キャンセル）
	{
		_isCancelled = true;
	}
}

void DatabaseSelector::Draw() const
{
	if(_inputHandle == -1) { return; }

	int screenW = Layout::Screen.w;				// 画面幅
	int screenH = Layout::Screen.h;				// 画面高さ
	int boxW = Layout::DataBase::DBBoxWidth;	// データベース表示ボックスの幅
	int boxH = Layout::DataBase::DBBoxHeight;	// データベース表示ボックスの高さ
	int x1 = Layout::DataBase::DBBoxPos1.x;		// データベース表示ボックスの左上X座標
	int y1 = Layout::DataBase::DBBoxPos1.y;		// データベース表示ボックスの左上Y座標
	int x2 = Layout::DataBase::DBBoxPos2.x;		// データベース表示ボックスの右下X座標
	int y2 = Layout::DataBase::DBBoxPos2.y;		// データベース表示ボックスの右下Y座標
	int rowSpace = Layout::DataBase::DBSpacing;	// 行間のスペース
	int stringSpace = Common::ButtonSpacing;	// 文字列間のスペース
	int MarginLeft = Common::MarginRight;		// 左側の余白

	// 半透明暗色背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha::Dim);
	DrawBox(x1, y1, x2, y2,Color::Dim(), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, Alpha::Min);

	// 白枠線
	DrawBox(x1, y1, x2, y2, Color::White(), FALSE);
	SetFontSize(Font::Small);

	// テキストと説明
	DrawString(x1 + rowSpace, y1 + rowSpace, "セーブデータの切り替え / 新規作成", Color::White());
	DrawString(x1 + rowSpace, y1 + rowSpace * 2, "セーブデータのファイル名を入力してください (例: f )", Color::TextGray());
	DrawString(x1 + rowSpace, y1 + rowSpace * 3, "※拡張子(.sqlite3)は自動的に付与されます。", Color::DimGray());
	DrawString(x1 + rowSpace, y1 + rowSpace * 4, "※存在しない場合は新規作成します。", Color::DimGray());

	// 入力ボックス背景
	DrawBox(x1 + rowSpace, y1 + rowSpace * 5, x2 - rowSpace, y1 + rowSpace * 6, Color::DimGray(), TRUE);
	DrawBox(x1 + rowSpace, y1 + rowSpace * 5, x2 - rowSpace, y1 + rowSpace * 6, Color::White(), FALSE);

	// 入力中文字列の描画
	DrawKeyInputString(x1 + rowSpace + MarginLeft, y1 + rowSpace * 5 + stringSpace, _inputHandle);

	// ガイドキー表示
	DrawString(x1 + rowSpace, y1 + rowSpace * 7, "[Enter] 決定 / [Esc] キャンセル", Color::TextGray());
}