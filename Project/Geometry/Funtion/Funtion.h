/*
    Copyright (c) June 9, 2025 Gitgary-1024

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "../../Expression/ExpressionEvaluator.h"
#include "../../Configuration.h"
#include <bits/stdc++.h>

class Function {
	private:
		std::set<char, std::greater<char >> unknowList;
		ExpressionEvaluator evaluator;
	public:
		Function() {}
		
		Function(const std::string& expression)
			: evaluator(expression) {
			// 统计未知数
			for (const auto& content : expression) {
				if ('a' <= content && content <= 'z') {
					unknowList.insert(content);
				}
			}
		}
		
		Function(const Function& other) : unknowList(other.unknowList), evaluator(other.evaluator) {
		}
		
		// 赋值运算符重载
		Function& operator = (const Function& other) {
			evaluator = other.evaluator;
			unknowList = other.unknowList;
			return *this;
		}
		
		~Function() {
			// 释放空间
		}
		
		double evaluate(const std::unordered_map<std::string, double>& varValues)
		const {
			try {
				return evaluator.evaluate(varValues);
			}
			catch (const std::exception& e) {
				std::cerr << "Error evaluating function: " << e.what() << std::endl;
				return 0.0;
			}
		}
		
		void printFunction() const {
			// print the function
			printf("Function:f(");
			
			for (const auto& unknow : unknowList) {
				printf("%c,", unknow);
			}
			
			printf(")=");
			evaluator.printExpression();
			printf("\n");
		}
		
		std::string getAnalytical() const {
			return evaluator.getExpression();
		}
		
		ExpressionEvaluator getExpression() {
			return evaluator;
		}
};

// 求偏导数
double partialDerivative(const Function& expression,
                         const std::unordered_map<std::string, double>& varValues,
                         const std::string& variable) {
	std::unordered_map<std::string, double> leftIncrementValues = varValues;
	std::unordered_map<std::string, double> rightIncrementValues = varValues;
	// 对指定变量进行增量和减量操作
	leftIncrementValues[variable] += microIncrement;
	rightIncrementValues[variable] -= microIncrement;
	double leftValue = expression.evaluate(leftIncrementValues);
	double rightValue = expression.evaluate(rightIncrementValues);
	double derivativeValue = (leftValue - rightValue) / (2 * microIncrement);
	return derivativeValue;
}

// 求数值积分（梯形法则）
double numericalIntegration(const Function& expression,
                            const std::string& variable, double a, double b, int n) {
	double h = (b - a) / n;
	std::unordered_map<std::string, double> varValues;
	varValues[variable] = a;
	double sum = 0.5 * expression.evaluate(varValues);
	varValues[variable] = b;
	sum += 0.5 * expression.evaluate(varValues);
	
	for (int i = 1; i < n; ++i) {
		varValues[variable] = a + i * h;
		sum += expression.evaluate(varValues);
	}
	
	double integral = h * sum;
	return integral;
}

// 计算极限
double numericalLimit(const Function& expression, const std::string& variable,
                      double limitPoint) {
	// 检查输入参数的有效性
	if (std::isnan(limitPoint)) {
		std::cerr << "Invalid limit point: NaN." << std::endl;
		return std::nan("");
	}
	
	std::unordered_map<std::string, double> varValues;
	double leftValue, rightValue;
	// 从左边趋近极限点
	varValues[variable] = limitPoint - microIncrement;
	leftValue = expression.evaluate(varValues);
	// 从右边趋近极限点
	varValues[variable] = limitPoint + microIncrement;
	rightValue = expression.evaluate(varValues);
	
	// 如果左右极限相等，则认为极限存在
	if (std::abs(leftValue - rightValue) < microError) {
		return (leftValue + rightValue) / 2.0;
	}
	else {
		throw std::runtime_error("Limit does not exist");
		return std::nan("");
	}
}
