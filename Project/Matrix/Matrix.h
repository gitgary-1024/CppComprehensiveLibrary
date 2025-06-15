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

#include <iostream>
#include <vector>
#include <cmath>

class Matrix {
	private:
		std::vector<std::vector<double >> data;
		int rows;
		int cols;
		
	public:
		// 默认构造函数
		Matrix() : rows(0), cols(0) {}
		
		// 构造函数，初始化矩阵的行数和列数
		Matrix(int r, int c) : rows(r), cols(c) {
			data.resize(rows, std::vector<double>(cols, 0.0));
		}
		
		// 从向量数组构造矩阵的构造函数
		Matrix(const std::vector<std::vector<double >> & inputData) {
			rows = inputData.size();
			if (rows > 0) {
				cols = inputData[0].size();
				
				for (const auto& row : inputData) {
					if (static_cast<int>(row.size()) != cols) {
						throw std::invalid_argument("All rows must have the same number of columns.");
					}
				}
				
				data = inputData;
			}
			else {
				rows = 0;
				cols = 0;
			}
		}
		
		// 析构函数
		~Matrix() {}
		
		// 设置矩阵元素的值
		void set(int i, int j, double value) {
			if (i >= 0 && i < rows && j >= 0 && j < cols) {
				data[i][j] = value;
			}
		}
		
		// 获取矩阵元素的值
		double get(int i, int j) const {
			if (i >= 0 && i < rows && j >= 0 && j < cols) {
				return data[i][j];
			}
			
			return 0.0;
		}
		
		// 获取矩阵的行数
		int getRows() const {
			return rows;
		}
		
		// 获取矩阵的列数
		int getCols() const {
			return cols;
		}
		
		// 矩阵加法
		Matrix operator+(const Matrix& other) const {
			if (rows != other.rows || cols != other.cols) {
				throw std::invalid_argument("Matrix dimensions must match for addition.");
			}
			
			Matrix result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result.data[i][j] = data[i][j] + other.data[i][j];
				}
			}
			
			return result;
		}
		
		// 矩阵减法
		Matrix operator-(const Matrix& other) const {
			if (rows != other.rows || cols != other.cols) {
				throw std::invalid_argument("Matrix dimensions must match for subtraction.");
			}
			
			Matrix result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result.data[i][j] = data[i][j] - other.data[i][j];
				}
			}
			
			return result;
		}
		
		// 矩阵乘法
		Matrix operator*(const Matrix& other) const {
			if (cols != other.rows) {
				throw std::invalid_argument("Number of columns in the first matrix must match the number of rows in the second matrix for multiplication.");
			}
			
			Matrix result(rows, other.cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < other.cols; ++j) {
					for (int k = 0; k < cols; ++k) {
						result.data[i][j] += data[i][k] * other.data[k][j];
					}
				}
			}
			
			return result;
		}
		
		// 矩阵数乘
		Matrix operator*(double scalar) const {
			Matrix result(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result.data[i][j] = data[i][j] * scalar;
				}
			}
			
			return result;
		}
		
		// 矩阵转置
		Matrix transpose() const {
			Matrix result(cols, rows);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					result.data[j][i] = data[i][j];
				}
			}
			
			return result;
		}
		
		// 计算行列式（仅适用于方阵）
		double determinant() const {
			if (rows != cols) {
				throw std::invalid_argument("Determinant can only be calculated for square matrices.");
			}
			
			if (rows == 1) {
				return data[0][0];
			}
			
			if (rows == 2) {
				return data[0][0] * data[1][1] - data[0][1] * data[1][0];
			}
			
			double det = 0.0;
			
			for (int j = 0; j < cols; ++j) {
				Matrix submatrix(rows - 1, cols - 1);
				
				for (int i = 1; i < rows; ++i) {
					for (int k = 0; k < cols; ++k) {
						if (k < j) {
							submatrix.data[i - 1][k] = data[i][k];
						}
						else if (k > j) {
							submatrix.data[i - 1][k - 1] = data[i][k];
						}
					}
				}
				
				det += std::pow(-1, j) * data[0][j] * submatrix.determinant();
			}
			
			return det;
		}
		
		// 计算伴随矩阵（仅适用于方阵）
		Matrix adjugate() const {
			if (rows != cols) {
				throw std::invalid_argument("Adjugate can only be calculated for square matrices.");
			}
			
			Matrix adj(rows, cols);
			
			for (int i = 0; i < rows; ++i) {
				for (int j = 0; j < cols; ++j) {
					Matrix submatrix(rows - 1, cols - 1);
					
					for (int k = 0; k < rows; ++k) {
						for (int l = 0; l < cols; ++l) {
							if (k < i) {
								if (l < j) {
									submatrix.data[k][l] = data[k][l];
								}
								else if (l > j) {
									submatrix.data[k][l - 1] = data[k][l];
								}
							}
							else if (k > i) {
								if (l < j) {
									submatrix.data[k - 1][l] = data[k][l];
								}
								else if (l > j) {
									submatrix.data[k - 1][l - 1] = data[k][l];
								}
							}
						}
					}
					
					adj.data[i][j] = std::pow(-1, i + j) * submatrix.determinant();
				}
			}
			
			return adj.transpose();
		}
		
		// 计算逆矩阵（仅适用于可逆方阵）
		Matrix inverse() const {
			double det = determinant();
			
			if (std::abs(det) < 1e-10) {
				throw std::invalid_argument("Matrix is not invertible.");
			}
			
			return adjugate() * (1.0 / det);
		}
		
		// 矩阵除法（乘以逆矩阵）
		Matrix operator/(const Matrix& other) const {
			return (*this) * other.inverse();
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
};
