#include "Compiler.h"

Compiler::Compiler(std::string func_file, std::string assembly_file) :
	m_funcFile(func_file, std::ios::in),
	m_assemblyFile(assembly_file, std::ios::out),
	tokenizer(m_varFuncMap)
{
	if (!m_funcFile) {
		throw ExceptionError(func_file + "can't be opened\n");
	}
	if (!m_assemblyFile) {
		throw ExceptionError(assembly_file + "can't be opened\n");
	}
}

Compiler::~Compiler()
{
	m_funcFile.close();
	m_assemblyFile.close();
}

void Compiler::compile() {
	tokenizer.tokenize(m_funcFile);
	std::vector<TokenInf*>& tokens_pool = tokenizer.getTokenPool();
	Parser parser(m_varFuncMap, tokens_pool);
	parser.parse();
	output();
}

Compiler::Tokenizer::Tokenizer(std::map<std::string, func_map_inf>& varfunc_map)
	:varFuncMap(varfunc_map)
{
	b_firstInLine = true;
	m_token = "";
	b_funcvarOrNumber = true;              // true if is indentifaire
	b_varOrFunc = true;                    // true if indentifaire is variable name 
	b_expressionPart = false;
	m_currentState = tokenizerState::initial_state;
}

void Compiler::Tokenizer::tokenize(std::ifstream& func_file)
{
	char symbol;
	while (func_file.get(symbol)) {
		switch (m_currentState) {
		case tokenizerState::initial_state:
		{
			cultivateInitialState(symbol);
			break;
		}
		case tokenizerState::digit_state:
		{
			cultivateDigitState(symbol);
			break;
		}
		case tokenizerState::identifire_state:
		{
			cultivateIndentifireState(symbol);
			break;
		}
		case tokenizerState::function_state:
		{
			cultivateFunctionState(symbol);
			break;
		}
		case tokenizerState::operation_state:
		case tokenizerState::op_bracket_state:
		case tokenizerState::cl_bracket_state:
		case tokenizerState::comma_state:
		case tokenizerState::assign_state:
		{
			cultivateOperationBracketCommaAssignState(symbol);
			break;
		}
		}
	}
}



Compiler::Parser::Parser(std::map<std::string, func_map_inf>& var_func, std::vector<TokenInf*>& t_pool)
	: var_or_func_map(var_func),
	tokensPool(t_pool)
{
	b_expression_part = false;
	currentState.state = parserStates::initial_state;
	currentState.identifireName = "";
}


void Compiler::Parser::parse()
{
	
	std::vector<TokenInfPtr>::iterator i = tokensPool.begin();
	for (; i < tokensPool.end(); ++i)
	{
		checkValidation(i);
		switch (currentState.state) {
		case parserStates::initial_state:
		{
			cultivateInitialState(i);
			break;
		}
		case parserStates::var_state:
		case parserStates::func_state:
		{
			cultivateVarFuncStates(i);
			break;
		}
		case parserStates::identifire_state:
		{
			cultivateIdentifireState(i);
			break;
		}
		case parserStates::expression_state:
		{
			cultivateExpressionState(i);
			break;
		}
		case parserStates::operations_state:
		{
			cultivateOperationsState(i);
			break;
		}
		case parserStates::op_bracket_state:
		{
			cultivateOpeningBracketState(i);
			break;
		}
		case parserStates::cl_bracket_state:
		{
			cultivateClosingBracketState(i);
			break;
		}
		case parserStates::num_state:
		{
			cultivateNumberState(i);
			break;
		}
		}
	}
}


void Compiler::Parser::addZero() {
	m_tokenForExpressin->e_tokType = number;
	m_tokenForExpressin->nameOrValue = "0";
	m_command.push_back(m_tokenForExpressin);
}

void Compiler::Parser::checkValidation(std::vector<TokenInfPtr>::const_iterator& i)
{
	if (((*i)->e_tokType == function || (*i)->e_tokType == variable) && keywords.find(((*i))->nameOrValue) != keywords.end()) {
		throw ExceptionError((*i)->nameOrValue + " is keyword");
	}

	if (b_expression_part) {
		if ((*i)->e_tokType == variable) {
			if (funcName == "" && var_or_func_map.find((*i)->nameOrValue) == var_or_func_map.end()) {
				throw ExceptionError((*i)->nameOrValue + "  was not defined");
			}
			if (funcName != "" && (*i)->e_tokType == argument) {
				std::vector<std::string>& funcArgs = var_or_func_map.find(funcName)->second.arguments;
				if (find(funcArgs.begin(), funcArgs.end(), (*i)->nameOrValue) == funcArgs.end()) {
					throw ExceptionError((*i)->nameOrValue + "  was not defined");
				}
			}
		}
		if ((*i)->e_tokType == function) {
			if (var_or_func_map.find((*i)->nameOrValue) == var_or_func_map.end()) {
				throw ExceptionError("function " + (*i)->nameOrValue + "  was not defined");
			}
		}
	}
}

void Compiler::output()
{
	if (!m_assemblyFile) {
		throw ExceptionError("File can't be opened");
	}
	std::string line;
	m_assemblyFile << ".data\n";
	for (std::deque<std::string>::iterator it = CodeGenerator::m_dataBuf.begin(); it != CodeGenerator::m_dataBuf.end(); ++it) {
		m_assemblyFile << *it;
	}
	m_assemblyFile << ".code\n";
	for (std::deque<std::string>::iterator it = CodeGenerator::m_codeBuf.begin(); it != CodeGenerator::m_codeBuf.end(); ++it) {
		m_assemblyFile << *it;
	}
	m_assemblyFile << "\tEXIT" << std::endl;
	for (std::deque<std::string>::iterator it = CodeGenerator::m_defineBuf.begin(); it != CodeGenerator::m_defineBuf.end(); ++it) {
		m_assemblyFile << *it;
	}
}

std::deque<std::string> Compiler::CodeGenerator::m_defineBuf;
std::deque<std::string> Compiler::CodeGenerator::m_codeBuf;
std::deque<std::string> Compiler::CodeGenerator::m_dataBuf;