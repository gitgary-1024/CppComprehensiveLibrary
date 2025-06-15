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

#include<bits/stdc++.h>
#include "../../EquationAndFunction/EquationClasses.h"
#include <iostream>
#include <vector>
#include <string>
#include "../Funtion/Funtion.h" // 包含 Function 类的头文件

// 三次样条插值类
class CubicSpline {
	private:
		std::vector<double> x;
		std::vector<double> y;
		std::vector<double> h;
		std::vector<double> M;
		std::vector<double> a;
		std::vector<double> b;
		std::vector<double> c;
		std::vector<double> d;
		
	public:
		CubicSpline() {}
		
		// 在 CubicSpline 类的构造函数中添加调试输出
		CubicSpline(const std::vector<double>& x_values,
		            const std::vector<double>& y_values) : x(x_values), y(y_values) {
			int n = x.size() - 1;
			h.resize(n);
			
			for (int i = 0; i < n; ++i) {
				h[i] = x[i + 1] - x[i];
			}
			
			// 构造三对角线性方程组
			std::vector<std::vector<double >> coefficients(n - 1, std::vector<double>(n - 1,
			        0.0));
			std::vector<double> constants(n - 1);
			
			for (int i = 1; i < n; ++i) {
				if (i > 1) {
					coefficients[i - 1][i - 2] = h[i - 1] / (h[i - 1] + h[i]);
				}
				
				coefficients[i - 1][i - 1] = 2.0;
				
				if (i < n - 1) {
					coefficients[i - 1][i] = 1.0 - h[i - 1] / (h[i - 1] + h[i]);
				}
				
				constants[i - 1] = 6.0 * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i -
				                          1]) / (h[i - 1] + h[i]);
			}
			// 使用 LinearEquationSystem 求解线性方程组得到 M
			_LinearEquationSystem system(coefficients, constants);
			
			try {
				std::vector<double> solution = system.solve();
				M = solution;
				M.insert(M.begin(), 0.0); // 自然边界条件 M[0] = 0
				M.push_back(0.0); // 自然边界条件 M[n] = 0
			}
			catch (const std::exception& e) {
				std::cerr << "Error solving linear system: " << e.what() << std::endl;
			}
			// 计算每个小区间的多项式系数
			a = y;
			b.resize(n);
			c.resize(n);
			d.resize(n);
			
			for (int i = 0; i < n; ++i) {
				b[i] = (y[i + 1] - y[i]) / h[i] - h[i] * (2.0 * M[i] + M[i + 1]) / 6.0;
				c[i] = M[i] / 2.0;
				d[i] = (M[i + 1] - M[i]) / (6.0 * h[i]);
			}
		}
		
		// 插值计算
		// 在 CubicSpline 类的 interpolate 方法中添加边界检查
		double interpolate(double xi) const {
			int n = x.size() - 1;
			
			for (int i = 0; i < n; ++i) {
				if (x[i] <= xi && xi <= x[i + 1]) {
					double dx = xi - x[i];
					return a[i] + b[i] * dx + c[i] * dx * dx + d[i] * dx * dx * dx;
				}
			}
			
			std::cerr << "Input value " << xi << " is out of interpolation range." <<
			          std::endl;
			return 0.0;
		}
		
		// 返回每个区间的 Function 对象
		// 在 CubicSpline 类的 getFunctions 方法中添加异常捕获
		std::vector<Function> getFunctions() const {
			std::vector<Function> functions;
			
			for (size_t i = 0; i < x.size() - 1; ++i) {
				std::string expression = std::to_string(a[i]) + " + " + std::to_string(
				                             b[i]) + "*(x - " + std::to_string(x[i]) + ") + " +
				                         std::to_string(c[i]) + "*(x - " + std::to_string(x[i]) + ")^2 + " +
				                         std::to_string(d[i]) + "*(x - " + std::to_string(x[i]) + ")^3";
				std::cout << expression << std::endl;
				
				try {
					functions.emplace_back(expression);
				}
				catch (const std::exception& e) {
					std::cerr << "Error creating Function object for interval [" << x[i] << ", " <<
					          x[i + 1] << "]: " << e.what() << std::endl;
				}
			}
			
			return functions;
		}
		
		#ifdef _DEBUG
		// 打印多项式系数
		void printCoefficients() const {
			int n = x.size() - 1;
			
			for (int i = 0; i < n; ++i) {
				std::cout << "在区间 [" << x[i] << ", " << x[i + 1] << "] 上的多项式: "
				          << "S_" << i << "(x) = " << a[i] << " + "
				          << b[i] << "*(x - " << x[i] << ") + "
				          << c[i] << "*(x - " << x[i] << ")^2 + "
				          << d[i] << "*(x - " << x[i] << ")^3" << std::endl;
			}
		}
		
		#endif
};

// 拉格朗日插值类
class LagrangeInterpolation {
	private:
		std::vector<double> x;
		std::vector<double> y;
		
	public:
		LagrangeInterpolation() {}
		
		LagrangeInterpolation(const std::vector<double>& x_values,
		                      const std::vector<double>& y_values) : x(x_values), y(y_values) {}
		                      
		// 拉格朗日插值计算
		double interpolate(double xi) const {
			int n = x.size();
			double result = 0.0;
			
			for (int i = 0; i < n; ++i) {
				double l = 1.0;
				
				for (int j = 0; j < n; ++j) {
					if (i != j) {
						l *= (xi - x[j]) / (x[i] - x[j]);
					}
				}
				
				result += l * y[i];
			}
			
			return result;
		}
		
		// 输出拟合的拉格朗日多项式
		std::string getLagrangePolynomial() const {
			int n = x.size();
			std::string polynomial = "";
			
			for (int i = 0; i < n; ++i) {
				std::string term = std::to_string(y[i]) + " * ";
				
				for (int j = 0; j < n; ++j) {
					if (i != j) {
						term += "(x - " + std::to_string(x[j]) + ") / (" + std::to_string(
						            x[i]) + " - " + std::to_string(x[j]) + ")";
						            
						if (j < n - 1 && j != i - 1) {
							term += " * ";
						}
					}
				}
				
				if (i > 0) {
					polynomial += " + ";
				}
				
				polynomial += term;
			}
			
			// 修正缺少乘号的问题
			for (size_t pos = 0; (pos = polynomial.find(")(", pos)) != std::string::npos;) {
				polynomial.insert(pos + 1, " * ");
				pos += 2;
			}
			
			return polynomial;
		}
		
		// 返回拉格朗日插值的 Function 对象
		Function getFunction() const {
			std::string polynomial = getLagrangePolynomial();
			return Function(polynomial);
		}
};
