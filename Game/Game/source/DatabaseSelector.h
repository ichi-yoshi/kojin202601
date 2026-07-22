#pragma once
#include "container.h"

class DatabaseSelector
{
public:
	DatabaseSelector();
	~DatabaseSelector();

	// 入力を開始する
	void StartInput();

	// 入力の更新
	void Update();

	// 入力の描画
	void Draw() const;

	// 入力が完了したかどうかを取得する
	bool IsFinished() const { return _isFinished; }

	// 入力がキャンセルされたかどうかを取得する
	bool IsCancelled() const { return _isCancelled; }

	// 入力されたデータベースの接頭辞を取得する
	std::string GetDatabasePrefix() const { return _databasePrefix; }
private:
	int _inputHandle;	// 入力ハンドル
	bool _isFinished;	// 入力が完了したかどうか
	bool _isCancelled;	// 入力がキャンセルされたかどうか
	std::string _databasePrefix;	// 入力されたデータベースの接頭辞
};

