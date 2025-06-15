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
#include <stdexcept>

class UnionFind {
	private:
		std::vector<int> parent; // 存储每个节点的父节点
		std::vector<int> rank; // 存储每个节点的秩（树的高度）
		int count; // 连通分量的数量
		
	public:
		// 构造函数，初始化并查集
		explicit UnionFind(int n) : count(n) {
			if (n < 0)
				throw std::invalid_argument("Number of elements must be non-negative");
				
			parent.resize(n);
			rank.resize(n);
			
			for (int i = 0; i < n; ++i) {
				parent[i] = i; // 初始时每个节点的父节点是它自己
				rank[i] = 1; // 初始时每个节点的秩为1
			}
		}
		
		// 查找节点x所属集合的根节点，并进行路径压缩
		int find(int x) {
			if (x < 0 || x >= parent.size())
				throw std::out_of_range("Index out of range");
				
			// 路径压缩：直接将节点x连接到根节点
			if (parent[x] != x) {
				parent[x] = find(parent[x]);
			}
			
			return parent[x];
		}
		
		// 合并节点x和y所属的集合
		void unite(int x, int y) {
			int rootX = find(x);
			int rootY = find(y);
			
			if (rootX == rootY)
				return; // 已经在同一个集合中
				
			// 按秩合并：将秩较小的树合并到秩较大的树
			if (rank[rootX] < rank[rootY]) {
				parent[rootX] = rootY;
			}
			else if (rank[rootX] > rank[rootY]) {
				parent[rootY] = rootX;
			}
			else {
				parent[rootY] = rootX;
				rank[rootX]++; // 秩相同，合并后秩加1
			}
			
			count--; // 连通分量数量减1
		}
		
		// 判断节点x和y是否属于同一个集合
		bool connected(int x, int y) {
			return find(x) == find(y);
		}
		
		// 返回连通分量的数量
		int getCount() const {
			return count;
		}
};
