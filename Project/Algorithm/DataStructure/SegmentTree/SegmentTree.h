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
#include <vector>
#include <functional>
#include <stdexcept>

template <typename T>
class SegmentTree {
	private:
		std::vector<T> tree; // 存储线段树的数组
		std::vector<T> lazy; // 懒标记数组，用于区间更新
		std::vector<T> data; // 原始数据的副本
		int n; // 原始数据的大小
		std::function<T(T, T)> op; // 区间合并操作（如加法、最大值、最小值等）
		T init_val; // 初始值（用于空区间或叶子节点）
		
		// 构建线段树
		void build(int node, int start, int end) {
			if (start == end) {
				tree[node] = data[start];
			}
			else {
				int mid = (start + end) / 2;
				build(2 * node + 1, start, mid);
				build(2 * node + 2, mid + 1, end);
				tree[node] = op(tree[2 * node + 1], tree[2 * node + 2]);
			}
		}
		
		// 下推懒标记
		void push(int node, int start, int end) {
			if (lazy[node] != 0) {
				int mid = (start + end) / 2;
				// 更新左子树
				tree[2 * node + 1] += lazy[node] * (mid - start + 1);
				lazy[2 * node + 1] += lazy[node];
				// 更新右子树
				tree[2 * node + 2] += lazy[node] * (end - mid);
				lazy[2 * node + 2] += lazy[node];
				// 清除当前节点的懒标记
				lazy[node] = 0;
			}
		}
		
		// 区间更新辅助函数
		void update_range(int node, int start, int end, int l, int r, T val) {
			if (start > end || start > r || end < l)
				return;
				
			if (start >= l && end <= r) {
				// 当前区间完全被更新区间覆盖
				tree[node] += val * (end - start + 1);
				
				if (start != end) {
					lazy[node] += val;
				}
				
				return;
			}
			
			// 下推懒标记
			push(node, start, end);
			// 递归更新左右子树
			int mid = (start + end) / 2;
			update_range(2 * node + 1, start, mid, l, r, val);
			update_range(2 * node + 2, mid + 1, end, l, r, val);
			// 更新当前节点的值
			tree[node] = op(tree[2 * node + 1], tree[2 * node + 2]);
		}
		
		// 区间查询辅助函数
		T query_range(int node, int start, int end, int l, int r) {
			if (start > end || start > r || end < l)
				return init_val;
				
			if (start >= l && end <= r)
				return tree[node];
				
			// 下推懒标记
			push(node, start, end);
			// 递归查询左右子树
			int mid = (start + end) / 2;
			T left = query_range(2 * node + 1, start, mid, l, r);
			T right = query_range(2 * node + 2, mid + 1, end, l, r);
			return op(left, right);
		}
		
	public:
		// 构造函数
		SegmentTree(const std::vector<T>& arr,
		            const std::function<T(T, T)>& operation,
		            T initial_value = T())
			: data(arr), n(arr.size()), op(operation), init_val(initial_value) {
			if (n == 0)
				throw std::invalid_argument("Array cannot be empty");
				
			// 计算线段树的大小
			int size = 1;
			
			while (size < n)
				size <<= 1;
				
			size = 2 * size - 1;
			// 初始化线段树和懒标记数组
			tree.resize(size, init_val);
			lazy.resize(size, 0);
			// 构建线段树
			build(0, 0, n - 1);
		}
		
		// 单点更新
		void update_point(int idx, T val) {
			if (idx < 0 || idx >= n)
				throw std::out_of_range("Index out of range");
				
			data[idx] = val;
			update_range(0, 0, n - 1, idx, idx, val);
		}
		
		// 区间更新
		void update_range(int l, int r, T val) {
			if (l < 0 || r >= n || l > r)
				throw std::out_of_range("Invalid range");
				
			update_range(0, 0, n - 1, l, r, val);
		}
		
		// 区间查询
		T query_range(int l, int r) {
			if (l < 0 || r >= n || l > r)
				throw std::out_of_range("Invalid range");
				
			return query_range(0, 0, n - 1, l, r);
		}
};
