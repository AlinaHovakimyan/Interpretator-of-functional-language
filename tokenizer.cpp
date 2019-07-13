#include "Compiler.h"
//----------------------------------------------------------------------------------------
// FROM INITIALIZE_STATE_TO FUNCTIONS
//-----------------------------------------------------------------------------------------

void Compiler::Tokenizer::cultivateFromInitToOpBracket() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = opening_bracket;
	m_tok_inf->nameOrValue = "(";
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::op_bracket_state;
	return;
}

void Compiler::Tokenizer::cultivateFromInitToClBracket() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = closing_bracket;
	m_tok_inf->nameOrValue = ")";
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::cl_bracket_state;
	return;
}
void Compiler::Tokenizer::cultivateFromInitToAssign() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = assignment;
	m_tok_inf->nameOrValue = "=";
	b_expressionPart = true;                     
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::assign_state;
	return;
}
void Compiler::Tokenizer::cultivateFromInitToFinal() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = semicolon;
	m_tok_inf->nameOrValue = ";";
	b_expressionPart = false;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::initial_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_init_to_comma() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = comma;
	m_tok_inf->nameOrValue = ',';
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::comma_state;
	return;


}
void Compiler::Tokenizer::cultivate_from_init_to_operation(const char& symbol) {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = operation;
	m_tok_inf->nameOrValue = symbol;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::operation_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_init_to_digit(const char& symbol) {
	m_token += symbol;
	b_funcvarOrNumber = false;
	m_currentState = tokenizerState::digit_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_init_to_indentifire(const char& symbol) {
	m_token += symbol;
	b_funcvarOrNumber = true;
	m_currentState = tokenizerState::identifire_state;
	return;
}


void Compiler::Tokenizer::cultivateInitialState(char symbol) {
	switch (symbol) {
	case ' ':
	case '\n':
	{
		break;
	}
	case '(':
	{
		cultivateFromInitToOpBracket();
		break;
	}
	case ')':
	{
		cultivateFromInitToClBracket();
		break;
	}
	case '=':
	{
		cultivateFromInitToAssign();
		break;

	}
	case ';':
	{
		cultivateFromInitToFinal();
		break;
	}
	case ',':
	{
		cultivate_from_init_to_comma();
		break;
	}
	default:
	{
		if (isalpha(symbol) || symbol == '_') {
			cultivate_from_init_to_indentifire(symbol);
		}
		else if (isdigit(symbol)) {
			cultivate_from_init_to_digit(symbol);
		}
		else if (Tree::isOperator(symbol)) {
			cultivate_from_init_to_operation(symbol);
		}
		else {
			throw ExceptionError("Token can't be started with symbol " + symbol);
		}
		break;
	}
	}

}

//---------------------------------------------------------------------------------------
// FROM_DIGIT_STATE_TO FUNCTIONS
//---------------------------------------------------------------------------------------

void Compiler::Tokenizer::cultivate_from_digit_to_init() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = number;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";
	m_currentState = tokenizerState::initial_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_digit_to_op_bracket() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = number;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";
	m_currentState = tokenizerState::op_bracket_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_digit_to_cl_bracket() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = number;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = closing_bracket;
	m_tok_inf->nameOrValue = ")";
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::cl_bracket_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_digit_to_assign() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = number;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = assignment;
	m_tok_inf->nameOrValue = "=";
	b_expressionPart = true;                       
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::assign_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_digit_to_final() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = number;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = semicolon;
	m_tok_inf->nameOrValue = ";";
	m_tokenPool.push_back(m_tok_inf);
	b_firstInLine = true;
	b_funcvarOrNumber = true;
	b_varOrFunc = true;
	b_expressionPart = false;
	funcName = "";
	m_currentState = tokenizerState::initial_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_digit_to_comma() {
	TokenInfPtr m_tok_inf = new TokenInf();;
	m_tok_inf->e_tokType = number;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = comma;
	m_tok_inf->nameOrValue = ",";
	b_expressionPart = true;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::comma_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_digit_to_operation(const char& symbol) {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = number;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = operation;
	m_tok_inf->nameOrValue = symbol;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::operation_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_digit_to_digit(const char& symbol) {
	m_token += symbol;
	return;
}


void Compiler::Tokenizer::cultivateDigitState(char symbol) {
	switch (symbol) {
	case ' ':
	case '\n':
	{
		cultivate_from_digit_to_init();
		break;
	}
	case '(':
	{
		cultivate_from_digit_to_op_bracket();
		break;
	}
	case ')':
	{
		cultivate_from_digit_to_cl_bracket();
		break;
	}
	case '=':
	{
		cultivate_from_digit_to_assign();
		break;

	}
	case ';':
	{
		cultivate_from_digit_to_final();
		break;
	}
	case ',':
	{
		cultivate_from_digit_to_comma();
		break;
	}
	default:
	{
		if (isdigit(symbol)) {
			cultivate_from_digit_to_digit(symbol);
		}
		else if (Tree::isOperator(symbol)) {
			cultivate_from_digit_to_operation(symbol);
		}
		else {
			throw ExceptionError("Number can't contain symbol " + symbol);
		}
		break;
	}
	}
}
//-----------------------------------------------------------------------------------------
// FROM_INDENTIFIRE_STATE_TO
//------------------------------------------------------------------------------------------

void Compiler::Tokenizer::cultivate_from_indentifire_to_init() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = b_expressionPart ? identifire : variable;
	m_tok_inf->nameOrValue = m_token;
	if (!b_expressionPart && varFuncMap.find(m_token) == varFuncMap.end()) {
		varFuncMap[m_token].b_isFunc = false;
	}
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";
	b_varOrFunc = false;
	m_currentState = tokenizerState::initial_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_indentifire_to_op_bracket() {
	if (!b_expressionPart) {
		if (b_varOrFunc) {
			b_varOrFunc = false;
			varFuncMap[m_token].b_isFunc = true;
			m_currentState = tokenizerState::function_state;
			funcName = m_token;
		}
	}
	else {
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = identifire;
		m_tok_inf->nameOrValue = m_token;
		m_tokenPool.push_back(m_tok_inf);

		m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = opening_bracket;
		m_tok_inf->nameOrValue = "(";
		m_tokenPool.push_back(m_tok_inf);
		m_currentState = tokenizerState::op_bracket_state;
	}
	m_token = "";
	return;
}
void Compiler::Tokenizer::cultivate_from_indentifire_to_cl_bracket() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = identifire;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";
	b_varOrFunc = false;

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = closing_bracket;
	m_tok_inf->nameOrValue = ")";
	m_tokenPool.push_back(m_tok_inf);

	//}
	m_currentState = tokenizerState::cl_bracket_state;
}
void Compiler::Tokenizer::cultivate_from_indentifire_to_assign() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = variable;
	m_tok_inf->nameOrValue = m_token;
	if (!b_expressionPart && varFuncMap.find(m_token) == varFuncMap.end()) {
		varFuncMap[m_token].b_isFunc = false;
	}
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";
	b_varOrFunc = false;

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = assignment;
	m_tok_inf->nameOrValue = "=";
	b_expressionPart = true;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::assign_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_indentifire_to_final() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = identifire;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = semicolon;
	m_tok_inf->nameOrValue = ";";
	m_tokenPool.push_back(m_tok_inf);
	b_firstInLine = true;
	b_funcvarOrNumber = true;
	b_varOrFunc = true;
	b_expressionPart = false;

	m_currentState = tokenizerState::initial_state;
	return;
}
void Compiler::Tokenizer::cultivate_from_indentifire_to_comma() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = identifire;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);
	m_token = "";
	b_varOrFunc = false;

	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = comma;
	m_tok_inf->nameOrValue = ",";
	b_expressionPart = true;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::assign_state;
	return;

}
void Compiler::Tokenizer::cultivate_from_indentifire_to_operation(const char& symbol) {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = identifire;
	m_tok_inf->nameOrValue = m_token;
	m_tokenPool.push_back(m_tok_inf);

	if (b_expressionPart && varFuncMap.find(m_tok_inf->nameOrValue) == varFuncMap.end()) {
		throw ExceptionError("Undefined variable " + m_token);
	}
	m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = operation;
	m_tok_inf->nameOrValue = symbol;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::operation_state;
	m_token = "";
	return;
}
void Compiler::Tokenizer::cultivate_from_indentifire_to_indentifire(const char& symbol) {
	m_token += symbol;
	return;
}

