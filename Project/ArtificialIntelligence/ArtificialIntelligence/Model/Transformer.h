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

#include <iostream>
#include "./Public.h"

// 多头注意力机制类
class MultiHeadAttention {
	public:
		MultiHeadAttention(int input_dim, int num_heads) : input_dim(input_dim), num_heads(num_heads) {
			// 初始化权重
			W_q = Xavier(input_dim, input_dim); // 使用 Xavier 初始化
			W_k = Xavier(input_dim, input_dim);
			W_v = Xavier(input_dim, input_dim);
			W_o = Xavier(input_dim, input_dim);
		}
		
		~MultiHeadAttention() {}
		
		MatrixXd forward(const MatrixXd& input) {
			// 计算查询、键和值
			MatrixXd Q = input * W_q;
			MatrixXd K = input * W_k;
			MatrixXd V = input * W_v;
			// 计算注意力分数
			MatrixXd scores = Q * K.transpose();
			scores = scores * (1.0 / std::sqrt(input_dim));
			// 应用 softmax 函数
			MatrixXd attention_weights = scores;
			
			for (int i = 0; i < attention_weights.getRows(); ++i) {
				double max_val = attention_weights(i, 0);
				
				for (int j = 1; j < attention_weights.getCols(); ++j) {
					if (attention_weights(i, j) > max_val) {
						max_val = attention_weights(i, j);
					}
				}
				
				double sum = 0.0;
				
				for (int j = 0; j < attention_weights.getCols(); ++j) {
					attention_weights(i, j) = std::exp(attention_weights(i, j) - max_val);
					sum += attention_weights(i, j);
				}
				
				for (int j = 0; j < attention_weights.getCols(); ++j) {
					attention_weights(i, j) /= sum;
				}
			}
			
			// 计算加权值
			MatrixXd output = attention_weights * V;
			// 应用输出权重矩阵
			output = output * W_o;
			return output;
		}
		
		// 反向传播方法
		void backward(const MatrixXd& grad_output, const MatrixXd& input, double learning_rate) {
			MatrixXd Q = input * W_q;
			MatrixXd K = input * W_k;
			MatrixXd V = input * W_v;
			MatrixXd scores = Q * K.transpose();
			scores = scores * (1.0 / std::sqrt(input_dim));
			MatrixXd attention_weights = scores;
			
			for (int i = 0; i < attention_weights.getRows(); ++i) {
				double sum = 0.0;
				
				for (int j = 0; j < attention_weights.getCols(); ++j) {
					attention_weights(i, j) = std::exp(attention_weights(i, j));
					sum += attention_weights(i, j);
				}
				
				for (int j = 0; j < attention_weights.getCols(); ++j) {
					attention_weights(i, j) /= sum;
				}
			}
			
			MatrixXd grad_V = attention_weights.transpose() * grad_output;
			MatrixXd grad_attention_weights = grad_output * V.transpose();
			MatrixXd grad_scores = grad_attention_weights;
			
			for (int i = 0; i < grad_scores.getRows(); ++i) {
				double sum = 0.0;
				
				for (int j = 0; j < grad_scores.getCols(); ++j) {
					sum += attention_weights(i, j);
				}
				
				for (int j = 0; j < grad_scores.getCols(); ++j) {
					grad_scores(i, j) *= attention_weights(i, j) * (1 - attention_weights(i, j));
				}
			}
			
			grad_scores = grad_scores * (1.0 / std::sqrt(input_dim));
			MatrixXd grad_Q = grad_scores * K;
			MatrixXd grad_K = grad_scores.transpose() * Q;
			MatrixXd grad_W_q = input.transpose() * grad_Q;
			MatrixXd grad_W_k = input.transpose() * grad_K;
			MatrixXd grad_W_v = input.transpose() * grad_V;
			MatrixXd grad_W_o = (attention_weights * V).transpose() * grad_output;
			MatrixXd grad_W_q_clipped = grad_W_q.clipGradient(5.0);
			MatrixXd grad_W_k_clipped = grad_W_k.clipGradient(5.0);
			MatrixXd grad_W_v_clipped = grad_W_v.clipGradient(5.0);
			MatrixXd grad_W_o_clipped = grad_W_o.clipGradient(5.0);
			W_q -= learning_rate * grad_W_q_clipped;
			W_k -= learning_rate * grad_W_k_clipped;
			W_v -= learning_rate * grad_W_v_clipped;
			W_o -= learning_rate * grad_W_o_clipped;
		}
		
	private:
		int input_dim;
		int num_heads;
		MatrixXd W_q;
		MatrixXd W_k;
		MatrixXd W_v;
		MatrixXd W_o;
};

