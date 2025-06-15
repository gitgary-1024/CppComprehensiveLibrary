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

#include <bits/stdc++.h>

class ExpressionSyntaxError : public std::logic_error {
	public:
		explicit ExpressionSyntaxError(const std::string& message =
		                                   "Expression syntax error")
			: std::logic_error(message) {}
};

class DivideZeroError : public std::runtime_error {
	public:
		explicit DivideZeroError(const std::string& message = "Divide by zero error")
			: std::runtime_error(message) {}
};

class MismatchedParentheses : public std::runtime_error {
	public:
		explicit MismatchedParentheses(const std::string& message =
		                                   "Mismatched parentheses")
			: std::runtime_error(message) {}
};

struct Node {
	std::string value;
	Node* left;
	Node* right;
	
	Node(const std::string& val) : value(val), left(nullptr), right(nullptr) {}
	
	Node(const std::string& val, Node* l, Node* r) : value(val), left(l),
		right(r) {}
};

class ExpressionEvaluator {
	private:
		std::string expression;
		Node* root;
		std::unordered_map<std::string, double> publicTermMap;
		
		// 辅助函数：复制树
		Node* copyTree(Node* node) {
			if (node == nullptr) {
				return nullptr;
			}
			
			Node* newNode = new Node(node->value);
			newNode->left = copyTree(node->left);
			newNode->right = copyTree(node->right);
			return newNode;
		}
		
		bool isValidDecimal(const std::string& str) const {
			int dotCount = 0;
			
			for (size_t i = 0; i < str.length(); i++) {
				if (str[i] == '.') {
					dotCount++;
					
					if (dotCount > 1) {
						return false;
					}
				}
				else if (!std::isdigit(str[i])) {
					if (i == 0 && str[i] == '-') {
						continue;
					}
					
					return false;
				}
			}
			
			return dotCount <= 1;
		}
		
		void check_expression(const std::string& expression) {
			if (expression.empty()) {
				throw ExpressionSyntaxError();
			}
			
			int parentheses = 0;
			
			for (char c : expression) {
				if (c == '(') {
					parentheses++;
				}
				else if (c == ')') {
					parentheses--;
				}
				
				if (parentheses < 0) {
					throw MismatchedParentheses();
				}
			}
			
			if (parentheses != 0) {
				throw MismatchedParentheses();
			}
		}
		
		bool isConstantOnNode(Node* n) const {
			if (n == nullptr)
				return false;
				
			const std::string& value = n->value;
			
			if (value.empty())
				return false;
				
			if (value == "-")
				return false;
				
			return isValidDecimal(value);
		}
		
		std::string unifiedFormat(const std::string& expression) {
			std::string result;
			
			for (size_t i = 0; i < expression.length(); i++) {
				char c = expression[i];
				
				if (std::isdigit(c) || c == '.' || (c == '-' && (i == 0 ||
				isOperator(expression[i - 1]) || expression[i - 1] == '('))) {
					size_t start = i;
					
					if (c == '-') {
						i++;
					}
					
					while (i < expression.length() && (std::isdigit(expression[i]) ||
					expression[i] == '.')) {
						i++;
					}
					
					result += expression.substr(start, i - start);
					i--;
					
					if (i + 1 < expression.length()) {
						char next = expression[i + 1];
						
						if (std::isalnum(next) && !isOperator(next)) {
							result += '*';
						}
					}
				}
				else if (c == '(') {
					result += c;
					
					if (i + 1 < expression.length()) {
						char next = expression[i + 1];
						bool needMultiply = false;
						
						if (i > 0) {
							char prev = expression[i - 1];
							
							if (std::isalnum(prev) || prev == ')') {
								needMultiply = true;
							}
						}
						
						if (needMultiply && std::isalnum(next) && !isOperator(next)) {
							result += '*';
						}
					}
				}
				else {
					result += c;
				}
			}
			
			return result;
		}
		
		void inorderTraversal(Node* node) const {
			if (node == nullptr)
				return;
				
			if (node->left && isOperator(node->left->value[0])) {
				std::cout << "(";
				inorderTraversal(node->left);
				std::cout << ")";
			}
			else {
				inorderTraversal(node->left);
			}
			
			std::cout << node->value;
			
			if (node->right && isOperator(node->right->value[0])) {
				std::cout << "(";
				inorderTraversal(node->right);
				std::cout << ")";
			}
			else {
				inorderTraversal(node->right);
			}
		}
		
