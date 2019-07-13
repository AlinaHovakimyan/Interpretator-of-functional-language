#include "Compiler.h"
//--------------------------------------------------------------------------------------------------
//Parser_calculate_state
//--------------------------------------------------------------------------------------------------
void Compiler::Parser::cultivateInitialState(const std::vector<TokenInf*>::const_iterator& i) {
	switch ((*i)->e_tokType) {
	case semicolon:
	{
		break;
	}
	case variable:
	{
		m_command.push_back(*i);
		currentState.state = parserStates::var_state;
		break;
	}
	case function:
	{
		currentState.state = parserStates::func_state;
		m_command.push_back(*i);
		funcName = (*i)->nameOrValue;
		break;
	}
	default:
	{
		throw ExceptionError("Command can't start with " + (*i)->nameOrValue);
	}
	}
}

void Compiler::Parser::cultivateExpressionState(std::vector<TokenInf*>::iterator& i) {
	if ((*i)->nameOrValue == "+" || (*i)->nameOrValue == "-") {
		addZero();
	}
	else if ((*i)->nameOrValue == "(") {
		m_command.push_back(*i);
		currentState.state = parserStates::op_bracket_state;
	}
	else if ((*i)->nameOrValue == ")") {
		m_command.push_back(*i);
		currentState.state = parserStates::cl_bracket_state;
	}
	else if ((*i)->e_tokType == variable || (*i)->e_tokType == argument) {
		m_command.push_back(*i);
		currentState.state  = parserStates::var_state;
	}
	else if ((*i)->e_tokType == function) {          
		m_command.push_back(*i);
		currentState.state = parserStates::func_state;
	}
	else if ((*i)->e_tokType == identifire) {        
		currentState.identifireName = (*i)->nameOrValue;
		currentState.state = parserStates::identifire_state;
	}
	else if ((*i)->e_tokType == number) {
		m_command.push_back(*i);
		currentState.state = parserStates::num_state;
	}
	else {
		throw ExceptionError("Expression can't began with " + (*i)->nameOrValue);
	}
}

void Compiler::Parser::cultivateOperationsState(std::vector<TokenInf*>::iterator& i) {
	switch ((*i)->e_tokType) {
	case variable:
	case argument:
	{
		currentState.state = parserStates::var_state;
		m_command.push_back(*i);
		break;
	}
	case identifire:
	{
		currentState.identifireName = (*i)->nameOrValue;
		currentState.state = parserStates::identifire_state;
		break;
	}
	case function:
	{
		currentState.state = parserStates::func_state;
		m_command.push_back(*i);
		break;
	}
	case number:
	{
		currentState.state = parserStates::num_state;
		m_command.push_back(*i);
		break;
	}
	case opening_bracket:
	{
		currentState.state = parserStates::op_bracket_state;
		m_command.push_back(*i);
		break;
	}
	default:
	{
		throw ExceptionError(" Can't be " + (*i)->nameOrValue + " after operation");
		break;
	}
	}
}

void Compiler::Parser::cultivateOpeningBracketState(std::vector<TokenInf*>::iterator& i) {
	if ((*i)->nameOrValue == "+" || (*i)->nameOrValue == "-") {
		addZero();
	}
	else {
		switch ((*i)->e_tokType) {
		case opening_bracket:
		{
			currentState.state = parserStates::op_bracket_state;
			m_command.push_back(*i);
			break;
		}
		case number:
		{
			currentState.state = parserStates::num_state;
			m_command.push_back(*i);
			break;
		}
		case variable:
		case argument:
		{
			currentState.state = parserStates::var_state;
			m_command.push_back(*i);
			break;
		}
		case function:
		{
			currentState.state = parserStates::func_state;
			m_command.push_back(*i);
			break;
		}
		case identifire:
		{
			currentState.identifireName = (*i)->nameOrValue;
			currentState.state = parserStates::identifire_state;
			break;
		}
		case closing_bracket:
		{
			currentState.state = parserStates::cl_bracket_state;
			m_command.push_back(*i);
			break;
		}
		default:
			throw ExceptionError(" Can't be " + (*i)->nameOrValue + " after opening bracket");
		}
	}
}

