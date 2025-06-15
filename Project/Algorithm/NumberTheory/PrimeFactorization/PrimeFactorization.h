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
#include"../BigInt/bigint.hpp"
#include"../GCD/GCD.h"

BigInt f(BigInt a, BigInt b, BigInt c) {
	return (a * a + b) % c;
}

BigInt Pollard_Rho(BigInt N) {
	if (N == 4)
		return 2; // 因为一开始跳了两步，所以需要特判一下 4
		
	BigInt c = BigInt(rand()) % (N - 1) + 1;
	BigInt t = f(0, c, N);
	BigInt r = f(f(0, c, N), c, N);
	
	while (t != r) {
		BigInt d = gcd(abs(t - r), N);
		
		if (d > 1)
			return d;
			
		t = f(t, c, N);
		r = f(f(r, c, N), c, N);
	}
	
	return N;
}