		int precedence(char op) {
			if (op == '+' || op == '-')
				return 1;
				
			if (op == '*' || op == '/')
				return 2;
				
			if (op == '^')
				return 3;
				
			return 0;
		}
		
		bool isOperator(char c) const {
			return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
		}
		
		Node* constructExpressionTree(const std::string& expression) {
			std::stack<Node*> values;
			std::stack<char> operators;
			
			for (size_t i = 0; i < expression.length(); i++) {
				char c = expression[i];
				
				if (c == ' ')
					continue;
					
				if (std::isdigit(c) || (c == '-' && (i == 0 || isOperator(expression[i - 1]) ||
				expression[i - 1] == '(')) || c == '.') {
					std::string valStr;
					
					if (c == '-') {
						valStr += c;
						i++;
					}
					
					while (i < expression.length() && (std::isdigit(expression[i]) ||
					expression[i] == '.')) {
						valStr += expression[i];
						i++;
					}
					
					values.push(new Node(valStr));
					i--;
				}
				else if (std::isalpha(c)) {
					values.push(new Node(std::string(1, c)));
				}
				else if (c == '(') {
					operators.push(c);
				}
				else if (c == ')') {
					while (!operators.empty() && operators.top() != '(') {
						if (values.size() < 2) {
							throw ExpressionSyntaxError("Invalid expression syntax:" + expression);
						}
						
						Node* right = values.top();
						values.pop();
						Node* left = values.top();
						values.pop();
						char op = operators.top();
						operators.pop();
						values.push(new Node(std::string(1, op), left, right));
					}
					
					if (!operators.empty())
						operators.pop();
				}
				else if (isOperator(c)) {
					while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
						if (values.size() < 2) {
							throw ExpressionSyntaxError("Invalid expression syntax:" + expression);
						}
						
						Node* right = values.top();
						values.pop();
						Node* left = values.top();
						values.pop();
						char op = operators.top();
						operators.pop();
						values.push(new Node(std::string(1, op), left, right));
					}
					
					operators.push(c);
				}
			}
			
			while (!operators.empty()) {
				if (values.size() < 2) {
					throw ExpressionSyntaxError("Invalid expression syntax：" + expression);
				}
				
				Node* right = values.top();
				values.pop();
				Node* left = values.top();
				values.pop();
				char op = operators.top();
				operators.pop();
				values.push(new Node(std::string(1, op), left, right));
			}
			
