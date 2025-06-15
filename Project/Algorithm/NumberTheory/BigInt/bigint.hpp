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
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

class BigInt {
	private:
		bool isNegative; // 符号位，true表示负数
		std::vector<int> digits; // 存储每一位数字，低位在前
		
		// 去除前导零
		void trimLeadingZeros();
		
	public:
		// 构造函数
		BigInt();
		BigInt(long long num);
		BigInt(const std::string& numStr);
		
		// 拷贝构造和赋值运算符
		BigInt(const BigInt& other);
		BigInt& operator=(const BigInt& other);
		
		// 移动构造和移动赋值
		BigInt(BigInt&& other) noexcept;
		BigInt& operator=(BigInt&& other) noexcept;
		
		// 析构函数
		~BigInt();
		
		// 比较运算符
		bool operator==(const BigInt& other) const;
		bool operator!=(const BigInt& other) const;
		bool operator<(const BigInt& other) const;
		bool operator>(const BigInt& other) const;
		bool operator<=(const BigInt& other) const;
		bool operator>=(const BigInt& other) const;
		
		// 算术运算符
		BigInt operator+(const BigInt& other) const;
		BigInt operator-(const BigInt& other) const;
		BigInt operator*(const BigInt& other) const;
		BigInt operator/(const BigInt& other) const;
		BigInt operator%(const BigInt& other) const;
		
		// 复合赋值运算符
		BigInt& operator+=(const BigInt& other);
		BigInt& operator-=(const BigInt& other);
		BigInt& operator*=(const BigInt& other);
		BigInt& operator/=(const BigInt& other);
		BigInt& operator%=(const BigInt& other);
		
		// 自增自减运算符
		BigInt& operator++(); // 前置++
		BigInt operator++(int); // 后置++
		BigInt& operator--(); // 前置--
		BigInt operator--(int); // 后置--
		
		// 正负号运算符
		BigInt operator+() const; // 正号
		BigInt operator-() const; // 负号
		
		// 输入输出流
		friend std::ostream& operator<<(std::ostream& os, const BigInt& num);
		friend std::istream& operator>>(std::istream& is, BigInt& num);
		
		// 其他方法
		bool isZero() const;
		std::string toString() const;
};

BigInt abs(BigInt num) {
	return num < 0 ? -num : num;
}
