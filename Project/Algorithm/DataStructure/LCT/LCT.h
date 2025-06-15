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
class LinkCutTree {
	private:
		struct Node {
			T val; // 节点值
			T sum; // 路径和
			T lazy; // 懒标记
			Node* left;
			Node* right;
			Node* parent;
			bool reversed; // 反转标记
			int size; // 子树大小
			
			Node(T v) : val(v), sum(v), lazy(0), left(nullptr), right(nullptr),
				parent(nullptr), reversed(false), size(1) {}
				
			bool isRoot() const {
				return parent == nullptr || (parent->left != this && parent->right != this);
			}
			
			void push() {
				if (reversed) {
					std::swap(left, right);
					
					if (left)
						left->reversed = !left->reversed;
						
					if (right)
						right->reversed = !right->reversed;
						
					reversed = false;
				}
				
				if (lazy != 0) {
					if (left) {
						left->val += lazy;
						left->sum += lazy * left->size;
						left->lazy += lazy;
					}
					
					if (right) {
						right->val += lazy;
						right->sum += lazy * right->size;
						right->lazy += lazy;
					}
					
					lazy = 0;
				}
			}
			
			void update() {
				size = 1;
				sum = val;
				
				if (left) {
					size += left->size;
					sum += left->sum;
				}
				
				if (right) {
					size += right->size;
					sum += right->sum;
				}
			}
		};
		
		std::vector<Node*> nodes;
		
		// 右旋
		void rotateRight(Node* x) {
			Node* y = x->parent;
			Node* z = y->parent;
			y->push();
			x->push();
			y->left = x->right;
			
			if (x->right)
				x->right->parent = y;
				
			x->right = y;
			y->parent = x;
			x->parent = z;
			
			if (z) {
				if (z->left == y)
					z->left = x;
				else if (z->right == y)
					z->right = x;
			}
			
			y->update();
			x->update();
		}
		
		// 左旋
		void rotateLeft(Node* x) {
			Node* y = x->parent;
			Node* z = y->parent;
			y->push();
			x->push();
			y->right = x->left;
			
			if (x->left)
				x->left->parent = y;
				
			x->left = y;
			y->parent = x;
			x->parent = z;
			
			if (z) {
				if (z->left == y)
					z->left = x;
				else if (z->right == y)
					z->right = x;
			}
			
			y->update();
			x->update();
		}
		
		// Splay操作
		void splay(Node* x) {
			x->push();
			
			while (!x->isRoot()) {
				Node* y = x->parent;
				Node* z = y->parent;
				
				if (!y->isRoot()) {
					z->push();
				}
				
				y->push();
				x->push();
				
				if (!y->isRoot()) {
					if (y->left == x) {
						if (z->left == y)
							rotateRight(y);
							
						rotateRight(x);
					}
					else {
						if (z->right == y)
							rotateLeft(y);
							
						rotateLeft(x);
					}
				}
				else {
					if (y->left == x)
						rotateRight(x);
					else
						rotateLeft(x);
				}
			}
			
			x->update();
		}
		
		// 访问节点x，将x到根的路径变为Preferred Path
		void access(Node* x) {
			Node* last = nullptr;
			
			while (x) {
				splay(x);
				x->right = last;
				x->update();
				last = x;
				x = x->parent;
			}
		}
		
		// 使节点x成为其所在树的根
		void makeRoot(Node* x) {
			access(x);
			splay(x);
			x->reversed = !x->reversed;
		}
		
		// 找到节点x所在树的根，并将根节点旋转到Splay树的根
		Node* findRoot(Node* x) {
			access(x);
			splay(x);
			
			while (x->left) {
				x->push();
				x = x->left;
			}
			
			splay(x);
			return x;
		}
		
		// 连接节点x和y
		void link(Node* x, Node* y) {
			makeRoot(x);
			
			if (findRoot(y) == x)
				throw std::runtime_error("Nodes are already connected");
				
			x->parent = y;
		}
		
		// 断开节点x和y之间的边
		void cut(Node* x, Node* y) {
			makeRoot(x);
			
			if (findRoot(y) != x || y->parent != x || y->left != nullptr) {
				throw std::runtime_error("No edge between nodes");
			}
			
			y->parent = nullptr;
			x->left = nullptr;
			x->update();
		}
		
		// 判断节点x和y是否连通
		bool connected(Node* x, Node* y) {
			return findRoot(x) == findRoot(y);
		}
		
		// 路径更新
		void updatePath(Node* x, Node* y, T val) {
			makeRoot(x);
			access(y);
			splay(y);
			y->val += val;
			y->sum += val * y->size;
			y->lazy += val;
		}
		
		// 路径查询
		T queryPath(Node* x, Node* y) {
			makeRoot(x);
			access(y);
			splay(y);
			return y->sum;
		}
		
	public:
		// 构造函数
		LinkCutTree(int n) {
			nodes.resize(n);
			
			for (int i = 0; i < n; ++i) {
				nodes[i] = new Node(0);
			}
		}
		
		// 析构函数
		~LinkCutTree() {
			for (Node * node : nodes) {
				delete node;
			}
		}
		
		// 设置节点值
		void setValue(int idx, T val) {
			if (idx < 0 || idx >= nodes.size())
				throw std::out_of_range("Index out of range");
				
			access(nodes[idx]);
			splay(nodes[idx]);
			nodes[idx]->val = val;
			nodes[idx]->update();
		}
		
		// 连接两个节点
		void link(int x, int y) {
			if (x < 0 || x >= nodes.size() || y < 0 || y >= nodes.size()) {
				throw std::out_of_range("Index out of range");
			}
			
			link(nodes[x], nodes[y]);
		}
		
		// 切断两个节点之间的边
		void cut(int x, int y) {
			if (x < 0 || x >= nodes.size() || y < 0 || y >= nodes.size()) {
				throw std::out_of_range("Index out of range");
			}
			
			cut(nodes[x], nodes[y]);
		}
		
		// 判断两个节点是否连通
		bool connected(int x, int y) {
			if (x < 0 || x >= nodes.size() || y < 0 || y >= nodes.size()) {
				throw std::out_of_range("Index out of range");
			}
			
			return connected(nodes[x], nodes[y]);
		}
		
		// 更新路径上的所有节点值
		void updatePath(int x, int y, T val) {
			if (x < 0 || x >= nodes.size() || y < 0 || y >= nodes.size()) {
				throw std::out_of_range("Index out of range");
			}
			
			updatePath(nodes[x], nodes[y], val);
		}
		
		// 查询路径上的节点值之和
		T queryPath(int x, int y) {
			if (x < 0 || x >= nodes.size() || y < 0 || y >= nodes.size()) {
				throw std::out_of_range("Index out of range");
			}
			
			return queryPath(nodes[x], nodes[y]);
		}
};

