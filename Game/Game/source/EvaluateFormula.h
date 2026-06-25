#pragma once
#include "container.h"

class EvaluateFormula
{
public:
	EvaluateFormula() = delete;

	static double Evaluate(const std::string& expression);
private:
	static double EvaluateSimpleFormula(const std::string& expr);

};

