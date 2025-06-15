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
#include <string>
#include <map>
#include <stack>
#include <functional>
#include <optional>
#include <stdexcept>
#include <algorithm>

using RegisterMap = std::unordered_map<std::string, int>;
using VariableMap = std::unordered_map<std::string, int>;
using SignMap = std::unordered_map<std::string, int>;

struct InstructionPointer {
	int value;
	explicit InstructionPointer(int v) : value(v) {}
	
	bool operator==(const InstructionPointer& other) const {
		return value == other.value;
	}
	
	InstructionPointer operator+(int offset) const {
		return InstructionPointer(value + offset);
	}
};

using Execution = std::function <
                  std::tuple <
                  InstructionPointer,
                  RegisterMap,
                  VariableMap
                  > (
                      const std::vector<std::string>&,
                      const RegisterMap&,
                      const SignMap&,
                      const VariableMap &
                  )
                  >;

using Analysis = std::function <
                 std::vector<std::string>(const std::string&)
                 >;

class Code {
	private:
		Execution executionFunc;
		Analysis analysis;
		std::vector<std::string> codes;
		
	public:
		RegisterMap registers;
		VariableMap variable;
		SignMap sign;
		InstructionPointer startIp{0};
		InstructionPointer lastIp{0};
		InstructionPointer ip{0};
		
		Code() = default;
		
		Code(Execution exec, Analysis ana, InstructionPointer start, const std::vector<std::string>& cmds)
			: executionFunc(exec), analysis(ana), codes(cmds), startIp(start) {
			lastIp = InstructionPointer(start.value + cmds.size() - 1);
			registers.reserve(5);
			variable.reserve(10);
		}
		
		std::optional<InstructionPointer> stepExecution(
		    const RegisterMap& regs,
		    const SignMap& signs,
		    const VariableMap& vars) {
			if (ip.value < 0 || ip.value >= (int)codes.size()) {
				return std::nullopt;
			}
			
			const auto& code = codes[ip.value];
			auto command = analysis(code);
			auto [newIp, newRegs, newVars] = executionFunc(command, regs, signs, vars);
			ip = newIp;
			registers = std::move(newRegs);
			variable = std::move(newVars);
			return newIp;
		}
};

class Simulator {
	private:
		std::vector<Code> codes;
		RegisterMap registers;
		VariableMap variable;
		SignMap sign;
		std::stack<Code> callStack;
		InstructionPointer lastIp{0};
		Code lastCode;
		
		void init(const Code& start) {
			while (!callStack.empty()) {
				callStack.pop();
			}
			
			callStack.push(start);
			lastCode = start;
		}
		
		bool isInCurrentBlock(InstructionPointer ip) const {
			const auto& current = callStack.top();
			return ip.value >= current.startIp.value && ip.value <= current.lastIp.value;
		}
		
		Code findCodeBlock(InstructionPointer ip) {
			for (const auto& code : codes) {
				if (ip.value >= code.startIp.value && ip.value <= code.lastIp.value) {
					return code;
				}
			}
			
			throw std::runtime_error("Code block not found for IP: " + std::to_string(ip.value));
		}
		
		void synchronous() {
			callStack.top().registers = this->registers;
			callStack.top().variable = this->variable;
		}
		
	public:
		Simulator() = default;
		
		Simulator(const SignMap& signs, const VariableMap& vars,
		          const RegisterMap& regs, const Code& start,
		          const std::vector<Code>& codeBlocks)
			: codes(codeBlocks), registers(regs), variable(vars), sign(signs) {
			init(start);
		}
		
		std::optional<InstructionPointer> stepExecution() {
			auto& current = callStack.top();
			auto result = current.stepExecution(registers, sign, variable);
			
			if (!result) {
				return std::nullopt;
			}
			
			registers = current.registers;
			variable = current.variable;
			lastIp = *result;
			
			if (isInCurrentBlock(lastIp)) {
				return lastIp;
			}
			else {
				try {
					Code newCode = findCodeBlock(lastIp);
					// 同步寄存器和变量
					synchronous();
					callStack.push(newCode);
					lastCode = newCode;
					return lastIp;
				}
				catch (const std::exception& e) {
					return std::nullopt;
				}
			}
		}
		
		std::stack<Code> getCallStack() const {
			return callStack;
		}
		
		RegisterMap getRegiste() {
			return registers;
		}
		
		VariableMap getVariable() {
			return variable;
		}
		
};
