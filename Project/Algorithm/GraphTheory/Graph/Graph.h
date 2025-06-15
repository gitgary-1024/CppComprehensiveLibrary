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
#include <queue>
#include <limits>
#include <iostream>

// 定义边的结构体
struct Edge {
	int to;
	double weight;
	Edge(int t, double w) : to(t), weight(w) {}
};

class Graph {
	private:
		int V; // 顶点数量
		bool direction;
		std::vector<std::vector<Edge >> adj; // 邻接表
		
	public:
		// 构造函数，初始化图的顶点数量
		Graph(int vertices, bool direction = 1) : V(vertices) {
			this->direction = direction;
			adj.resize(V);
		}
		
		// 析构函数
		~Graph() {
			// 由于使用了标准库容器，不需要手动释放内存
		}
		
		// 添加边到图中
		void addEdge(int from, int to, double weight) {
			adj[from].emplace_back(to, weight);// 向顶点 v 的邻接表中添加顶点 w
			
			// 如果是无向图，还需要添加反向边
			if(!direction) {
				adj[to].push_back({from, weight});
			}
		}
		
		// 获取图的顶点数量
		int getVertexCount() const {
			return V;
		}
		
		// 求第 k 短路的距离
		double pathDistance(int k) {
			if (V <= 0 || k <= 0) return - 1; // 无效输入
			int start = 0; // 假设起点为 0
			int end = V - 1; // 假设终点为 V - 1
			// 优先队列用于存储待扩展的节点，按距离从小到大排序
			std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int >>, std::greater<std::pair<double, int >>> pq;
			std::vector<int> cnt(V, 0); // 记录每个节点被访问的次数
			pq.push({0, start});
			
			while (!pq.empty()) {
				auto [dist, u] = pq.top();
				pq.pop();
				cnt[u]++;
				
				if (u == end && cnt[u] == k) {
					return dist;
				}
				
				if (cnt[u] > k)
					continue;
					
				for (const auto& edge : adj[u]) {
					int v = edge.to;
					double newDist = dist + edge.weight;
					pq.push({newDist, v});
				}
			}
			return -1; // 没有找到第 k 短路
		}
};
