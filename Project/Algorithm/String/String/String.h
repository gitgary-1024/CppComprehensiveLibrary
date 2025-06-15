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

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include <thread>
#include <array>
#include <atomic>
#include <stdexcept>

class String {
	private:
		// 小字符串优化（SSO）：短字符串直接存储在栈上
		union Storage {
			char small_buff[36]; // 栈上缓冲区大小，可根据需要调整
			struct {
				char* data;
				size_t capacity;
			} large; // 大字符串使用堆分配
		};
		
		Storage storage;
		size_t length;
		bool is_small;
		mutable std::atomic<bool> buff_dirty{false};
		char buff[1025];
		
		// 缓冲写
		void write_buff(const std::string& s) const {
			char* non_const_buff = const_cast<char*>(buff);
			std::strncpy(non_const_buff, s.c_str(), sizeof(buff) - 1);
			non_const_buff[sizeof(buff) - 1] = '\0';
		}
		
		// 确保缓冲区是最新的
		void ensure_buff_updated() const {
			if (buff_dirty) {
				write_buff(to_std_string());
				buff_dirty = false;
			}
		}
		
		// 释放大字符串的内存
		void free_large_storage() {
			if (!is_small) {
				delete[] storage.large.data;
			}
		}
		
		// 转换为大字符串存储
		void convert_to_large(size_t new_capacity) {
			if (is_small) {
				char* new_data = new char[new_capacity];
				std::memcpy(new_data, storage.small_buff, length);
				storage.large.data = new_data;
				storage.large.capacity = new_capacity;
				is_small = false;
			}
			else if (new_capacity > storage.large.capacity) {
				char* new_data = new char[new_capacity];
				std::memcpy(new_data, storage.large.data, length);
				delete[] storage.large.data;
				storage.large.data = new_data;
				storage.large.capacity = new_capacity;
			}
		}
		
	public:
		// 默认构造函数
		String() : length(0), is_small(true) {
			storage.small_buff[0] = '\0';
			buff[0] = '\0';
		}
		
		// 从 C 风格字符串构造
		String(const char* s) {
			if (!s)
				s = "";
				
			length = std::strlen(s);
			
			if (length < sizeof(storage.small_buff)) {
				is_small = true;
				std::memcpy(storage.small_buff, s, length + 1);
			}
			else {
				is_small = false;
				storage.large.capacity = length + 1;
				storage.large.data = new char[storage.large.capacity];
				std::memcpy(storage.large.data, s, length + 1);
			}
			
			write_buff(s);
		}
		
		// 从 std::string 构造
		String(const std::string& s) {
			length = s.length();
			
			if (length < sizeof(storage.small_buff)) {
				is_small = true;
				std::memcpy(storage.small_buff, s.c_str(), length + 1);
			}
			else {
				is_small = false;
				storage.large.capacity = length + 1;
				storage.large.data = new char[storage.large.capacity];
				std::memcpy(storage.large.data, s.c_str(), length + 1);
			}
			
			write_buff(s);
		}
		
		// 拷贝构造函数
		String(const String& other) : length(other.length), is_small(other.is_small) {
			if (is_small) {
				std::memcpy(storage.small_buff, other.storage.small_buff, length + 1);
			}
			else {
				storage.large.capacity = other.storage.large.capacity;
				storage.large.data = new char[storage.large.capacity];
				std::memcpy(storage.large.data, other.storage.large.data, length + 1);
			}
			
			std::strcpy(buff, other.buff);
			buff_dirty = other.buff_dirty.load(); // 原子操作，正确复制 atomic<bool> 的值
		}
		
		// 移动构造函数
		String(String&& other) noexcept : length(other.length), is_small(other.is_small) {
			if (is_small) {
				std::memcpy(storage.small_buff, other.storage.small_buff, length + 1);
			}
			else {
				storage.large = other.storage.large;
			}
			
			std::strcpy(buff, other.buff);
			buff_dirty = other.buff_dirty.load(); // 原子操作，正确复制 atomic<bool> 的值
			other.length = 0;
			other.is_small = true;
			other.storage.small_buff[0] = '\0';
			other.buff_dirty = true;
		}
		
		// 析构函数
		~String() {
			free_large_storage();
		}
		
