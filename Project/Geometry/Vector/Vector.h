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
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

class Vector {
		std::vector<double> vec;
	public:
		// 空
		Vector() : vec() {}
		
		// 构造函数
		Vector(const std::vector<double>& vec) : vec(vec) {}
		
		// 析构函数
		~Vector() {}
		
		void set(const std::vector<double>& v) {
			vec = v;
		}
		
		std::vector<double> getVector() {
			return vec ;
		}
		
		// 向量加法
		Vector operator+(const Vector& other) const {
			if (vec.size() != other.vec.size()) {
				throw std::invalid_argument("Vectors must have the same dimension for addition.");
			}
			
			std::vector<double> result(vec.size());
			
			for (size_t i = 0; i < vec.size(); ++i) {
				result[i] = vec[i] + other.vec[i];
			}
			
			return Vector(result);
		}
		
		// 向量减法
		Vector operator-(const Vector& other) const {
			if (vec.size() != other.vec.size()) {
				throw std::invalid_argument("Vectors must have the same dimension for subtraction.");
			}
			
			std::vector<double> result(vec.size());
			
			for (size_t i = 0; i < vec.size(); ++i) {
				result[i] = vec[i] - other.vec[i];
			}
			
			return Vector(result);
		}
		
		// 向量点乘
		double operator*(const Vector& other) const {
			if (vec.size() != other.vec.size()) {
				throw std::invalid_argument("Vectors must have the same dimension for dot product.");
			}
			
			double dotProduct = 0.0;
			
			for (size_t i = 0; i < vec.size(); ++i) {
				dotProduct += vec[i] * other.vec[i];
			}
			
			return dotProduct;
		}
		
		// 向量叉乘（仅适用于三维向量）
		Vector cross(const Vector& other) const {
			if (vec.size() != 3 || other.vec.size() != 3) {
				throw std::invalid_argument("Cross product is only defined for 3D vectors.");
			}
			
			std::vector<double> result(3);
			result[0] = vec[1] * other.vec[2] - vec[2] * other.vec[1];
			result[1] = vec[2] * other.vec[0] - vec[0] * other.vec[2];
			result[2] = vec[0] * other.vec[1] - vec[1] * other.vec[0];
			return Vector(result);
		}
		
		// 向量数乘
		Vector operator*(double scalar) const {
			std::vector<double> result(vec.size());
			
			for (size_t i = 0; i < vec.size(); ++i) {
				result[i] = vec[i] * scalar;
			}
			
			return Vector(result);
		}
		
		// 向量数除
		Vector operator/(double scalar) const {
			if (scalar == 0.0) {
				throw std::invalid_argument("Division by zero is not allowed.");
			}
			
			std::vector<double> result(vec.size());
			
			for (size_t i = 0; i < vec.size(); ++i) {
				result[i] = vec[i] / scalar;
			}
			
			return Vector(result);
		}
		
		// 打印向量
		void print() const {
			std::cout << "(";
			
			for (size_t i = 0; i < vec.size(); ++i) {
				if (i > 0) {
					std::cout << ", ";
				}
				
				std::cout << vec[i];
			}
			
			std::cout << ")" << std::endl;
		}
		
		// 向量归一化
		Vector normalize() const {
			double magnitude = 0.0;
			
			for (double val : vec) {
				magnitude += val * val;
			}
			
			magnitude = sqrt(magnitude);
			
			if (magnitude == 0.0) {
				throw std::invalid_argument("Cannot normalize a zero vector.");
			}
			
			return *this / magnitude;
		}
};

// 数乘运算符重载（支持 scalar * vector）
Vector operator*(double scalar, const Vector& vec) {
	return vec * scalar;
}