// 编码器层类
class EncoderLayer {
	public:
		EncoderLayer(int input_dim, int num_heads) : mha(input_dim, num_heads) {
			// 初始化前馈网络的权重矩阵
			W_ff1 = MatrixXd(input_dim, input_dim);
			W_ff2 = MatrixXd(input_dim, input_dim);
			
			// 随机初始化矩阵元素
			for (int i = 0; i < input_dim; ++i) {
				for (int j = 0; j < input_dim; ++j) {
					W_ff1(i, j) = (double)rand() / RAND_MAX;
					W_ff2(i, j) = (double)rand() / RAND_MAX;
				}
			}
		}
		
		~EncoderLayer() {}
		
		MatrixXd forward(const MatrixXd& input) {
			// 多头注意力机制
			MatrixXd mha_output = mha.forward(input);
			// 残差连接
			MatrixXd residual1 = input + mha_output;
			// 前馈网络
			MatrixXd ff_output = residual1 * W_ff1;
			
			// ReLU 激活函数
			for (int i = 0; i < ff_output.getRows(); ++i) {
				for (int j = 0; j < ff_output.getCols(); ++j) {
					ff_output(i, j) = std::max(0.0, ff_output(i, j));
				}
			}
			
			ff_output = ff_output * W_ff2;
			// 残差连接
			MatrixXd output = residual1 + ff_output;
			return output;
		}
		
		// 反向传播方法
		void backward(const MatrixXd& grad_output, const MatrixXd& input, double learning_rate) {
			MatrixXd mha_output = mha.forward(input);
			MatrixXd residual1 = input + mha_output;
			MatrixXd ff_output = residual1 * W_ff1;
			
			for (int i = 0; i < ff_output.getRows(); ++i) {
				for (int j = 0; j < ff_output.getCols(); ++j) {
					ff_output(i, j) = std::max(0.0, ff_output(i, j));
				}
			}
			
			ff_output = ff_output * W_ff2;
			MatrixXd grad_ff_output = grad_output;
			MatrixXd grad_ff2 = ff_output.transpose() * grad_ff_output;
			MatrixXd grad_ff1_output = grad_ff_output * W_ff2.transpose();
			
			for (int i = 0; i < grad_ff1_output.getRows(); ++i) {
				for (int j = 0; j < grad_ff1_output.getCols(); ++j) {
					if (ff_output(i, j) <= 0) {
						grad_ff1_output(i, j) = 0;
					}
				}
			}
			
			MatrixXd grad_ff1 = residual1.transpose() * grad_ff1_output;
			MatrixXd grad_residual1 = grad_ff1_output * W_ff1.transpose() + grad_output;
			MatrixXd grad_mha_output = grad_residual1;
			MatrixXd grad_input = grad_residual1;
			mha.backward(grad_mha_output, input, learning_rate);
			W_ff1 -= learning_rate * grad_ff1;
			W_ff2 -= learning_rate * grad_ff2;
		}
		
	private:
		MultiHeadAttention mha;
		MatrixXd W_ff1;
		MatrixXd W_ff2;
};

// 解码器层类
class DecoderLayer {
	public:
		DecoderLayer(int input_dim, int num_heads) : mha_self(input_dim, num_heads), mha_cross(input_dim, num_heads) {
			// 初始化前馈网络的权重矩阵
			W_ff1 = MatrixXd(input_dim, input_dim);
			W_ff2 = MatrixXd(input_dim, input_dim);
			
			// 随机初始化矩阵元素
			for (int i = 0; i < input_dim; ++i) {
				for (int j = 0; j < input_dim; ++j) {
					W_ff1(i, j) = (double)rand() / RAND_MAX;
					W_ff2(i, j) = (double)rand() / RAND_MAX;
				}
			}
		}
		
		~DecoderLayer() {}
		
		// DecoderLayer 类中的 forward 方法
		MatrixXd forward(const MatrixXd& input, const MatrixXd& encoder_output) {
			// 自注意力机制
			MatrixXd mha_self_output = mha_self.forward(input);
			// 残差连接
			MatrixXd residual1 = input + mha_self_output;
			// 交叉注意力机制（使用编码器输出）
			MatrixXd mha_cross_output = mha_cross.forward(residual1);
			// 残差连接
			MatrixXd residual2 = residual1 + mha_cross_output;
			// 前馈网络
			MatrixXd ff_output = residual2 * W_ff1;
			
			// ReLU 激活函数
			for (int i = 0; i < ff_output.getRows(); ++i) {
				for (int j = 0; j < ff_output.getCols(); ++j) {
					ff_output(i, j) = std::max(0.0, ff_output(i, j));
				}
			}
			
			ff_output = ff_output * W_ff2;
			// 残差连接
			MatrixXd output = residual2 + ff_output;
			return output;
		}
		
