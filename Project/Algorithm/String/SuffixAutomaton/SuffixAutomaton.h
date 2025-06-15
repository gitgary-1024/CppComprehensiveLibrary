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
#include <unordered_map>
#include <memory>

class SuffixAutomaton {
	private:
		struct State {
			int len;
			int link;
			std::unordered_map<char, int> next;
			
			State(int l = 0, int ln = -1) : len(l), link(ln) {}
		};
		
		std::vector<State> states;
		int size;
		int last;
		
	public:
		SuffixAutomaton() {
			states.push_back(State());
			size = 1;
			last = 0;
		}
		
		void extend(char c) {
			int cur = size++;
			states.push_back(State(states[last].len + 1));
			int p = last;
			
			while (p != -1 && !states[p].next.count(c)) {
				states[p].next[c] = cur;
				p = states[p].link;
			}
			
			if (p == -1) {
				states[cur].link = 0;
			}
			else {
				int q = states[p].next[c];
				
				if (states[p].len + 1 == states[q].len) {
					states[cur].link = q;
				}
				else {
					int clone = size++;
					states.push_back(State(states[p].len + 1, states[q].link));
					states[clone].next = states[q].next;
					
					while (p != -1 && states[p].next[c] == q) {
						states[p].next[c] = clone;
						p = states[p].link;
					}
					
					states[q].link = states[cur].link = clone;
				}
			}
			
			last = cur;
		}
		
		void build(const std::string& s) {
			for (char c : s) {
				extend(c);
			}
		}
		
		bool contains(const std::string& pattern) {
			int current = 0;
			
			for (char c : pattern) {
				if (!states[current].next.count(c)) {
					return false;
				}
				
				current = states[current].next[c];
			}
			
			return true;
		}
		
		int longestCommonSubstring(const std::string& s) {
			int v = 0, l = 0, best = 0, bestpos = 0;
			
			for (size_t i = 0; i < s.size(); i++) {
				while (v && !states[v].next.count(s[i])) {
					v = states[v].link;
					l = states[v].len;
				}
				
				if (states[v].next.count(s[i])) {
					v = states[v].next[s[i]];
					l++;
				}
				
				if (l > best) {
					best = l;
					bestpos = i;
				}
			}
			
			return best;
		}
};
