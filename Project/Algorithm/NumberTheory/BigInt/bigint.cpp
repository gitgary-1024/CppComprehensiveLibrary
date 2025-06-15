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
#include "bigint.hpp"
#include <sstream>

// 去除前导零
void BigInt::trimLeadingZeros() {
	while (digits.size() > 1 && digits.back() == 0) {
		digits.pop_back();
	}
	
	if (digits.size() == 1 && digits[0] == 0) {
		isNegative = false;
	}
}

// 构造函数
BigInt::BigInt() : isNegative(false), digits(1, 0) {}

BigInt::BigInt(long long num) {
	if (num == 0) {
		isNegative = false;
		digits = {0};
		return;
	}
	
	if (num < 0) {
		isNegative = true;
		num = -num;
	}
	else {
		isNegative = false;
	}
	
	while (num > 0) {
		digits.push_back(num % 10);
		num /= 10;
	}
}

BigInt::BigInt(const std::string& numStr) {
	if (numStr.empty()) {
		throw std::invalid_argument("Invalid number string");
	}
	
	size_t i = 0;
	isNegative = false;
	
	if (numStr[0] == '-') {
		isNegative = true;
		i = 1;
	}
	else if (numStr[0] == '+') {
		i = 1;
	}
	
	// 跳过前导零
	while (i < numStr.size() && numStr[i] == '0') {
		i++;
	}
	
	if (i == numStr.size()) {
		// 全零
		isNegative = false;
		digits = {0};
		return;
	}
	
	// 存储每一位数字，低位在前
	for (size_t j = numStr.size(); j > i; j--) {
		if (!isdigit(numStr[j - 1])) {
			throw std::invalid_argument("Invalid character in number string");
		}
		
		digits.push_back(numStr[j - 1] - '0');
	}
}

// 拷贝构造
BigInt::BigInt(const BigInt& other) : isNegative(other.isNegative), digits(other.digits) {}

// 赋值运算符
BigInt& BigInt::operator=(const BigInt& other) {
	if (this != &other) {
		isNegative = other.isNegative;
		digits = other.digits;
	}
	
	return *this;
}

// 移动构造
BigInt::BigInt(BigInt&& other) noexcept : isNegative(other.isNegative), digits(std::move(other.digits)) {}

// 移动赋值
BigInt& BigInt::operator=(BigInt&& other) noexcept {
	if (this != &other) {
		isNegative = other.isNegative;
		digits = std::move(other.digits);
	}
	
	return *this;
}

// 析构函数
BigInt::~BigInt() {}

// 比较运算符
bool BigInt::operator==(const BigInt& other) const {
	if (isNegative != other.isNegative) {
		return false;
	}
	
	if (digits.size() != other.digits.size()) {
		return false;
	}
	
	for (size_t i = 0; i < digits.size(); i++) {
		if (digits[i] != other.digits[i]) {
			return false;
		}
	}
	
	return true;
}