void Compiler::Tokenizer::cultivate_from_func_to_cl_bracket(std::string funcName) {
	TokenInfPtr m_tok_inf = new TokenInf();;
	if (m_token != "") {
		varFuncMap[funcName].arguments.push_back(m_token);
		m_token = "";
	}
	varFuncMap[funcName].argCount = varFuncMap[funcName].arguments.size();
	m_tok_inf->e_tokType = function;
	m_tok_inf->nameOrValue = funcName;
	if (!b_expressionPart && varFuncMap.find(m_token) != varFuncMap.end()) {
		throw ExceptionError("Double definetion of function " + m_token);
	}
	m_tokenPool.push_back(m_tok_inf);
	b_varOrFunc = true;
	m_currentState = tokenizerState::cl_bracket_state;
	m_token = "";
	return;
}

void Compiler::Tokenizer::cultivate_in_func_to_comma(std::string funcName) {
	if (m_token != "") {
		varFuncMap[funcName].arguments.push_back(m_token);
		m_token = "";
	}
	else {
		throw ExceptionError("Expected argument before comma in declaration of function " + funcName);
	}
	return;
}

void Compiler::Tokenizer::cultivateIndentifireState(char symbol) {
	switch (symbol) {
	case ' ':
	case '\n':
	{
		cultivate_from_indentifire_to_init();
		break;
	}
	case '(':
	{
		cultivate_from_indentifire_to_op_bracket();
		break;
	}
	case ')':
	{
		cultivate_from_indentifire_to_cl_bracket();
		break;
	}
	case '=':
	{
		cultivate_from_indentifire_to_assign();
		break;
	}
	case ',':
	{
		cultivate_from_indentifire_to_comma();
		break;
	}
	case ';':
	{
		cultivate_from_indentifire_to_final();
		break;
	}
	default:
	{
		if (isalpha(symbol) || symbol == '_' || isdigit(symbol)) {
			cultivate_from_indentifire_to_indentifire(symbol);
		}
		else if (Tree::isOperator(symbol)) {
			cultivate_from_indentifire_to_operation(symbol);
			break;
		}
		else {
			throw ExceptionError("Indentifire can't contain symbol: " + symbol);
		}
		break;
	}
	}
}

