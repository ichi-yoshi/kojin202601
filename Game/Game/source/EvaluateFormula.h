#pragma once
#include "container.h"

class EvaluateFormula
{
public:
	EvaluateFormula() = delete;

	// 数式を評価して結果を返す
	static double Evaluate(const std::string& expression);

private:
	// 簡易的な数式評価関数（四則演算のみ対応）
	static double EvaluateSimpleFormula(const std::string& expr);
};

