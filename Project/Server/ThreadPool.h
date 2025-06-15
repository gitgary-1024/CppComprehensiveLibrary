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
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
	public:
		ThreadPool(size_t threads) : stop(false) {
			for (size_t i = 0; i < threads; ++i) {
				workers.emplace_back([this] {
					while (true) {
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(this->queue_mutex);
							this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
							
							if (this->stop && this->tasks.empty())
								return;
								
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}
						
						task();
					}
				});
			}
		}
		
		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type> {
			using return_type = typename std::result_of<F(Args...)>::type;
			auto task = std::make_shared< std::packaged_task<return_type()> >(
			                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			            );
			std::future<return_type> res = task->get_future();
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				
				// 不允许在线程池停止后添加新任务
				if (stop)
					throw std::runtime_error("enqueue on stopped ThreadPool");
					
				tasks.emplace([task]() {
					(*task)();
				});
			}
			
			condition.notify_one();
			return res;
		}
		
		~ThreadPool() {
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				stop = true;
			}
			
			condition.notify_all();
			
			for (std::thread &worker : workers) {
				worker.join();
			}
		}
		
	private:
		// 工作线程集合
		std::vector<std::thread> workers;
		// 任务队列
		std::queue<std::function<void() >> tasks;
		
		// 同步相关
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool stop;
};