			return values.top();
		}
		
		void deleteNode(Node* node) {
			if (node == nullptr)
				return;
				
			std::stack<Node*> nodes;
			nodes.push(node);
			
			while (!nodes.empty()) {
				Node* current = nodes.top();
				nodes.pop();
				
				if (current->left)
					nodes.push(current->left);
					
				if (current->right)
					nodes.push(current->right);
					
				delete current;
			}
		}
		
		Node* sortExpressionTree(Node* node) {
			if (node == nullptr)
				return nullptr;
				
			if (node->value == "+" || node->value == "*") {
				node->left = sortExpressionTree(node->left);
				node->right = sortExpressionTree(node->right);
				
				if (isConstantOnNode(node->left) && !isConstantOnNode(node->right)) {
					std::swap(node->left, node->right);
				}
				
				if (node->value == "*") {
					if (node->left && node->left->value == "*") {
						if (isConstantOnNode(node->left->left) &&
						    !isConstantOnNode(node->left->right)) {
							std::swap(node->left->left, node->left->right);
						}
					}
				}
			}
			
			return node;
		}
		
		double evaluateNode(Node* node,
		                    const std::unordered_map<std::string, double>& varValues) const {
			if (isConstantOnNode(node)) {
				try {
					return std::stod(node->value);
				}
				catch (const std::invalid_argument& e) {
					std::cerr << "Invalid argument for std::stod: " << node->value << std::endl;
					throw;
				}
			}
			
			if (std::isalpha(node->value[0])) {
				auto it = varValues.find(node->value);
				
				if (it != varValues.end()) {
					return it->second;
				}
				
				throw std::runtime_error("Variable value not provided: " + node->value);
			}
			
			if (node->value == "+") {
				return evaluateNode(node->left, varValues) + evaluateNode(node->right,
				       varValues);
			}
			
			if (node->value == "-") {
				return evaluateNode(node->left, varValues) - evaluateNode(node->right,
				       varValues);
			}
			
			if (node->value == "*") {
				return evaluateNode(node->left, varValues) * evaluateNode(node->right,
				       varValues);
			}
			
			if (node->value == "/") {
				double rightVal = evaluateNode(node->right, varValues);
				
				if (rightVal == 0) {
					throw DivideZeroError();
				}
				
				return evaluateNode(node->left, varValues) / rightVal;
			}
			
			if (node->value == "^") {
				return std::pow(evaluateNode(node->left, varValues), evaluateNode(node->right,
				                varValues));
			}
			
			throw std::runtime_error("Unsupported operator: " + node->value);
		}
		
		void extractCoefficients(Node* node, double currentCoefficient = 1.0) {
			if (node == nullptr)
				return;
				
			if (isOperator(node->value[0])) {
				if (node->value == "+") {
					// 处理加法，继续递归左右子树
					extractCoefficients(node->left, currentCoefficient);
					extractCoefficients(node->right, currentCoefficient);
				}
				else if (node->value == "-") {
					// 处理减法，右子树系数取负
					extractCoefficients(node->left, currentCoefficient);
					extractCoefficients(node->right, -currentCoefficient);
				}
				else if (node->value == "*") {
					if (isConstantOnNode(node->left)) {
						// 如果左子节点是常数，更新当前系数
						double newCoefficient = currentCoefficient * std::stod(node->left->value);
						extractCoefficients(node->right, newCoefficient);
					}
					else {
						// 否则，继续递归左右子树
						extractCoefficients(node->left, currentCoefficient);
						extractCoefficients(node->right, currentCoefficient);
					}
				}
			}
			else if (std::isalpha(node->value[0])) {
				// 如果是未知数，累加系数到 publicTermMap
				publicTermMap[node->value] += currentCoefficient;
			}
			else if (isConstantOnNode(node)) {
				// 如果是常数项，将其值乘以当前系数累加到 "constant" 项上
				publicTermMap["constant"] += currentCoefficient * std::stod(node->value);
			}
		}
		
		void __init() {
			check_expression(expression);
			expression = unifiedFormat(expression);
			root = constructExpressionTree(expression);
			root = sortExpressionTree(root);
			extractCoefficients(root);
			// root = combineLikeTerms(root); //这个功能没有完善
			#ifdef _DEBUG
			printPublicTerms();
			#endif
		}
		
	public:
		ExpressionEvaluator() {}
		
		ExpressionEvaluator(const std::string& expr) : expression(expr), root(nullptr) {
			__init();
		}
		
		// 拷贝构造函数
		ExpressionEvaluator(const ExpressionEvaluator& other) : expression(other.expression) {
			if (other.root) {
				root = copyTree(other.root);
			}
			else {
				root = nullptr;
			}
			
			// 不需要再次调用 __init()，因为已经复制了树
		}
		
		// 赋值运算符重载
		ExpressionEvaluator& operator=(const ExpressionEvaluator& other) {
			if (this != &other) {
				if (!(expression == "" || root == nullptr)) {
					deleteNode(root);
				}
				
				expression = other.expression;
				
				if (other.root) {
					root = copyTree(other.root);
				}
				else {
					root = nullptr;
				}
				
				// 不需要再次调用 __init()，因为已经复制了树
			}
			
			return *this;
		}
		
		~ExpressionEvaluator() {
			deleteNode(root);
			root = nullptr;
		}
		
		void printExpression() const {
			#ifdef _DEBUG
			printf("Expression:");
			
			if (root) {
				inorderTraversal(root);
			}
			else {
				printf("Empty expression tree");
			}
			
			std::cout << std::endl;
			#endif
		}
		
		std::unordered_map<std::string, double> printPublicTerms() {
			#ifdef _DEBUG
			printExpression();
			
			for (const auto& pair : publicTermMap) {
				std::cout << pair.first << "->" << pair.second << ".";
			}
			
			std::cout << "\n";
			#endif
			return publicTermMap;
		}
		
		double evaluate(const std::unordered_map<std::string, double>& varValues)
		const {
			return evaluateNode(root, varValues);
		}
		
		std::string getExpression() const {
			return expression;
		}
};

