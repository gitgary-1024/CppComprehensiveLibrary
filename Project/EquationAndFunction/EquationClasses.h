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
#include <bits/stdc++.h>
#include "../Expression/ExpressionEvaluator.h"

// 基类 Equation
class Equation {
	public:
		Equation() {
			// 默认构造函数
		}
		
		virtual ~Equation() {
			// 析构函数
		}
		
		// 纯虚函数，用于计算方程的解
		virtual std::vector<double> solve() = 0;
};

// 线性方程类，继承自 Equation
class LinearEquation : public Equation {
	private:
		double a; // 线性方程 ax + b = 0 中的 a
		double b; // 线性方程 ax + b = 0 中的 b
	public:
		LinearEquation() {}
		
		// 构造函数，从表达式字符串构造线性方程
		LinearEquation(const std::string& expr) {
			std::string leftExpr, rightExpr;
			size_t equalPos = expr.find('=');
			
			if (equalPos != std::string::npos) {
				leftExpr = expr.substr(0, equalPos);
				rightExpr = expr.substr(equalPos + 1);
			}
			else {
				throw std::runtime_error("Invalid equation: no '=' found.");
			}
			
			ExpressionEvaluator leftEvaluator(leftExpr);
			ExpressionEvaluator rightEvaluator(rightExpr);
			std::unordered_map<std::string, double> vars;
			// 假设方程为 ax + b = 0，我们可以通过设置 x 的不同值来求解 a 和 b
			vars["x"] = 0;
			double leftVal1 = leftEvaluator.evaluate(vars);
			double rightVal1 = rightEvaluator.evaluate(vars);
			vars["x"] = 1;
			double leftVal2 = leftEvaluator.evaluate(vars);
			double rightVal2 = rightEvaluator.evaluate(vars);
			a = (leftVal2 - leftVal1) - (rightVal2 - rightVal1);
			b = (leftVal1 - rightVal1);
		}
		
		// 实现 solve 函数，用于求解线性方程
		std::vector<double> solve() override {
			std::vector<double> solutions;
			
			if (a != 0) {
				solutions.push_back(-b / a);
			}
			
			return solutions;
		}
};

// 线性方程组类，继承自 Equation，仅供其他.h文件使用
class _LinearEquationSystem {
	private:
		std::vector<std::vector<double >> coefficients;
		std::vector<double> constants;
		int n;
		
	public:
		_LinearEquationSystem() {}
		
		_LinearEquationSystem(const
		                      std::vector<std::vector<double >> & coeffs, const std::vector<double>& consts)
			: coefficients(coeffs), constants(consts), n(coeffs.size()) {}
			
		std::vector<double> solve() {
			std::vector<double> solutions(n);
			
			// 高斯消元法
			for (int i = 0; i < n; ++i) {
				// 选主元
				int maxRow = i;
				
				for (int k = i + 1; k < n; ++k) {
					if (std::abs(coefficients[k][i]) > std::abs(coefficients[maxRow][i])) {
						maxRow = k;
					}
				}
				
				std::swap(coefficients[i], coefficients[maxRow]);
				std::swap(constants[i], constants[maxRow]);
				
				// 消元
				for (int j = i + 1; j < n; ++j) {
					double factor = coefficients[j][i] / coefficients[i][i];
					
					for (int k = i; k < n; ++k) {
						coefficients[j][k] -= factor * coefficients[i][k];
					}
					
					constants[j] -= factor * constants[i];
				}
			}
			
			// 回代求解
			for (int i = n - 1; i >= 0; --i) {
				double sum = 0;
				
				for (int j = i + 1; j < n; ++j) {
					sum += coefficients[i][j] * solutions[j];
				}
				
				solutions[i] = (constants[i] - sum) / coefficients[i][i];
			}
			
			return solutions;
		}
};

// 线性方程组类，继承自 Equation
class LinearEquationSystem : public Equation {
	private:
		std::vector<std::vector<double >> coefficients; // 方程组的系数矩阵
		std::vector<double> constants; // 方程组的常数项向量
	public:
		LinearEquationSystem() {}
		
