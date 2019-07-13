#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#define wordSize 4

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <deque>
#include <stack>
#include <vector>
#include <list>
#include <map>
#include <set>

enum tokenType {
	function,
	argument,
	variable,
	assignment,
	identifire,
	operation,
	opening_bracket,
	closing_bracket,
	number,
	comma,
	semicolon
};

enum class tokenizerState {
	initial_state,
	identifire_state,
	function_state,
	op_bracket_state,
	cl_bracket_state,
	assign_state,
	digit_state,
	operation_state,
	comma_state
};

enum class parserStates {
	initial_state,
	var_state,
	func_state,
	func_arg_state,
	identifire_state,
	expression_state,
	operations_state,
	num_state,
	op_bracket_state,
	cl_bracket_state,
	semicolon_state,
	comma_state
};

const std::set<std::string> keywords = {
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"LOAD",
	"MOVE",
	"STORE",
	"CALL",
	"EXIT",
	"RET",
	"ASSIGN",
	"R0",
	"R1",
	"R2",
	"R3",
	"R4",
	"R5",
	"R6",
	"R7",
	"A0",
	"A1",
	"A2"
};


class ExceptionError {
	std::string exceptionMessage;
public:
	ExceptionError(std::string message) : exceptionMessage(message) {};
	std::string message() { return exceptionMessage; };
};

struct TokenInf {
	std::string nameOrValue;
	tokenType e_tokType;
	TokenInf(std::string name = "", tokenType type = tokenType()) :
		nameOrValue(name), e_tokType(type)
	{}
	virtual ~TokenInf() {}
};

typedef TokenInf* TokenInfPtr;

struct Node {
	Node();
	Node(TokenInfPtr);
	virtual ~Node() {};
	Node* left;
	Node* right;
	Node* parent;
	TokenInfPtr value;
};

struct FuncTokenInf : public TokenInf {
public:
	std::vector<Node*> args;
	FuncTokenInf(std::string name = "", tokenType type = function) :
		TokenInf(name, type)
	{}
};

typedef FuncTokenInf* FuncTokenInfPtr;

struct FunctionNode : public Node {
public:
	FunctionNode(TokenInfPtr);
private:
	std::vector<Node*> arguments;
};

struct func_map_inf {
	std::vector<std::string> arguments;
	bool b_isFunc;
	size_t argCount;
	
};


struct ParserStateStruct {
	parserStates state;
	tokenType type;
	std::string identifireName;
	std::vector<Node*> args; 
	std::deque<TokenInfPtr> commandTokens;
};

#endif

