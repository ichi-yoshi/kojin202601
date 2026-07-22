#include "EvaluateFormula.h"

double EvaluateFormula::Evaluate(const std::string& expression)
{
	// 空白を完全に除去
	std::string expr = std::regex_replace(expression, std::regex("\\s+"), "");

	// 一番内側のカッコ () を探して、その中身を先に計算して数字に置き換える処理を繰り返す
	std::smatch match;

	// カッコの中身を計算して置換するループ
	while(std::regex_search(expr, match, std::regex("\\(([^()]+)\\)")))
	{
		// カッコの中身を計算する
		double subResult = EvaluateSimpleFormula(match[1].str());

		// カッコで囲まれた部分を計算結果の文字列に置換
		expr = std::regex_replace(expr, std::regex("\\(" + std::regex_replace(match[1].str(), std::regex("[\\+\\-\\*\\/]"), "\\$&") + "\\)"), std::to_string(subResult));
	}

	// カッコがすべて消えたら、残った四則演算を計算する
	double finalResult = EvaluateSimpleFormula(expr);

	// 小数点第1位までに切り捨て
	return std::floor(finalResult * 10.0) / 10.0;
}

double EvaluateFormula::EvaluateSimpleFormula(const std::string& expr)
{
	// 数式を解析して、数値と演算子を分離する
	std::vector<double> values;
	std::vector<char> ops;

	// 文字列ストリームを使って数値と演算子を順番に読み取る
	std::stringstream ss(expr);
	double val;
	char op;

	// 最初の数値を読み取る
	if(ss >> val) { values.push_back(val); }

	// その後、演算子と数値を交互に読み取る
	while(ss >> op >> val)
	{
		ops.push_back(op);		// 演算子を保存
		values.push_back(val);	// 数値を保存
	}

	// 数値が空の場合は 0.0 を返す
	if(values.empty()) return 0.0;

	// 掛け算（*）と割り算（/）を先に処理
	for(size_t i = 0; i < ops.size(); )
	{
		if(ops[i] == '*' || ops[i] == '/')
		{
			if(ops[i] == '*') values[i] = values[i] * values[i + 1];
			else values[i] = (values[i + 1] != 0.0) ? values[i] / values[i + 1] : 0.0;

			values.erase(values.begin() + i + 1);
			ops.erase(ops.begin() + i);
		}
		else
		{
			i++;
		}
	}

	// 足し算（+）と引き算（-）を処理
	double result = values[0];
	for(size_t i = 0; i < ops.size(); i++)
	{
		if(ops[i] == '+') result += values[i + 1];
		if(ops[i] == '-') result -= values[i + 1];

		values.erase(values.begin() + i + 1);
		ops.erase(ops.begin() + i);
	}

	return result;
}