		// 构造函数，从表达式字符串向量构造线性方程组
		LinearEquationSystem(const std::vector<std::string>& exprs,
		                     const std::vector<std::string>& variables) {
			int n = exprs.size();
			coefficients.resize(n, std::vector<double>(n));
			constants.resize(n);
			
			for (int i = 0; i < n; ++i) {
				std::string leftExpr, rightExpr;
				size_t equalPos = exprs[i].find('=');
				
				if (equalPos != std::string::npos) {
					leftExpr = exprs[i].substr(0, equalPos);
					rightExpr = exprs[i].substr(equalPos + 1);
				}
				else {
					throw std::runtime_error("Invalid equation: no '=' found.");
				}
				
				ExpressionEvaluator leftEvaluator(leftExpr);
				ExpressionEvaluator rightEvaluator(rightExpr);
				std::unordered_map<std::string, double> vars;
				
				// 初始化所有变量的值为 0
				for (const auto& var : variables) {
					vars[var] = 0;
				}
				
				for (int j = 0; j < n; ++j) {
					vars[variables[j]] = 1;
					double leftVal1 = leftEvaluator.evaluate(vars);
					double rightVal1 = rightEvaluator.evaluate(vars);
					vars[variables[j]] = 0;
					double leftVal2 = leftEvaluator.evaluate(vars);
					double rightVal2 = rightEvaluator.evaluate(vars);
					coefficients[i][j] = (leftVal1 - leftVal2) - (rightVal1 - rightVal2);
				}
				
				constants[i] = -(leftEvaluator.evaluate(vars) - rightEvaluator.evaluate(vars));
			}
		}
		
		// 实现 solve 函数，用于求解线性方程组
		std::vector<double> solve() override {
			int n = coefficients.size();
			std::vector<double> solutions(n);
			
			// 高斯消元法
			for (int i = 0; i < n; ++i) {
				// 选主元
				int maxRow = i;
				
				for (int k = i + 1; k < n; ++k) {
					if (std::abs(coefficients[k][i]) > std::abs(coefficients[maxRow][i])) {
						maxRow = k;
					}
				}
				
				std::swap(coefficients[i], coefficients[maxRow]);
				std::swap(constants[i], constants[maxRow]);
				
				// 检查主元是否为零
				if (std::abs(coefficients[i][i]) < 1e-10) {
					throw std::runtime_error("Linear equation system has no unique solution.");
				}
				
				// 消元
				for (int j = i + 1; j < n; ++j) {
					double factor = coefficients[j][i] / coefficients[i][i];
					
					for (int k = i; k < n; ++k) {
						coefficients[j][k] -= factor * coefficients[i][k];
					}
					
					constants[j] -= factor * constants[i];
				}
			}
			
			// 回代求解
			for (int i = n - 1; i >= 0; --i) {
				double sum = 0;
				
				for (int j = i + 1; j < n; ++j) {
					sum += coefficients[i][j] * solutions[j];
				}
				
				solutions[i] = (constants[i] - sum) / coefficients[i][i];
			}
			
			return solutions;
		}
};

// 非线性方程组类，继承自 Equation
class NonlinearEquationSystem : public Equation {
	private:
		std::vector<std::function<double(const std::vector<double>&) >> equations; // 非线性方程组
		int numEquations; // 方程的数量
		double tolerance; // 求解的容差
		int maxIterations; // 最大迭代次数
		double initialGuess; // 初始化猜测值
		std::vector<std::string> variables;
		
	public:
		// 构造函数，从表达式字符串向量构造非线性方程组
		NonlinearEquationSystem(const std::vector<std::string>& exprs,
		                        const std::vector<std::string>& vars, double tol = 1e-6, int maxIter = 100,
		                        double initialGuess = 0.0)
			: numEquations(exprs.size()), tolerance(tol), maxIterations(maxIter),
			  initialGuess(initialGuess), variables(vars) {
			for (const auto& expr : exprs) {
				equations.push_back([expr, vars](const std::vector<double>& x) {
					std::unordered_map<std::string, double> varMap;
					
					for (size_t i = 0; i < vars.size(); ++i) {
						varMap[vars[i]] = x[i];
					}
					
					ExpressionEvaluator evaluator(expr);
					return evaluator.evaluate(varMap);
				});
			}
		}
		
