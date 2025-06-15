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

#include<cstring>
#include<string>

struct trie {
	int nex[100000][26], cnt;
	bool exist[100000]; // 该结点结尾的字符串是否存在
	
	void insert(char *s, int l) { // 插入字符串
		int p = 0;
		
		for (int i = 0; i < l; i++) {
			int c = s[i] - 'a';
			
			if (!nex[p][c])
				nex[p][c] = ++cnt; // 如果没有，就添加结点
				
			p = nex[p][c];
		}
		
		exist[p] = true;
	}
	
	void insert(const std::string& s) { // 插入字符串
		int p = 0;
		int l = s.size();
		
		for (int i = 0; i < l; i++) {
			int c = s[i] - 'a';
			
			if (!nex[p][c])
				nex[p][c] = ++cnt; // 如果没有，就添加结点
				
			p = nex[p][c];
		}
		
		exist[p] = true;
	}
	
	bool find(char *s, int l) { // 查找字符串
		int p = 0;
		
		for (int i = 0; i < l; i++) {
			int c = s[i] - 'a';
			
			if (!nex[p][c])
				return 0;
				
			p = nex[p][c];
		}
		
		return exist[p];
	}
};