		// DecoderLayer 类中的 backward 方法
		void backward(const MatrixXd& grad_output, const MatrixXd& input, const MatrixXd& encoder_output, double learning_rate) {
			// 前向传播计算中间值（需要重新计算以获取正确的中间值）
			MatrixXd mha_self_output = mha_self.forward(input);
			MatrixXd residual1 = input + mha_self_output;
			MatrixXd mha_cross_output = mha_cross.forward(residual1);
			MatrixXd residual2 = residual1 + mha_cross_output;
			MatrixXd ff_output = residual2 * W_ff1;
			
			for (int i = 0; i < ff_output.getRows(); ++i) {
				for (int j = 0; j < ff_output.getCols(); ++j) {
					ff_output(i, j) = std::max(0.0, ff_output(i, j));
				}
			}
			
			ff_output = ff_output * W_ff2;
			// 反向传播（与之前相同）
			MatrixXd grad_ff_output = grad_output;
			MatrixXd grad_ff2 = ff_output.transpose() * grad_ff_output;
			MatrixXd grad_ff1_output = grad_ff_output * W_ff2.transpose();
			
			for (int i = 0; i < grad_ff1_output.getRows(); ++i) {
				for (int j = 0; j < grad_ff1_output.getCols(); ++j) {
					if (ff_output(i, j) <= 0) {
						grad_ff1_output(i, j) = 0;
					}
				}
			}
			
			MatrixXd grad_ff1 = residual2.transpose() * grad_ff1_output;
			MatrixXd grad_residual2 = grad_ff1_output * W_ff1.transpose() + grad_output;
			MatrixXd grad_mha_cross_output = grad_residual2;
			MatrixXd grad_residual1 = grad_residual2;
			// 交叉注意力反向传播（使用编码器输出）
			mha_cross.backward(grad_mha_cross_output, residual1, learning_rate);
			MatrixXd grad_mha_self_output = grad_residual1;
			MatrixXd grad_input = grad_residual1;
			mha_self.backward(grad_mha_self_output, input, learning_rate);
			W_ff1 -= learning_rate * grad_ff1;
			W_ff2 -= learning_rate * grad_ff2;
		}
		
	private:
		MultiHeadAttention mha_self;
		MultiHeadAttention mha_cross;
		MatrixXd W_ff1;
		MatrixXd W_ff2;
};

// Transformer 类
class Transformer {
		MultiHeadAttention MA;
		EncoderLayer EL;
		DecoderLayer DL;
		int input_dim;
		int num_heads;
		
	public:
		Transformer(int input_dim, int num_heads) : MA(input_dim, num_heads), EL(input_dim, num_heads), DL(input_dim, num_heads), input_dim(input_dim), num_heads(num_heads) {}
		
		~Transformer() {}
		
		// 计算均方误差损失函数
		double mse_loss(const MatrixXd& output, const MatrixXd& target) {
			double loss = 0.0;
			
			for (int i = 0; i < output.getRows(); ++i) {
				for (int j = 0; j < output.getCols(); ++j) {
					loss += std::pow(output(i, j) - target(i, j), 2);
				}
			}
			
			return loss / (output.getRows() * output.getCols());
		}
		
		void train(const MatrixXd& encoder_input, const MatrixXd& decoder_input, const MatrixXd& target, int epochs, double learning_rate) {
			for (int epoch = 0; epoch < epochs; ++epoch) {
				// 前向传播
				MatrixXd encoder_output = EL.forward(encoder_input);
				MatrixXd decoder_output = DL.forward(decoder_input, encoder_output);
				// 计算损失
				double loss = mse_loss(decoder_output, target);
				
				// 打印更多监控信息
				if (epoch % 100 == 0) {
					std::cout << "Epoch " << epoch << ", Loss: " << loss << std::endl;
					// 检查是否有 nan
					bool has_nan = false;
					
					for (int i = 0; i < decoder_output.getRows(); ++i) {
						for (int j = 0; j < decoder_output.getCols(); ++j) {
							if (std::isnan(decoder_output(i, j))) {
								has_nan = true;
								break;
							}
						}
						
						if (has_nan)
							break;
					}
					
					if (has_nan) {
						std::cout << "警告: 输出中检测到 NaN!" << std::endl;
						// 可以在这里添加调试代码，例如打印中间值
					}
				}
				
				// 反向传播
				MatrixXd grad_output = 2 * (decoder_output - target) / (decoder_output.getRows() * decoder_output.getCols());
				DL.backward(grad_output, decoder_input, encoder_output, learning_rate);
				EL.backward(grad_output, encoder_input, learning_rate);
			}
		}
		
		// 推理方法
		MatrixXd inference(const MatrixXd& encoder_input, const MatrixXd& decoder_input) {
			// 编码器前向传播
			MatrixXd encoder_output = EL.forward(encoder_input);
			// 解码器前向传播
			MatrixXd decoder_output = DL.forward(decoder_input, encoder_output);
			return decoder_output;
		}
};