void Compiler::Parser::cultivateClosingBracketState(const std::vector<TokenInf*>::const_iterator& i) 
{
	if (currentState.type == function && Tree::bracketValidator(m_command) == 1) {
		m_command.pop_back();
		cultivateToStateComma();  
		FuncTokenInfPtr temp = new FuncTokenInf();
		temp->e_tokType = function;
		currentState = stateStack.top();
		stateStack.pop();
		temp->nameOrValue = currentState.identifireName;
		temp->args = currentState.args;
		m_command = currentState.commandTokens;
		m_command.push_back(temp);
	}
	switch ((*i)->e_tokType) {
		case operation:
		{
			currentState.state = parserStates::operations_state;
			m_command.push_back(*i);
			break;
		}
		case closing_bracket:
		{
			currentState.state = parserStates::cl_bracket_state;
			m_command.push_back(*i);
			break;
		}
		case comma:
		{
			cultivateToStateComma();
			break;
		}
		case semicolon:
		{
			cultivateSemicolonState();
			break;
		}
		default:
		{
			throw ExceptionError("After closing bracket used incorrect token");
		}
	}
}

void Compiler::Parser::cultivateVarFuncStates(const std::vector<TokenInf*>::const_iterator& i) {
	if (!b_expression_part) {
		if ((*i)->e_tokType == assignment) {
			m_command.push_back(*i);
			currentState.state = parserStates::expression_state;
			b_expression_part = true;
		}
		else {
			throw ExceptionError("Expected '=' after " + (*i)->nameOrValue + "\'s declaration");
		}
	}
	else {
		switch ((*i)->e_tokType)
		{
		case operation:
		{
			currentState.state = parserStates::operations_state;
			m_command.push_back(*i);
			break;
		}
		case closing_bracket:
		{
			currentState.state = parserStates::cl_bracket_state;
			m_command.push_back(*i);
			break;
		}
		case comma:
		{
			cultivateToStateComma();
			break;
		}
		case semicolon:
		{
			cultivateSemicolonState();
			break;
		}
		default:
			throw ExceptionError("Used incorrect token after indentifire " + (*i)->nameOrValue);
		}
	}
}

void Compiler::Parser::cultivateIdentifireState(std::vector<TokenInfPtr>::iterator& i)
{
	switch ((*i)->e_tokType) {
	case opening_bracket:
	{
		cultivateFromIdentifireToOpeningBracket();
		break;
	}
	case operation:
	{
		cultivateFromIdentifireToOperation(i);
		break;
	}
	case closing_bracket:         
	{
		cultivateFromIdentifireToClosingBracket(i);
		break;
	}
	case comma:               
	{
		cultivateFromIdentifireToComma();
		break;
	}
	case semicolon:
	{
		cultivateFromIdentifireToSemicolon();
		break;
	}
	default:
		throw ExceptionError("Used incorrect token after number");
	}
}

void Compiler::Parser::cultivateNumberState(const std::vector<TokenInf*>::const_iterator& i) {
	switch ((*i)->e_tokType)
	{
	case operation:
	{
		currentState.state = parserStates::operations_state;
		m_command.push_back(*i);
		break;
	}
	case closing_bracket:
	{
		currentState.state = parserStates::cl_bracket_state;
		m_command.push_back(*i);
		break;
	}
	case comma:
	{
		cultivateToStateComma();
		break;
	}
	case semicolon:
	{
		cultivateSemicolonState();
		break;
	}
	default:
		throw ExceptionError("Used incorrect token after number");
	}

}

void Compiler::Parser::cultivateFromIdentifireToOpeningBracket()
{
	if (currentState.args.size() == 0) {
		currentState.commandTokens = m_command;
		currentState.type = tokenType::function;
		stateStack.push(currentState);
	}
	else {
		currentState.type = tokenType::function;
		currentState.commandTokens = m_command;
		currentState.args = {};
		stateStack.push(currentState);
	}
	currentState.state = parserStates::expression_state;
	m_command.clear();
}

void Compiler::Parser::cultivateFromIdentifireToOperation(std::vector<TokenInfPtr>::iterator& i)
{
	TokenInfPtr temp = new TokenInf;
	if (funcName != "") {
		std::vector<std::string>& funcArgs = var_or_func_map.find(funcName)->second.arguments;
		if ((find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) == funcArgs.end()) && (var_or_func_map.find(currentState.identifireName) == var_or_func_map.end())) {
			throw ExceptionError(currentState.identifireName + "  was not defined");
		}
		if (find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) != funcArgs.end()) {
			temp->e_tokType = argument;
		}
		else {
			temp->e_tokType = variable;
		}
	}
	else {
		if (var_or_func_map.find(temp->nameOrValue) == var_or_func_map.end()) {
			throw ExceptionError(temp->nameOrValue + "  was not defined");
		}
		temp->e_tokType = variable;
	}
	temp->nameOrValue = currentState.identifireName;
	m_command.push_back(temp);
	m_command.push_back(*i);
	currentState.state = parserStates::operations_state;
}

