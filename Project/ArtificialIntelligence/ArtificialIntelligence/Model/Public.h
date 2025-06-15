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

#include <functional>
#include <math.h>
#include <vector>
#include <iostream>
#include <random>

// 定义激活函数类型
using ActivationFunction = std::function<double(double)>;

// Sigmoid激活函数
double sigmoid(double x) {
	return 1.0 / (1.0 + std::exp(-x));
}

// Sigmoid激活函数的导数
double sigmoid_derivative(double x) {
	double sig = sigmoid(x);
	return sig * (1 - sig);
}

// ReLU激活函数
double relu(double x) {
	return std::max(0.0, x);
}

// ReLU激活函数的导数
double relu_derivative(double x) {
	return x > 0 ? 1.0 : 0.0;
}

class MatrixXd {
	private:
		int rows;
		int cols;
		std::vector<std::vector<double >> data;
		
	public:
		// 默认构造函数
		MatrixXd() : rows(0), cols(0) {}
		
		// 构造函数，初始化矩阵的行数和列数
		MatrixXd(int r, int c) : rows(r), cols(c) {
			data.resize(rows, std::vector<double>(cols, 0.0));
		}
		
		// 析构函数
		~MatrixXd() {}
		
		// 获取矩阵的行数
		int getRows() const {
			return rows;
		}
		
		// 获取矩阵的列数
		int getCols() const {
			return cols;
		}
		
		// 访问矩阵元素
		double& operator()(int i, int j) {
			return data[i][j];
		}
		
		// 访问矩阵元素（常量版本）
		const double& operator()(int i, int j) const {
			return data[i][j];
		}
		
		// 矩阵加法
		MatrixXd operator+(const MatrixXd& other) const {
			if (rows != other.rows || cols != other.cols) {
				throw std::invalid_argument("Matrix dimensions must match for addition.");
			}
			
			MatrixXd result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result(i, j) = data[i][j] + other(i, j);
				}
			}
			
			return result;
		}
		
		// 矩阵乘法
		MatrixXd operator*(const MatrixXd& other) const {
			if (cols != other.rows) {
				throw std::invalid_argument("Number of columns in the first matrix must match the number of rows in the second matrix for multiplication.");
			}
			
			MatrixXd result(rows, other.cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < other.cols; ++j) {
					for (int k = 0; k < cols; ++k) {
						result(i, j) += data[i][k] * other(k, j);
					}
				}
			}
			
			return result;
		}
		
		// 矩阵与标量相乘
		MatrixXd operator*(double scalar) const {
			MatrixXd result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result(i, j) = data[i][j] * scalar;
				}
			}
			
			return result;
		}
		
		// 矩阵与标量相除
		MatrixXd operator/(double scalar) const {
			MatrixXd result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result(i, j) = data[i][j] / scalar;
				}
			}
			
			return result;
		}
		
		// 矩阵元素应用激活函数
		MatrixXd apply(ActivationFunction func) const {
			MatrixXd result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result(i, j) = func(data[i][j]);
				}
			}
			
			return result;
		}
		
		// 矩阵转置
		MatrixXd transpose() const {
			MatrixXd result(cols, rows);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result(j, i) = data[i][j];
				}
			}
			
			return result;
		}
		
		// 打印矩阵
		void print() const {
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					std::cout << data[i][j] << " ";
				}
				
				std::cout << std::endl;
			}
		}
		
		// 生成随机矩阵
		static MatrixXd Random(int rows, int cols) {
			MatrixXd result(rows, cols);
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(-1.0, 1.0);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result(i, j) = dis(gen);
				}
			}
			
			return result;
		}
		
		// 矩阵减法
		MatrixXd operator-(const MatrixXd& other) const {
			if (rows != other.rows || cols != other.cols) {
				throw std::invalid_argument("Matrix dimensions must match for subtraction.");
			}
			
			MatrixXd result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result(i, j) = data[i][j] - other(i, j);
				}
			}
			
			return result;
		}
		
		MatrixXd & operator-=(const MatrixXd& other) {
			if (rows != other.rows || cols != other.cols) {
				throw std::invalid_argument("Matrix dimensions must match for subtraction assignment.");
			}
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					data[i][j] -= other(i, j);
				}
			}
			
			return *this;
		}
		
		// 梯度裁剪方法
		MatrixXd clipGradient(double max_norm) const {
			double norm = 0.0;
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					norm += data[i][j] * data[i][j];
				}
			}
			
			norm = std::sqrt(norm);
			
			if (norm > max_norm) {
				double scale = max_norm / norm;
				MatrixXd result(rows, cols);
				
				for (int i = 0; i < rows; ++i) {
					for (int j = 0; j < cols; ++j) {
						result(i, j) = data[i][j] * scale;
					}
				}
				
				return result;
			}
			
			return *this;
		}
};

// 重载 double 和 MatrixXd 的乘法
MatrixXd operator*(double scalar, const MatrixXd& matrix) {
	return matrix * scalar;
}

MatrixXd Xavier(int rows, int cols) {
	MatrixXd result(rows, cols);
	std::random_device rd;
	std::mt19937 gen(rd());
	double stddev = std::sqrt(2.0 / (rows + cols));
	std::normal_distribution<> dis(0.0, stddev);
	
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result(i, j) = dis(gen);
		}
	}
	
	return result;
}