		// 赋值运算符重载
		String& operator=(const String& other) {
			if (this != &other) {
				free_large_storage();
				length = other.length;
				is_small = other.is_small;
				
				if (is_small) {
					std::memcpy(storage.small_buff, other.storage.small_buff, length + 1);
				}
				else {
					storage.large.capacity = other.storage.large.capacity;
					storage.large.data = new char[storage.large.capacity];
					std::memcpy(storage.large.data, other.storage.large.data, length + 1);
				}
				
				std::strcpy(buff, other.buff);
				buff_dirty = other.buff_dirty.load(); // 原子操作，正确复制 atomic<bool> 的值
			}
			
			return *this;
		}
		
		// 移动赋值运算符
		String& operator=(String&& other) noexcept {
			if (this != &other) {
				free_large_storage();
				length = other.length;
				is_small = other.is_small;
				
				if (is_small) {
					std::memcpy(storage.small_buff, other.storage.small_buff, length + 1);
				}
				else {
					storage.large = other.storage.large;
				}
				
				std::strcpy(buff, other.buff);
				buff_dirty = other.buff_dirty.load(); // 原子操作，正确复制 atomic<bool> 的值
				other.length = 0;
				other.is_small = true;
				other.storage.small_buff[0] = '\0';
				other.buff_dirty = true;
			}
			
			return *this;
		}
		
		// 获取字符串长度
		size_t get_length() const {
			return length;
		}
		
		// 预分配内存
		void reserve(size_t new_capacity) {
			if (new_capacity > capacity()) {
				convert_to_large(new_capacity);
			}
		}
		
		// 获取容量
		size_t capacity() const {
			return is_small ? sizeof(storage.small_buff) : storage.large.capacity;
		}
		
		// 重载 += 运算符实现字符串拼接（优化版本）
		String& operator+=(const String& other) {
			if (other.length == 0)
				return *this;
				
			size_t new_length = length + other.length;
			
			if (new_length > capacity()) {
				// 使用指数增长策略避免频繁扩容
				reserve(std::max(new_length, capacity() * 2));
			}
			
			char* dest = is_small ? storage.small_buff + length : storage.large.data + length;
			const char* src = other.is_small ? other.storage.small_buff : other.storage.large.data;
			std::memcpy(dest, src, other.length);
			length = new_length;
			
			if (is_small && length >= sizeof(storage.small_buff)) {
				convert_to_large(capacity() * 2);
			}
			else if (!is_small) {
				storage.large.data[length] = '\0';
			}
			
			buff_dirty = true;
			return *this;
		}
		
		// 重载 [] 运算符用于访问字符
		char& operator[](size_t index) {
			if (index >= length)
				throw std::out_of_range("String index out of range");
				
			return is_small ? storage.small_buff[index] : storage.large.data[index];
		}
		
		const char& operator[](size_t index) const {
			if (index >= length)
				throw std::out_of_range("String index out of range");
				
			return is_small ? storage.small_buff[index] : storage.large.data[index];
		}
		
		// 转换为 std::string
		std::string to_std_string() const {
			return std::string(data(), length);
		}
		
		// 获取 C 风格字符串
		const char* c_str() const {
			ensure_buff_updated();
			return buff;
		}
		
		// 获取数据指针
		const char* data() const {
			return is_small ? storage.small_buff : storage.large.data;
		}
		
		// 重载 << 运算符用于输出（优化版本）
		friend std::ostream& operator<<(std::ostream& os, const String& s) {
			os.write(s.data(), s.length);
			return os;
		}
		
		// 查找子字符串（使用 Boyer-Moore 算法优化）
		size_t find(const char* substr) const {
			if (!substr || !*substr)
				return 0; // 空字符串
				
			size_t substr_len = std::strlen(substr);
			
			if (substr_len > length)
				return npos;
				
			// Boyer-Moore 坏字符规则
			std::array<int, 256> bad_char;
			bad_char.fill(-1);
			
			for (size_t i = 0; i < substr_len; ++i) {
				bad_char[static_cast<unsigned char>(substr[i])] = i;
			}
			
			const char* text = data();
			size_t i = 0;
			
			while (i <= length - substr_len) {
				int j = substr_len - 1;
				
				while (j >= 0 && text[i + j] == substr[j]) {
					--j;
				}
				
				if (j < 0) {
					return i; // 找到匹配
				}
				
				// 坏字符规则
				i += std::max(1, j - bad_char[static_cast<unsigned char>(text[i + j])]);
			}
			
			return npos; // 未找到
		}
		
		// 常量静态成员，表示未找到
		static constexpr size_t npos = static_cast<size_t>(-1);
};
