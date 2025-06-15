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

#include "./Public.h"
#include "./FullyConnectedNeuralNetwork.h"

// 定义卷积层
class ConvolutionalLayer {
		std::vector<std::vector<std::vector<double >>> filters;
		int filterSize;
		int numFilters;
		
	public:
		ConvolutionalLayer(int inputChannels, int filterSize, int numFilters)
			: filterSize(filterSize), numFilters(numFilters) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(-0.5, 0.5);
			filters.resize(numFilters);
			for (int i = 0; i < numFilters; ++i) {
				filters[i].resize(inputChannels);
				for (int j = 0; j < inputChannels; ++j) {
					filters[i][j].resize(filterSize * filterSize);
					for (int k = 0; k < filterSize * filterSize; ++k) {
						filters[i][j][k] = dis(gen);
					}
				}
			}
		}
		
		std::vector<std::vector<double >> forward(const std::vector<std::vector<std::vector<double >>> & input) {
			int inputHeight = input[0].size();
			int inputWidth = input[0][0].size();
			int outputHeight = inputHeight - filterSize + 1;
			int outputWidth = inputWidth - filterSize + 1;
			std::vector<std::vector<double >> output(numFilters, std::vector<double>(outputHeight * outputWidth, 0.0));
			
			for (int f = 0; f < numFilters; ++f) {
				for (size_t c = 0; c < input.size(); ++c) {
					for (int h = 0; h < outputHeight; ++h) {
						for (int w = 0; w < outputWidth; ++w) {
							for (int i = 0; i < filterSize; ++i) {
								for (int j = 0; j < filterSize; ++j) {
									output[f][h * outputWidth + w] += input[c][h + i][w + j] * filters[f][c][i * filterSize + j];
								}
							}
						}
					}
				}
			}
			return output;
		}
		
		// 添加公共访问器方法
		int getNumFilters() const {
			return numFilters;
		}
};

// 定义池化层
class PoolingLayer {
		int poolSize;
		
	public:
		PoolingLayer(int poolSize) : poolSize(poolSize) {}
		
		std::vector<std::vector<double >> forward(const std::vector<std::vector<double >>& input) {
			int inputHeight = static_cast<int>(std::sqrt(input[0].size()));
			int inputWidth = inputHeight;
			int outputHeight = inputHeight / poolSize;
			int outputWidth = inputWidth / poolSize;
			std::vector<std::vector<double >> output(input.size(), std::vector<double>(outputHeight * outputWidth, 0.0));
			
			for (size_t c = 0; c < input.size(); ++c) {
				for (int h = 0; h < outputHeight; ++h) {
					for (int w = 0; w < outputWidth; ++w) {
						double maxVal = -std::numeric_limits<double>::max();
						
						for (int i = 0; i < poolSize; ++i) {
							for (int j = 0; j < poolSize; ++j) {
								int inputIndex = (h * poolSize + i) * inputWidth + (w * poolSize + j);
								maxVal = std::max(maxVal, input[c][inputIndex]);
							}
						}
						
						output[c][h * outputWidth + w] = maxVal;
					}
				}
			}
			return output;
		}
};

// 定义CNN
class ConvolutionalNeuralNetwork {
		ConvolutionalLayer CL;
		PoolingLayer PL;
		FullyConnectedNeuralNetwork FCL;
		
	public:
		ConvolutionalNeuralNetwork(int inputChannels, int filterSize, int numFilters, int poolSize, const std::vector<int>& layerSizes, int OutputHeight = 32, int OutputWidth = 32)
			: CL(inputChannels, filterSize, numFilters), PL(poolSize),
			  FCL([ & ]() {
			int convOutputHeight = OutputHeight - filterSize + 1;
			int convOutputWidth = OutputWidth - filterSize + 1;
			int poolOutputHeight = convOutputHeight / poolSize;
			int poolOutputWidth = convOutputWidth / poolSize;
			int inputSize = numFilters * poolOutputHeight * poolOutputWidth;
			std::vector<int> newLayerSizes = {inputSize};
			newLayerSizes.insert(newLayerSizes.end(), layerSizes.begin(), layerSizes.end());
			return FullyConnectedNeuralNetwork(newLayerSizes, sigmoid, sigmoid_derivative);
		}()) {}
		
		~ConvolutionalNeuralNetwork() {}
		
		std::vector<double> forward(const std::vector<std::vector<std::vector<double >>> & input) {
			auto convOutput = CL.forward(input);
			auto poolOutput = PL.forward(convOutput);
			std::vector<double> flattenedOutput;
			for (const auto& channel : poolOutput) {
				flattenedOutput.insert(flattenedOutput.end(), channel.begin(), channel.end());
			}
			
			return FCL.calculate(flattenedOutput);
		}
		
		void train(const std::vector<std::vector<std::vector<std::vector<double >>> > & inputs, const std::vector<std::vector<double >> & targets, int epochs, double learning_rate) {
			int epoch = 0;
			double totalLoss = 0.0;
			for (; epoch < epochs; ++epoch) {
				totalLoss = 0.0;
				for (size_t i = 0; i < inputs.size(); ++i) {
					auto output = forward(inputs[i]);
					double loss = FCL.mse_loss(output, targets[i]);
					totalLoss += loss;
					FCL.backpropagate(targets[i], learning_rate);
				}
			}
			
			std::cout << "Epoch " << epoch << ", Loss: " << totalLoss / inputs.size() << std::endl;
		}
};
