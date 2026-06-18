#pragma once
#include <string>

class EvaluateFormula
{
public:
	EvaluateFormula() = delete;

	static double Evaluate(const std::string& expression);
private:
	static double EvaluateSimpleFormula(const std::string& expr);

};

