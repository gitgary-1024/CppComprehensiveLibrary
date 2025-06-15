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
/*

*/

#pragma once

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "./ThreadPool.h"
#include "../Logging/Logging.h"

#ifndef MAX_BODY_SIZE
	#define MAX_BODY_SIZE 1048576
#endif

using display = std::function <
                std::string(std::unordered_map<std::string, std::string>)
                >; // 用户自定义显示函数
using analysis = std::function <
                 std::unordered_map<std::string, std::string>(std::string)
                 >; // 用户自定义解析函数

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "ws2_32.lib")
	typedef int SocketType;
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	typedef int SocketType;
#endif

class Server {
	public:
		Server(int port, int backlog = 5, size_t threadCount = std::thread::hardware_concurrency())
			: port_(port), backlog_(backlog), running_(false), server_fd_(-1), threadPool(threadCount),
			  logger("server.log", LogLevelEnum::LevelINFO) {
			initWinsock();
			createSocket();
			bindSocket();
			listenForConnections();
		}
		
		~Server() {
			stop();
		}
		
		void setServer(analysis analysisFunc, display displayFunc) {
			Analysis = analysisFunc;
			Display = displayFunc;
		}
		
		void start() {
			running_ = true;
			logger.write("Server started");
			
			while (running_) {
				sockaddr_in client_addr{};
				socklen_t client_addr_len = sizeof(client_addr);
				// 接受连接
				SocketType client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
				
				if (client_fd == -1) {
					std::cerr << "Accept failed" << std::endl;
					logger.write("Accept failed", LogLevelEnum::LevelERROR);
					continue;
				}
				
				try {
					// 将客户端请求处理任务加入线程池
					threadPool.enqueue([this, client_fd, client_addr]() {
						this->handleClient(client_fd, client_addr);
					});
				}
				catch (const std::runtime_error& e) {
					std::cerr << "Failed to enqueue task: " << e.what() << std::endl;
					logger.write("Failed to enqueue task: " + std::string(e.what()), LogLevelEnum::LevelERROR);
					// 关闭客户端连接
					#ifdef _WIN32
					closesocket(client_fd);
					#else
					close(client_fd);
					#endif
				}
			}
		}
		
		void stop() {
			if (running_) {
				running_ = false;
				logger.write("Server stopped");
				#ifdef _WIN32
				
				if (server_fd_ != -1) {
					closesocket(server_fd_);
					server_fd_ = -1;
				}
				
				WSACleanup();
				#else
				
				if (server_fd_ != -1) {
					close(server_fd_);
					server_fd_ = -1;
				}
				
				#endif
			}
		}
		
	private:
		analysis Analysis;
		display Display;
		ThreadPool threadPool;
		Logging logger;
		
		int port_;
		int backlog_;
		bool running_;
		SocketType server_fd_;
		
		void initWinsock() {
			#ifdef _WIN32
			WSADATA wsaData;
			
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				logger.write("WSAStartup failed", LogLevelEnum::LevelFATAL);
				throw std::runtime_error("WSAStartup failed");
			}
			
			#endif
		}
		
		void createSocket() {
			server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
			
			if (server_fd_ == -1) {
				logger.write("Socket creation failed", LogLevelEnum::LevelFATAL);
				throw std::runtime_error("Socket creation failed");
			}
			
			int opt = 1;
			
			if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char * >(&opt), sizeof(opt)) == -1) {
				logger.write("Setsockopt failed", LogLevelEnum::LevelFATAL);
				throw std::runtime_error("Setsockopt failed");
			}
		}
		
		void bindSocket() {
			sockaddr_in server_addr{};
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = INADDR_ANY;
			server_addr.sin_port = htons(port_);
			
			if (bind(server_fd_, reinterpret_cast<sockaddr * >(&server_addr), sizeof(server_addr)) == -1) {
				logger.write("Bind failed", LogLevelEnum::LevelFATAL);
				throw std::runtime_error("Bind failed");
			}
		}
		
		void listenForConnections() {
			if (listen(server_fd_, backlog_) == -1) {
				logger.write("Listen failed", LogLevelEnum::LevelFATAL);
				throw std::runtime_error("Listen failed");
			}
			
			std::cout << "Server started on port " << port_ << std::endl;
			logger.write("Server listening on port " + std::to_string(port_));
		}
		
		void handleClient(SocketType client_fd, const sockaddr_in& client_addr) {
			char client_ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
			std::cout << "Connection from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
			logger.write("Connection from " + std::string(client_ip) + ":" + std::to_string(ntohs(client_addr.sin_port)));
			char buffer[MAX_BODY_SIZE] = {0};
			ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
			
			if (bytes_received == -1) {
				std::cerr << "Receive failed" << std::endl;
				logger.write("Receive failed from " + std::string(client_ip) + ":" + std::to_string(ntohs(client_addr.sin_port)), LogLevelEnum::LevelERROR);
			}
			else if (bytes_received > 0) {
				// 检查是否是HTTP请求
				bool isHttp = (strncmp(buffer, "GET ", 4) == 0 ||
				strncmp(buffer, "POST ", 5) == 0 ||
				strncmp(buffer, "HEAD ", 5) == 0);
				
				if (isHttp) {
					try {
						// 提取请求路径
						std::string request(buffer);
						size_t pathStart = request.find(' ') + 1;
						size_t pathEnd = request.find(' ', pathStart);
						std::string path = request.substr(pathStart, pathEnd - pathStart);
						// 生成HTTP响应解析并生成内容
						std::unordered_map<std::string, std::string> analysis_result = Analysis(path); // 解析
						std::string display_result = Display(analysis_result); //生成内容
						send(client_fd, display_result.c_str(), display_result.length(), 0);
						logger.write("HTTP request handled successfully from " + std::string(client_ip) + ":" + std::to_string(ntohs(client_addr.sin_port)));
					}
					catch (const std::exception& e) {
						std::cerr << "LevelERROR handling HTTP request: " << e.what() << std::endl;
						logger.write("LevelERROR handling HTTP request from " + std::string(client_ip) + ":" + std::to_string(ntohs(client_addr.sin_port)) + ": " + e.what(), LogLevelEnum::LevelERROR);
					}
				}
				else {
					// 普通文本回显
					send(client_fd, buffer, bytes_received, 0);
					logger.write("Plain text echoed to " + std::string(client_ip) + ":" + std::to_string(ntohs(client_addr.sin_port)));
				}
			}
			
			// 关闭客户端连接
			#ifdef _WIN32
			closesocket(client_fd);
			#else
			close(client_fd);
			#endif
			std::cout << "Connection closed" << std::endl;
			logger.write("Connection closed from " + std::string(client_ip) + ":" + std::to_string(ntohs(client_addr.sin_port)));
		}
};