void Compiler::Tokenizer::cultivateFunctionState(char symbol) {
	switch (symbol) {
	case ' ':
	case '\n':
	{
		break;
	}
	case ')':
	{
		cultivate_from_func_to_cl_bracket(funcName);
		break;
	}
	case ',':
	{
		cultivate_in_func_to_comma(funcName);
		break;
	}
	default:
	{
		if (isalpha(symbol) || symbol == '_' || isdigit(symbol)) {
			cultivate_from_indentifire_to_indentifire(symbol);
		}
		else {
			throw ExceptionError("Function declaration can't contain symbol: " + symbol);
		}
		break;
	}
	}
}

//-----------------------------------------------------------------------------------------------------
// FROM_OTHERS_TO
//-----------------------------------------------------------------------------------------------------

void Compiler::Tokenizer::cultivate_to_op_bracket() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = opening_bracket;
	m_tok_inf->nameOrValue = "(";
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::op_bracket_state;
	return;
}
void Compiler::Tokenizer::cultivate_to_cl_bracket() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = closing_bracket;
	m_tok_inf->nameOrValue = ")";
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::cl_bracket_state;
	return;
}
void Compiler::Tokenizer::cultivate_to_assign() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = assignment;
	m_tok_inf->nameOrValue = "=";
	b_expressionPart = true;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::assign_state;
	return;
}
void Compiler::Tokenizer::cultivate_to_final() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = semicolon;
	m_tok_inf->nameOrValue = ";";
	b_expressionPart = false;
	b_funcvarOrNumber = true;
	b_varOrFunc = true;
	b_firstInLine = true;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::initial_state;
	return;
}
void Compiler::Tokenizer::cultivate_to_comma() {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = comma;
	m_tok_inf->nameOrValue = ",";
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::comma_state;
	return;
}
void Compiler::Tokenizer::cultivate_to_operation(const char& symbol) {
	TokenInfPtr m_tok_inf = new TokenInf();
	m_tok_inf->e_tokType = operation;
	m_tok_inf->nameOrValue = symbol;
	m_tokenPool.push_back(m_tok_inf);
	m_currentState = tokenizerState::operation_state;
	return;
}
void Compiler::Tokenizer::cultivate_to_indentifire(const char& symbol) {
	m_token += symbol;
	m_currentState = tokenizerState::identifire_state;
	return;
}
void Compiler::Tokenizer::cultivate_to_digit(const char& symbol) {
	m_token += symbol;
	m_currentState = tokenizerState::digit_state;
	return;
}

void Compiler::Tokenizer::cultivateOperationBracketCommaAssignState(char symbol) {
	switch (symbol) {
	case ' ':
	case '\n':
	{
		break;
	}
	case '(':
	{
		cultivate_to_op_bracket();
		break;
	}
	case ')':
	{
		cultivate_to_cl_bracket();
		break;
	}
	case '=':
	{
		cultivate_to_assign();
		break;
	}
	case ',':
	{
		cultivate_to_comma();
		break;
	}
	case ';':
	{
		cultivate_to_final();
		break;
	}
	default:
	{
		if (isdigit(symbol)) {
			cultivate_to_digit(symbol);
		}
		else if (isalpha(symbol) || symbol == '_') {
			cultivate_to_indentifire(symbol);
		}
		else if (Tree::isOperator(symbol)) {
			cultivate_to_operation(symbol);
			break;
		}
		else {
			throw ExceptionError("Syntax error: Used unoffered symbol" + symbol);
		}
		break;
	}
	}
}