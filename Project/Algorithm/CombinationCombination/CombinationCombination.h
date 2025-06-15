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

#include"../NumberTheory/BigInt/bigint.hpp"

BigInt factorial(BigInt num, BigInt mod) {
	BigInt ans("1");
	
	for (BigInt i = 2; i <= num; i++) {
		ans *= i;
		ans %= mod;
	}
	
	return ans;
}

BigInt C(BigInt a, BigInt b, BigInt mod) {
	return factorial(a, mod) / (factorial(b, mod) * factorial(a - b, mod));
}

BigInt A(BigInt a, BigInt b, BigInt mod) {
	return factorial(a, mod) / factorial(b - a, mod);
}

BigInt Lucas(BigInt n, BigInt k, BigInt p) {
	if (k == 0)
		return 1;
		
	return (C(n % p, k % p, p) * Lucas(n / p, k / p, p)) % p;
}
