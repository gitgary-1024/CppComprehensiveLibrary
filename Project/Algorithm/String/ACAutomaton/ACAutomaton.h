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

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iostream>

class ACAutomaton {
	private:
		struct TrieNode {
			std::unordered_map<char, TrieNode*> children;
			TrieNode* fail;
			int patternIndex;
			
			TrieNode() : fail(nullptr), patternIndex(-1) {}
		};
		
		TrieNode* root;
		std::vector<std::string> patterns;
		
		void destroy(TrieNode* node) {
			if (!node)
				return;
				
			for (auto& child : node->children) {
				destroy(child.second);
			}
			
			delete node;
		}
		
	public:
		ACAutomaton() {
			root = new TrieNode();
		}
		
		~ACAutomaton() {
			destroy(root);
		}
		
		void insert(const std::string& pattern) {
			TrieNode* current = root;
			
			for (char c : pattern) {
				if (!current->children.count(c)) {
					current->children[c] = new TrieNode();
				}
				
				current = current->children[c];
			}
			
			patterns.push_back(pattern);
			current->patternIndex = patterns.size() - 1;
		}
		
		void buildFailurePointer() {
			std::queue<TrieNode*> q;
			root->fail = nullptr;
			q.push(root);
			
			while (!q.empty()) {
				TrieNode* current = q.front();
				q.pop();
				
				for (auto& child : current->children) {
					char c = child.first;
					TrieNode* node = child.second;
					
					if (current == root) {
						node->fail = root;
					}
					else {
						TrieNode* temp = current->fail;
						
						while (temp != nullptr) {
							if (temp->children.count(c)) {
								node->fail = temp->children[c];
								break;
							}
							
							temp = temp->fail;
						}
						
						if (temp == nullptr) {
							node->fail = root;
						}
					}
					
					q.push(node);
				}
			}
		}
		
		std::vector<std::pair<int, std::string >> query(const std::string& text) {
			std::vector<std::pair<int, std::string >> results;
			TrieNode* current = root;
			
			for (size_t i = 0; i < text.size(); ++i) {
				char c = text[i];
				
				while (current != root && !current->children.count(c)) {
					current = current->fail;
				}
				
				if (current->children.count(c)) {
					current = current->children[c];
				}
				
				TrieNode* temp = current;
				
				while (temp != root) {
					if (temp->patternIndex != -1) {
						results.push_back({i - patterns[temp->patternIndex].size() + 1,
						                   patterns[temp->patternIndex]});
					}
					
					temp = temp->fail;
				}
			}
			return results;
		}
};
