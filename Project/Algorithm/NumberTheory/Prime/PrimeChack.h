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
#include"../FastExponent/FastExponent.h"
#include"../BigInt/bigint.hpp"

// Miller–Rabin素数测试
bool millerRabin(BigInt num) {
	if (num <= BigInt(1))
		return false;
		
	if (num == BigInt(2) || num == BigInt(3))
		return true;
		
	if (num % BigInt(2) == BigInt(0))
		return false;
		
	// 将 num - 1 表示为 2^r * d 的形式
	BigInt d = num - BigInt(1);
	int r = 0;
	
	while (d % BigInt(2) == BigInt(0)) {
		d = d / BigInt(2);
		r++;
	}
	
	// 进行多次测试
	const int k = 5; // 测试次数，可以根据需要调整
	
	for (int i = 0; i < k; i++) {
		BigInt a = BigInt(BigInt(std::rand()) % (num - BigInt(3)) + 2); // 随机选择一个底数 a
		BigInt x = fastPowMod(a, d, num);
		
		if (x == BigInt(1) || x == num - BigInt(1))
			continue;
			
		bool isComposite = true;
		
		for (int j = 0; j < r - 1; j++) {
			x = (x * x) % num;
			
			if (x == num - BigInt(1)) {
				isComposite = false;
				break;
			}
		}
		
		if (isComposite)
			return false;
	}
	
	return true;
}
