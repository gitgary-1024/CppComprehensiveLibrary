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
#include <random>
#include <cmath>
#include <iostream>
#include <functional>
#include "./Public.h"

// 定义神经网络节点结构体
struct NeuralNetworkNode {
	std::vector<double> edge; // 到下一层i号节点的边权
	double threshold; // 阈值
	double result; // 节点的输出结果
	double delta; // 用于反向传播的误差项
	ActivationFunction activation; // 激活函数
	ActivationFunction activation_derivative; // 激活函数的导数
	
	// 构造函数，初始化节点
	NeuralNetworkNode(ActivationFunction func, ActivationFunction func_derivative)
		: threshold(0.0), result(0.0), delta(0.0), activation(func), activation_derivative(func_derivative) {}
		
	// 计算当前节点的值，并应用激活函数
	double calculate(const std::vector<NeuralNetworkNode>& input, size_t currentIndex) {
		double sum = 0.0;
		
		for (size_t i = 0; i < input.size(); ++i) {
			sum += input[i].result * input[i].edge[currentIndex];
		}
		
		result = activation(sum - threshold); // 应用激活函数
		return result;
	}
};

// 定义全连接神经网络类
class FullyConnectedNeuralNetwork {

		std::vector<std::vector<NeuralNetworkNode >> nodes; // 存储神经网络的所有节点
		std::random_device rd; // 随机数设备
		std::mt19937 gen; // Mersenne Twister随机数生成器
		std::uniform_real_distribution<> dis; // 均匀分布
		ActivationFunction activation; // 激活函数
		ActivationFunction activation_derivative; // 激活函数的导数
		
	public:
		// 默认构造函数
		FullyConnectedNeuralNetwork(ActivationFunction func = relu, ActivationFunction func_derivative = relu_derivative)
			: gen(rd()), dis(-0.5, 0.5), activation(func), activation_derivative(func_derivative) {}
			
		// 构造函数，根据每层的节点数量初始化神经网络
		FullyConnectedNeuralNetwork(const std::vector<int>& layerSizes, ActivationFunction func = sigmoid, ActivationFunction func_derivative = sigmoid_derivative)
			: gen(rd()), dis(-0.5, 0.5), activation(func), activation_derivative(func_derivative) {
			int deep = layerSizes.size();
			nodes.resize(deep);
			
			// 初始化每层的节点
			for (int i = 0; i < deep; ++i) {
				nodes[i].resize(layerSizes[i], NeuralNetworkNode(activation, activation_derivative));
				
				// 初始化每个节点的边权和阈值
				if (i < deep - 1) {
					for (int j = 0; j < layerSizes[i]; ++j) {
						nodes[i][j].edge.resize(layerSizes[i + 1]);
						
						for (int k = 0; k < layerSizes[i + 1]; ++k) {
							nodes[i][j].edge[k] = dis(gen); // 初始化边权
						}
						
						nodes[i][j].threshold = dis(gen); // 初始化阈值
					}
				}
			}
		}
		
		// 析构函数
		~FullyConnectedNeuralNetwork() {}
		
		// 用input作为输入，计算当前神经元的结果
		std::vector<double> calculate(const std::vector<double>& input) {
			if (input.size() != nodes[0].size()) {
				std::cout << input.size() << " " << nodes[0].size() << std::endl;
				throw std::invalid_argument("Input size does not match the number of input nodes.");
			}
			
			int deep = nodes.size();
			
			// 初始化输入层节点的输出值
			for (size_t i = 0; i < input.size(); ++i) {
				nodes[0][i].result = input[i];
			}
			
			// 逐层计算每个节点的输出值
			for (int i = 1; i < deep; ++i) {
				for (size_t j = 0; j < nodes[i].size(); ++j) {
					nodes[i][j].calculate(nodes[i - 1], j);
				}
			}
			
			// 返回输出层节点的输出值向量
			std::vector<double> output;
			
			for (const auto& node : nodes.back()) {
				output.push_back(node.result);
			}
			
			return output;
		}
		
		// 计算均方误差损失函数
		double mse_loss(const std::vector<double>& output, const std::vector<double>& target) {
			double loss = 0.0;
			
			for (size_t i = 0; i < output.size(); ++i) {
				loss += std::pow(output[i] - target[i], 2);
			}
			
			return loss / output.size();
		}
		
		// 反向传播算法
		void backpropagate(const std::vector<double>& target, double learning_rate) {
			int deep = nodes.size();
			
			// 计算输出层的误差项
			for (size_t i = 0; i < nodes.back().size(); ++i) {
				double error = nodes.back()[i].result - target[i];
				nodes.back()[i].delta = error * nodes.back()[i].activation_derivative(nodes.back()[i].result);
			}
			
			// 反向传播计算隐藏层的误差项
			for (int i = deep - 2; i > 0; --i) {
				for (size_t j = 0; j < nodes[i].size(); ++j) {
					double sum = 0.0;
					
					for (size_t k = 0; k < nodes[i + 1].size(); ++k) {
						sum += nodes[i + 1][k].delta * nodes[i][j].edge[k];
					}
					
					nodes[i][j].delta = sum * nodes[i][j].activation_derivative(nodes[i][j].result);
				}
			}
			
			// 更新权重和阈值
			for (int i = 0; i < deep - 1; ++i) {
				for (size_t j = 0; j < nodes[i].size(); ++j) {
					for (size_t k = 0; k < nodes[i + 1].size(); ++k) {
						nodes[i][j].edge[k] -= learning_rate * nodes[i][j].result * nodes[i + 1][k].delta;
					}
					
					nodes[i][j].threshold -= learning_rate * nodes[i][j].delta;
				}
			}
		}
		
		// 训练神经网络
		void train(const std::vector<std::vector<double >> & inputs, const std::vector<std::vector<double >>& targets, int epochs, double learning_rate) {
			int epoch = 0;
			double total_loss = 0.0;
			
			for (; epoch < epochs; ++epoch) {
				total_loss = 0;
				
				for (size_t i = 0; i < inputs.size(); ++i) {
					std::vector<double> output = calculate(inputs[i]);
					double loss = mse_loss(output, targets[i]);
					total_loss += loss;
					backpropagate(targets[i], learning_rate);
				}
			}
			
			std::cout << "Epoch " << epoch << ", Loss: " << total_loss / inputs.size() << std::endl;
		}
};