bool BigInt::operator!=(const BigInt& other) const {
	return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const {
	if (isNegative != other.isNegative) {
		return isNegative;
	}
	
	if (digits.size() != other.digits.size()) {
		return (isNegative ? digits.size() > other.digits.size() : digits.size() < other.digits.size());
	}
	
	for (int i = digits.size() - 1; i >= 0; i--) {
		if (digits[i] != other.digits[i]) {
			return (isNegative ? digits[i] > other.digits[i] : digits[i] < other.digits[i]);
		}
	}
	
	return false;
}

bool BigInt::operator>(const BigInt& other) const {
	return other < *this;
}

bool BigInt::operator<=(const BigInt& other) const {
	return !(other < *this);
}

bool BigInt::operator>=(const BigInt& other) const {
	return !(*this < other);
}

// 加法运算符
BigInt BigInt::operator+(const BigInt& other) const {
	if (isNegative == other.isNegative) {
		// 符号相同，绝对值相加，符号不变
		BigInt result;
		result.isNegative = isNegative;
		size_t maxSize = std::max(digits.size(), other.digits.size());
		result.digits.resize(maxSize);
		int carry = 0;
		
		for (size_t i = 0; i < maxSize || carry != 0; i++) {
			if (i == result.digits.size()) {
				result.digits.push_back(0);
			}
			
			int sum = carry;
			sum += (i < digits.size() ? digits[i] : 0);
			sum += (i < other.digits.size() ? other.digits[i] : 0);
			result.digits[i] = sum % 10;
			carry = sum / 10;
		}
		
		result.trimLeadingZeros();
		return result;
	}
	else {
		// 符号不同，绝对值相减，符号取绝对值较大的数的符号
		const BigInt* absThis = this;
		const BigInt* absOther = &other;
		// 比较绝对值大小
		bool thisBigger = false;
		
		if (digits.size() != other.digits.size()) {
			thisBigger = digits.size() > other.digits.size();
		}
		else {
			for (int i = digits.size() - 1; i >= 0; i--) {
				if (digits[i] != other.digits[i]) {
					thisBigger = digits[i] > other.digits[i];
					break;
				}
			}
		}
		
		if (!thisBigger) {
			std::swap(absThis, absOther);
		}
		
		BigInt result;
		result.isNegative = (absThis == this) ? isNegative : other.isNegative;
		size_t maxSize = absThis->digits.size();
		result.digits.resize(maxSize);
		int borrow = 0;
		
		for (size_t i = 0; i < maxSize; i++) {
			int diff = (i < absThis->digits.size() ? absThis->digits[i] : 0)
			           - (i < absOther->digits.size() ? absOther->digits[i] : 0)
			           - borrow;
			           
			if (diff < 0) {
				diff += 10;
				borrow = 1;
			}
			else {
				borrow = 0;
			}
			
			result.digits[i] = diff;
		}
		
		result.trimLeadingZeros();
		return result;
	}
}

// 减法运算符
BigInt BigInt::operator-(const BigInt& other) const {
	// a - b 等价于 a + (-b)
	BigInt negOther = other;
	negOther.isNegative = !other.isNegative;
	return *this + negOther;
}

// 乘法运算符
BigInt BigInt::operator*(const BigInt& other) const {
	if (isZero() || other.isZero()) {
		return BigInt(0);
	}
	
	BigInt result;
	result.isNegative = isNegative ^ other.isNegative;
	result.digits.resize(digits.size() + other.digits.size(), 0);
	
	for (size_t i = 0; i < digits.size(); i++) {
		int carry = 0;
		
		for (size_t j = 0; j < other.digits.size() || carry != 0; j++) {
			long long cur = result.digits[i + j] +
			                (long long)digits[i] * (j < other.digits.size() ? other.digits[j] : 0) +
			                carry;
			result.digits[i + j] = cur % 10;
			carry = cur / 10;
		}
	}
	
	result.trimLeadingZeros();
	return result;
}

// 除法运算符
BigInt BigInt::operator/(const BigInt& other) const {
	if (other.isZero()) {
		throw std::runtime_error("Division by zero");
	}
	
	if (isZero()) {
		return BigInt(0);
	}
	
	BigInt dividend = *this;
	BigInt divisor = other;
	dividend.isNegative = false;
	divisor.isNegative = false;
	
	if (dividend < divisor) {
		return BigInt(0);
	}
	
	BigInt quotient;
	quotient.isNegative = isNegative ^ other.isNegative;
	BigInt current;
	current.digits.clear();
	
	for (int i = dividend.digits.size() - 1; i >= 0; i--) {
		// 左移一位并加上当前位
		current.digits.insert(current.digits.begin(), dividend.digits[i]);
		current.trimLeadingZeros();
		// 计算当前部分商
		int x = 0;
		int left = 0, right = 9;
		
		while (left <= right) {
			int mid = (left + right) / 2;
			BigInt tmp = divisor * BigInt(mid);
			
			if (tmp <= current) {
				x = mid;
				left = mid + 1;
			}
			else {
				right = mid - 1;
			}
		}
		
		quotient.digits.insert(quotient.digits.begin(), x);
		// 减去已经计算的部分
		current = current - (divisor * BigInt(x));
	}
	
	quotient.trimLeadingZeros();
	return quotient;
}

// 取模运算符
BigInt BigInt::operator%(const BigInt& other) const {
	if (other.isZero()) {
		throw std::runtime_error("Modulo by zero");
	}
	
	if (isZero()) {
		return BigInt(0);
	}
	
	BigInt dividend = *this;
	BigInt divisor = other;
	dividend.isNegative = false;
	divisor.isNegative = false;
	
	if (dividend < divisor) {
		return *this;
	}
	
	BigInt current;
	current.digits.clear();
	
	for (int i = dividend.digits.size() - 1; i >= 0; i--) {
		// 左移一位并加上当前位
		current.digits.insert(current.digits.begin(), dividend.digits[i]);
		current.trimLeadingZeros();
		// 计算当前部分商
		int x = 0;
		int left = 0, right = 9;
		
		while (left <= right) {
			int mid = (left + right) / 2;
			BigInt tmp = divisor * BigInt(mid);
			
			if (tmp <= current) {
				x = mid;
				left = mid + 1;
			}
			else {
				right = mid - 1;
			}
		}
		
		// 减去已经计算的部分
		current = current - (divisor * BigInt(x));
	}
	
	current.isNegative = isNegative;
	current.trimLeadingZeros();
	return current;
}

// 复合赋值运算符
BigInt& BigInt::operator+=(const BigInt& other) {
	*this = *this + other;
	return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) {
	*this = *this - other;
	return *this;
}

BigInt& BigInt::operator*=(const BigInt& other) {
	*this = *this * other;
	return *this;
}

BigInt& BigInt::operator/=(const BigInt& other) {
	*this = *this / other;
	return *this;
}

BigInt& BigInt::operator%=(const BigInt& other) {
	*this = *this % other;
	return *this;
}

// 自增运算符
BigInt& BigInt::operator++() {
	*this += BigInt(1);
	return *this;
}

BigInt BigInt::operator++(int) {
	BigInt temp = *this;
	++(*this);
	return temp;
}

// 自减运算符
BigInt& BigInt::operator--() {
	*this -= BigInt(1);
	return *this;
}

BigInt BigInt::operator--(int) {
	BigInt temp = *this;
	--(*this);
	return temp;
}

// 正负号运算符
BigInt BigInt::operator+() const {
	return *this;
}

BigInt BigInt::operator-() const {
	BigInt result = *this;
	
	if (!isZero()) {
		result.isNegative = !isNegative;
	}
	
	return result;
}

// 输出流
std::ostream& operator<<(std::ostream& os, const BigInt& num) {
	if (num.isNegative) {
		os << '-';
	}
	
	for (int i = num.digits.size() - 1; i >= 0; i--) {
		os << num.digits[i];
	}
	
	return os;
}

// 输入流
std::istream& operator>>(std::istream& is, BigInt& num) {
	std::string s;
	is >> s;
	
	try {
		num = BigInt(s);
	}
	catch (const std::invalid_argument& e) {
		is.setstate(std::ios::failbit);
	}
	
	return is;
}

// 判断是否为零
bool BigInt::isZero() const {
	return digits.size() == 1 && digits[0] == 0;
}

// 转换为字符串
std::string BigInt::toString() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}
