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

#include <fstream>
#include <string>
#include <ctime>
#include <stdexcept>
#include <sstream>

// 格式化时间函数
std::string formatTime(const std::time_t& time, const std::string& format = "%Y-%m-%d %H:%M:%S") {
	std::tm* localTime = std::localtime(&time);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), format.c_str(), localTime);
	std::ostringstream oss;
	oss << "[" << buffer << "]";
	return oss.str();
}

// 日志级别枚举
enum class LogLevelEnum {
	LevelTRACE,
	LevelDEBUG,
	LevelINFO,
	LevelWARN,
	LevelERROR,
	LevelFATAL
};

// 日志级别枚举转字符串
std::string logLevelToString(LogLevelEnum level) {
	switch (level) {
		case LogLevelEnum::LevelTRACE:
			return "TRACE";
			
		case LogLevelEnum::LevelDEBUG:
			return "DEBUG";
			
		case LogLevelEnum::LevelINFO:
			return "INFO";
			
		case LogLevelEnum::LevelWARN:
			return "WARN";
			
		case LogLevelEnum::LevelERROR:
			return "ERROR";
			
		case LogLevelEnum::LevelFATAL:
			return "FATAL";
			
		default:
			return "UNKNOWN";
	}
}

// 日志记录类
class Logging {
		std::fstream log;
		LogLevelEnum level; // 当前日志的默认级别
		
	public:
		// 默认构造函数
		Logging() {}
		
		// 带参数的构造函数，打开日志文件并设置默认日志级别
		Logging(std::string fileName, LogLevelEnum level) {
			log.open(fileName, std::ios::app); // 使用追加模式打开文件
			
			if (!log.is_open()) {
				throw std::runtime_error("Failed to open log file: " + fileName);
			}
			
			this->level = level;
		}
		
		// 以默认日志级别写入日志
		void write(std::string content) {
			std::time_t currentTime = std::time(nullptr);
			log << formatTime(currentTime) << " " << logLevelToString(level) << " " << content << std::endl;
		}
		
		// 以指定的特殊日志级别写入日志
		void write(std::string content, LogLevelEnum specialLevel) {
			std::time_t currentTime = std::time(nullptr);
			log << formatTime(currentTime) << " " << logLevelToString(specialLevel) << " " << content << std::endl;
		}
		
		// 析构函数，关闭日志文件
		~Logging() {
			if (log.is_open()) {
				log.close();
			}
		}
};