		// 计算雅可比矩阵
		std::vector<std::vector<double >> jacobian(const std::vector<double>& x) {
			std::vector<std::vector<double >> jac(numEquations, std::vector<double>
			                                      (numEquations));
			double h = 1e-6; // 数值微分的步长
			
			for (int i = 0; i < numEquations; ++i) {
				for (int j = 0; j < numEquations; ++j) {
					std::vector<double> x_plus_h = x;
					x_plus_h[j] += h;
					std::vector<double> x_minus_h = x;
					x_minus_h[j] -= h;
					jac[i][j] = (equations[i](x_plus_h) - equations[i](x_minus_h)) / (2 * h);
				}
			}
			return jac;
		}
		
		// 矩阵求逆（简单实现，可替换为更高效的方法）
		std::vector<std::vector<double >> inverse(const std::vector<std::vector<double >>
		        & matrix) {
			int n = matrix.size();
			std::vector<std::vector<double >> augmented(n, std::vector<double>(2 * n));
			
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					augmented[i][j] = matrix[i][j];
				}
				
				augmented[i][i + n] = 1.0;
			}
			
			// 高斯 - 约旦消元法求逆
			for (int i = 0; i < n; ++i) {
				double pivot = augmented[i][i];
				
				if (std::abs(pivot) < 1e-10) {
					throw std::runtime_error("Matrix is singular, cannot find inverse.");
				}
				
				for (int j = 0; j < 2 * n; ++j) {
					augmented[i][j] /= pivot;
				}
				
				for (int k = 0; k < n; ++k) {
					if (k != i) {
						double factor = augmented[k][i];
						
						for (int j = 0; j < 2 * n; ++j) {
							augmented[k][j] -= factor * augmented[i][j];
						}
					}
				}
			}
			std::vector<std::vector<double >> inv(n, std::vector<double>(n));
			
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					inv[i][j] = augmented[i][j + n];
				}
			}
			return inv;
		}
		
		// 矩阵向量乘法
		std::vector<double> matrixVectorMultiply(const
		        std::vector<std::vector<double >> & matrix, const std::vector<double>& vector) {
			int n = matrix.size();
			std::vector<double> result(n);
			
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					result[i] += matrix[i][j] * vector[j];
				}
			}
			
			return result;
		}
		
		// 向量减法
		std::vector<double> vectorSubtract(const std::vector<double>& a,
		                                   const std::vector<double>& b) {
			int n = a.size();
			std::vector<double> result(n);
			
			for (int i = 0; i < n; ++i) {
				result[i] = a[i] - b[i];
			}
			
			return result;
		}
		
		// 计算方程组的值
		std::vector<double> evaluateEquations(const std::vector<double>& x) {
			std::vector<double> values(numEquations);
			
			for (int i = 0; i < numEquations; ++i) {
				values[i] = equations[i](x);
			}
			
			return values;
		}
		
		// 实现 solve 函数，使用牛顿 - 拉夫逊方法求解非线性方程组
		std::vector<double> solve() override {
			std::vector<double> x(numEquations, initialGuess); // 初始猜测值
			
			for (int iter = 0; iter < maxIterations; ++iter) {
				std::vector<std::vector<double >> J = jacobian(x);
				std::vector<std::vector<double >> J_inv;
				
				try {
					J_inv = inverse(J);
				}
				catch (const std::runtime_error& e) {
					throw std::runtime_error("Failed to find inverse of Jacobian matrix.");
				}
				std::vector<double> F = evaluateEquations(x);
				std::vector<double> delta_x = matrixVectorMultiply(J_inv, F);
				x = vectorSubtract(x, delta_x);
				// 检查收敛条件
				double error = 0.0;
				
				for (double val : F) {
					error += val * val;
				}
				if (std::sqrt(error) < tolerance) {
					return x;
				}
			}
			
			throw std::runtime_error("Newton-Raphson method did not converge.");
		}
};
