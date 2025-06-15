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
#include <bits/stdc++.h>
using namespace std;

template <typename T>
class SparseTable {
		using VT = vector<T>;
		using VVT = vector<VT>;
		using func_type = function<T(const T &, const T &)>;
		
		VVT ST;
		
		static T default_func(const T &t1, const T &t2) {
			return max(t1, t2);
		}
		
		func_type op;
		
	public:
		SparseTable(const vector<T> &v, func_type _func = default_func) {
			op = _func;
			int len = v.size(), l1 = ceil(log2(len)) + 1;
			ST.assign(len, VT(l1, 0));
			
			for (int i = 0; i < len; ++i) {
				ST[i][0] = v[i];
			}
			
			for (int j = 1; j < l1; ++j) {
				int pj = (1 << (j - 1));
				
				for (int i = 0; i + pj < len; ++i) {
					ST[i][j] = op(ST[i][j - 1], ST[i + (1 << (j - 1))][j - 1]);
				}
			}
		}
		
		T query(int l, int r) {
			int lt = r - l + 1;
			int q = floor(log2(lt));
			return op(ST[l][q], ST[r - (1 << q) + 1][q]);
		}
};
