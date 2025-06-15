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

BigInt euler_phi(BigInt n) {
	BigInt ans = n;
	
	for (BigInt i = 2; i * i <= n; i++)
		if (n % i == 0) {
			ans = ans / i * (i - 1);
			
			while (n % i == 0)
				n /= i;
		}
		
	if (n > 1)
		ans = ans / n * (n - 1);
		
	return ans;
}

// 线性筛法求欧拉函数
std::vector<int> linearEulerSieve(int n) {
	std::vector<int> phi(n + 1);
	std::vector<bool> isPrime(n + 1, true);
	std::vector<int> primes;
	phi[1] = 1;
	
	for (int i = 2; i <= n; ++i) {
		if (isPrime[i]) {
			primes.push_back(i);
			phi[i] = i - 1;
		}
		
		for (size_t j = 0; j < primes.size() && i * primes[j] <= n; ++j) {
			isPrime[i * primes[j]] = false;
			
			if (i % primes[j] == 0) {
				phi[i * primes[j]] = phi[i] * primes[j];
				break;
			}
			else {
				phi[i * primes[j]] = phi[i] * (primes[j] - 1);
			}
		}
	}
	
	return phi;
}
