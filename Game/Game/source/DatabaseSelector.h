#pragma once
#include "container.h"

class DatabaseSelector
{
public:
	DatabaseSelector();
	~DatabaseSelector();

	void StartInput();
	void Update();
	void Draw() const;
	bool IsFinished() const { return _isFinished; }
	bool IsCancelled() const { return _isCancelled; }
	std::string GetDatabasePrefix() const { return _databasePrefix; }
private:
	int _inputHandle;
	bool _isFinished;
	bool _isCancelled;
	std::string _databasePrefix;
};

