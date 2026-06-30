#include "DatabaseSelector.h"

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
		char buf[256] = "";
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
	int screenW = 1280;
	int screenH = 720;
	int boxW = 520;
	int boxH = 210;
	int x1 = (screenW - boxW) / 2;
	int y1 = (screenH - boxH) / 2;
	int x2 = x1 + boxW;
	int y2 = y1 + boxH;

	// 半透明暗色背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawBox(x1, y1, x2, y2, GetColor(20, 20, 20), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 白枠線
	DrawBox(x1, y1, x2, y2, GetColor(255, 255, 255), FALSE);
	SetFontSize(18);

	// テキストと説明
	DrawString(x1 + 30, y1 + 25, "データベースの切り替え / 新規作成", GetColor(255, 255, 255));
	DrawString(x1 + 30, y1 + 60, "データベースのファイル名を入力してください (例: f )", GetColor(200, 200, 200));
	DrawString(x1 + 30, y1 + 80, "※拡張子(.sqlite3)は自動的に付与されます。", GetColor(140, 140, 140));
	DrawString(x1 + 30, y1 + 100, "※存在しない場合は新規作成します。", GetColor(140, 140, 140));

	// 入力ボックス背景
	DrawBox(x1 + 30, y1 + 115, x2 - 30, y1 + 145, GetColor(45, 45, 45), TRUE);
	DrawBox(x1 + 30, y1 + 115, x2 - 30, y1 + 145, GetColor(180, 180, 180), FALSE);

	// 入力中文字列の描画
	DrawKeyInputString(x1 + 40, y1 + 121, _inputHandle);

	// ガイドキー表示
	DrawString(x1 + 30, y1 + 165, "[Enter] 決定 / [Esc] キャンセル", GetColor(180, 180, 180));
	SetFontSize(16); // デフォルトに戻す
}