void Compiler::Parser::cultivateFromIdentifireToClosingBracket(std::vector<TokenInfPtr>::iterator& i)
{
	TokenInfPtr temp = new TokenInf;
	temp->nameOrValue = currentState.identifireName;
	if (funcName != "") {
		std::vector<std::string>& funcArgs = var_or_func_map.find(funcName)->second.arguments;
		if ((find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) == funcArgs.end()) && (var_or_func_map.find(currentState.identifireName) == var_or_func_map.end())) {
			throw ExceptionError(currentState.identifireName + "  was not defined");
		}
		if (find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) != funcArgs.end()) {
			temp->e_tokType = argument;
		}
		else {
			temp->e_tokType = variable;
		}
	}
	else {
		if (var_or_func_map.find(temp->nameOrValue) == var_or_func_map.end()) {
			throw ExceptionError(temp->nameOrValue + "  was not defined");
		}
		temp->e_tokType = variable;
	}
	m_command.push_back(temp);
	m_command.push_back(*i);
	currentState.state = parserStates::cl_bracket_state;
}

void Compiler::Parser::cultivateFromIdentifireToComma()
{
	TokenInfPtr temp = new TokenInf;
	temp->nameOrValue = currentState.identifireName;
	if (funcName != "") {
		std::vector<std::string>& funcArgs = var_or_func_map.find(funcName)->second.arguments;
		if ((find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) == funcArgs.end()) && (var_or_func_map.find(currentState.identifireName) == var_or_func_map.end())) {
			throw ExceptionError(currentState.identifireName + "  was not defined");
		}
		if (find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) != funcArgs.end()) {
			temp->e_tokType = argument;
		}
		else {
			temp->e_tokType = variable;
		}
	}
	else {
		if (var_or_func_map.find(temp->nameOrValue) == var_or_func_map.end()) {
			throw ExceptionError(temp->nameOrValue + "  was not defined");
		}
		temp->e_tokType = variable;
	}
	m_command.push_back(temp);
	cultivateToStateComma();
}

void Compiler::Parser::cultivateFromIdentifireToSemicolon()
{
	TokenInfPtr temp = new TokenInf;
	if (funcName != "") {
		std::vector<std::string>& funcArgs = var_or_func_map.find(funcName)->second.arguments;
		if ((find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) == funcArgs.end()) && (var_or_func_map.find(currentState.identifireName) == var_or_func_map.end())) {
			throw ExceptionError(currentState.identifireName + "  was not defined");
		}
		if (find(funcArgs.begin(), funcArgs.end(), currentState.identifireName) != funcArgs.end()) {
			temp->e_tokType = argument;
		}
		else {
			temp->e_tokType = variable;
		}
	}
	else {
		if (var_or_func_map.find(temp->nameOrValue) == var_or_func_map.end()) {
			throw ExceptionError(temp->nameOrValue + "  was not defined");
		}
		temp->e_tokType = variable;
	}
	temp->nameOrValue = currentState.identifireName;
	m_command.push_back(temp);
	cultivateSemicolonState();
}

void Compiler::Parser::cultivateToStateComma()
{
	if (currentState.type == tokenType::function) {
		if (m_command.size() != 0) {
			expression_tree.setExpression(m_command);
			stateStack.top().args.push_back(expression_tree.root());
		}
		currentState.state = parserStates::expression_state;
	}
	else {
		throw  ExceptionError("Comma can be used only in function declarations and definitians");
	}
	m_command.clear();
}

void Compiler::Parser::cultivateSemicolonState()
{
	b_expression_part = false;
	if (Tree::bracketValidator(m_command) != 0) {
		throw ExceptionError(" Uncorrect brackets");
	}
	if (m_command.size() != 0) {
		expression_tree.setExpression(m_command);
	}
	CodeGenerator codeGenerator(expression_tree, var_or_func_map);
	m_command.clear();
	currentState.state = parserStates::initial_state;
	currentState.args = {};
	currentState.commandTokens.clear();
	funcName = "";
}
