#include "Compiler.h"

Compiler::CodeGenerator::CodeGenerator(Tree& expression_tree, std::map<std::string, func_map_inf>& var_fanc_map)
	: m_varFuncMap(var_fanc_map)
{
	R0_is_bizy = false;
	R0_is_pushed = false;
	if (expression_tree.root()->left->value->e_tokType == function) {
		generateFunctionCode(expression_tree);
	}
	else if (expression_tree.root()->left->value->e_tokType == variable) {
		generateVariableCode(expression_tree);
	}
}
void Compiler::CodeGenerator::generateFunctionCode(Tree& expression_tree) {
	funcName = expression_tree.root()->left->value->nameOrValue;
	m_defineBuf.push_back(expression_tree.root()->left->value->nameOrValue + ":\n");
	m_defineBuf.push_back("\tPUSH SF\n");
	TokenInfPtr rightToken = expression_tree.root()->right->value;

	switch (rightToken->e_tokType) {
	case number:
	{
		m_defineBuf.push_back("\tASSIGN R0 " + rightToken->nameOrValue + "\n");    //in case variable = number;
		break;
	}
	case variable:
	{
		m_defineBuf.push_back("\tASSIGN A2 " + rightToken->nameOrValue + "\n");
		m_defineBuf.push_back("\tLOAD R0 A2\n");
		break;
	}
	case argument:
	{
		generateOnlyArgumentCode(rightToken, expression_tree);
		break;
	}
	case function:
	{
		generateOnlyFunctionCallCode(rightToken, expression_tree);
		break;
	}
	default:
		GenerateFromTree(expression_tree.root()->right, m_defineBuf);
	}
	m_defineBuf.push_back("\tPOP SF\n");
	m_defineBuf.push_back("\tRET\n");

}

void Compiler::CodeGenerator::generateVariableCode(Tree& expression_tree) {
	TokenInfPtr rightToken = expression_tree.root()->right->value;
	std::map<std::string, func_map_inf>::iterator index = m_varFuncMap.find(expression_tree.root()->left->value->nameOrValue);
	if (index != m_varFuncMap.end() && index->second.b_isFunc == false) {
		m_dataBuf.push_back("\tDWORD " + expression_tree.root()->left->value->nameOrValue);
		index->second.b_isFunc = true;
		switch (rightToken->e_tokType) {
		case number:
		{
			m_dataBuf.push_back("=" + rightToken->nameOrValue + "\n");     //in case variable = number;
			break;
		}
		case function:
		{
			generateRightFunctionCallCode(rightToken, expression_tree);
			break;
		}
		case variable:
		{
			m_codeBuf.push_back("\tASSIGN A2, " + rightToken->nameOrValue + "\n");
			m_codeBuf.push_back("\tLOAD R0, A2\n");
			m_codeBuf.push_back("\tASSIGN A2 " + expression_tree.root()->left->value->nameOrValue + "\n");
			m_codeBuf.push_back("\tSTORE R0, A2\n");
			break;
		}
		default:
		{
			m_dataBuf.push_back("=0\n");
			std::string var_name = expression_tree.root()->left->value->nameOrValue;
			GenerateFromTree(expression_tree.root()->right, m_codeBuf);
			m_codeBuf.push_back("\tASSIGN A2, " + var_name + "\n");
			m_codeBuf.push_back("\tSTORE R0, A2\n");
			break;
		}
		}
	}
	else if (rightToken->e_tokType == number) {
		m_codeBuf.push_back("\tASSIGN A2, " + expression_tree.root()->left->value->nameOrValue + "\n");
		m_codeBuf.push_back("\tASSIGN R0, " + rightToken->nameOrValue + "\n");
		m_codeBuf.push_back("\tSTORE R0, A2\n");
	}
}

void Compiler::CodeGenerator::GenerateAssemblyCode(Node* tree_node, std::deque<std::string>& m_code_buf) {
	TokenInfPtr leftOperand, rightOperand;
	Node* operation_Node;
	if (tree_node->left != nullptr && Compiler::Tree::isOperator(tree_node->left->value->nameOrValue[0])) {
		GenerateAssemblyCode(tree_node->left, m_code_buf);
	}
	if (tree_node->right != nullptr && Compiler::Tree::isOperator(tree_node->right->value->nameOrValue[0])) {
		GenerateAssemblyCode(tree_node->right, m_code_buf);
	}
	leftOperand = tree_node->left ? tree_node->left->value : nullptr;
	rightOperand = tree_node->right ? tree_node->right->value : nullptr;
	if (!leftOperand && !rightOperand) {
		generateLeafCode(tree_node, m_code_buf);
		return;
	}
	operation_Node = tree_node;
	if (R0_is_bizy) {
		if (leftOperand->nameOrValue != "R0" && rightOperand->nameOrValue != "R0") {
			m_code_buf.push_back("\tPUSH R0\n");
			R0_is_pushed = true;
		}
		else if (R0_is_pushed) {
			if (leftOperand->nameOrValue == "R0" && rightOperand->nameOrValue == "R0") {
				m_code_buf.push_back("\tMOVE R4, R0\n");
			}
			m_code_buf.push_back("\tPOP R0\n");
			R0_is_pushed = false;
		}
	}
	if (leftOperand) {
		generateLeftOperandCode(leftOperand, rightOperand, m_code_buf);
	}
	if (rightOperand) {
		generateRightOperandCode(rightOperand, m_code_buf);
	}
	m_code_buf.push_back("\t" + generateOperationCode(operation_Node->value->nameOrValue[0]) + "\n");
	TokenInfPtr tok_of_reg = new TokenInf();
	if (operation_Node->parent != nullptr) {
		tok_of_reg->e_tokType = variable;
		tok_of_reg->nameOrValue = "R0";
		Node* new_node = new Node(tok_of_reg);
		operation_Node->left->~Node();
		operation_Node->right->~Node();
		if (operation_Node->parent->left == operation_Node) {
			operation_Node->parent->left = new_node;
			new_node->parent = operation_Node->parent;
			tree_node = new_node->parent->right;
		}
		else {
			operation_Node->parent->right = new_node;
			new_node->parent = operation_Node->parent;
			tree_node = new_node->parent->right;
		}
	}
	else {
		tree_node = NULL;
	}
	operation_Node->~Node();
	R0_is_bizy = true;
}

void Compiler::CodeGenerator::GenerateFromTree(Node* root, std::deque<std::string>&code_buf)
{
	R0_is_bizy = false;
	R0_is_pushed = false;
	GenerateAssemblyCode(root, code_buf);
}

std::string Compiler::CodeGenerator::generateOperationCode(const char &operation) const
{
	switch (operation)
	{
	case '+':
	{
		return "ADD R0, R4, R0";
	}
	case '-':
		return "SUB R0, R4, R0";
	case '*':
		return "MUL R0, R4, R0";
	case '/':
		return "DIV R0, R4, R0";
	}
	return "";
}

void Compiler::CodeGenerator::generateOnlyArgumentCode(TokenInfPtr& rightToken, Tree& expression_tree)
{
	std::vector<std::string>& funcArgs = m_varFuncMap.find(expression_tree.root()->left->value->nameOrValue)->second.arguments;
	m_defineBuf.push_back("\tMOVE R8, SF\n");
	size_t offset = funcArgs.size() - (find(funcArgs.begin(), funcArgs.end(), rightToken->nameOrValue) - funcArgs.begin()) - 1;
	std::string offsetString = std::to_string((offset + 1) * 4 + 8);
	m_defineBuf.push_back("\tASSIGN R12 " + offsetString + "\n");
	m_defineBuf.push_back("\tADD R8, R12, R8\n");
	m_defineBuf.push_back("\tMOVE A3, R8\n");
	m_defineBuf.push_back("\tLOAD R0, A3\n");
}

void Compiler::CodeGenerator::generateOnlyFunctionCallCode(TokenInfPtr& rightToken, Tree& expression_tree)
{
	std::vector<Node*>& funcArgs = dynamic_cast<FuncTokenInf*>(rightToken)->args;
	for (int i = 0; i < funcArgs.size(); ++i) {
		GenerateAssemblyCode(funcArgs[i], m_defineBuf);
		m_defineBuf.push_back("\tPUSH R0\n");
		R0_is_bizy = false;
	}
	m_defineBuf.push_back("\tASSIGN A2 " + rightToken->nameOrValue + "\n");
	m_defineBuf.push_back("\tCALL A2\n");
	size_t SPOffset = m_varFuncMap.find(rightToken->nameOrValue)->second.argCount * 4;
	std::string SPOffsetString = std::to_string(SPOffset);
	m_defineBuf.push_back("\tADD SP, " + SPOffsetString + "\n");
}

void Compiler::CodeGenerator::generateRightFunctionCallCode(TokenInfPtr& rightToken, Tree& expression_tree)
{
	m_dataBuf.push_back("=0\n");
	std::vector<Node*>& funcArgs = dynamic_cast<FuncTokenInf*>(rightToken)->args;
	for (int i = 0; i < funcArgs.size(); ++i) {
		GenerateAssemblyCode(funcArgs[i], m_codeBuf);
		m_codeBuf.push_back("\tPUSH R0\n");
		R0_is_bizy = false;
	}
	m_codeBuf.push_back("\tASSIGN A2 " + rightToken->nameOrValue + "\n");
	m_codeBuf.push_back("\tCALL A2\n");
	size_t SPOffset = m_varFuncMap.find(rightToken->nameOrValue)->second.argCount * 4;
	if (SPOffset != 0) {
		std::string SPOffsetString = std::to_string(SPOffset);
		m_codeBuf.push_back("\tADD SP, " + SPOffsetString + "\n");
	}
	m_codeBuf.push_back("\tASSIGN A2 " + expression_tree.root()->left->value->nameOrValue + "\n");
	m_codeBuf.push_back("\tSTORE R0, A2\n");
}

void Compiler::CodeGenerator::generateLeafCode(Node* tree_node, std::deque<std::string>& m_code_buf)
{
	switch (tree_node->value->e_tokType) {
	case function:
	{
		std::vector<Node*>& funcArgs = dynamic_cast<FuncTokenInf*>(tree_node->value)->args;
		for (int i = 0; i < funcArgs.size(); ++i) {
			GenerateAssemblyCode(funcArgs[i], m_code_buf);
			m_code_buf.push_back("\tPUSH R0\n");
			R0_is_bizy = false;
		}
		m_code_buf.push_back("\tASSIGN A2 " + tree_node->value->nameOrValue + "\n");
		m_code_buf.push_back("\tCALL A2\n");
		size_t SPOffset = m_varFuncMap.find(tree_node->value->nameOrValue)->second.argCount * 4;
		if (SPOffset != 0) {
			std::string SPOffsetString = std::to_string(SPOffset);
			m_code_buf.push_back("\tADD SP, " + SPOffsetString + "\n");
		}
		break;
	}
	case argument:
	{
		std::vector<std::string>& funcArgs = m_varFuncMap.find(funcName)->second.arguments;
		m_code_buf.push_back("\tMOVE R8, SF\n");
		size_t offset = funcArgs.size() - (find(funcArgs.begin(), funcArgs.end(), tree_node->value->nameOrValue) - funcArgs.begin()) - 1;
		std::string offsetString = std::to_string((offset + 1) * wordSize + 2 * wordSize);
		m_code_buf.push_back("\tASSIGN R12 " + offsetString + "\n");
		m_code_buf.push_back("\tADD R8, R12, R8\n");
		m_code_buf.push_back("\tMOVE A3 R8\n");
		m_code_buf.push_back("\tLOAD R0, A3\n");
		break;
	}
	case number:
	{
		m_code_buf.push_back("\tASSIGN R0 " + tree_node->value->nameOrValue + "\n");
		break;
	}
	}
}

void Compiler::CodeGenerator::generateLeftOperandCode(TokenInfPtr& leftOperand, TokenInfPtr& rightOperand, std::deque<std::string>& m_code_buf)
{
	switch (leftOperand->e_tokType) {
	case function:
	{
		std::vector<Node*>& funcArgs = dynamic_cast<FuncTokenInf*>(leftOperand)->args;
		for (int i = 0; i < funcArgs.size(); ++i) {
			GenerateAssemblyCode(funcArgs[i], m_code_buf);
			m_code_buf.push_back("\tPUSH R0\n");
			R0_is_bizy = false;
		}
		m_defineBuf.push_back("\tASSIGN A2 " + leftOperand->nameOrValue + "\n");
		m_defineBuf.push_back("\tCALL A2\n");
		size_t SPOffset = m_varFuncMap.find(leftOperand->nameOrValue)->second.argCount * 4;
		if (SPOffset != 0) {
			std::string SPOffsetString = std::to_string(SPOffset);
			m_defineBuf.push_back("\tADD SP, " + SPOffsetString + "\n");
		}
		break;
	}
	case variable:
	{
		if (leftOperand->nameOrValue != "R0") {
			m_code_buf.push_back("\tASSIGN A2, " + leftOperand->nameOrValue + "\n");
			m_code_buf.push_back("\tLOAD R0, A2\n");
		}
		break;
	}
	case number:
	{
		if (rightOperand->nameOrValue == "R0") {
			m_code_buf.push_back("\tMOVE R4, R0\n");
			R0_is_bizy = false;
		}
		m_code_buf.push_back("\tASSIGN R0, " + leftOperand->nameOrValue + "\n");
		break;
	}
	case argument:
	{
		std::vector<std::string>& funcArgs = m_varFuncMap.find(funcName)->second.arguments;
		m_code_buf.push_back("\tMOVE R8, SF\n");
		size_t offset = funcArgs.size() - (find(funcArgs.begin(), funcArgs.end(), leftOperand->nameOrValue) - funcArgs.begin()) - 1;
		std::string offsetString = std::to_string((offset + 1) * 4 + 8);
		m_code_buf.push_back("\tASSIGN R12 " + offsetString + "\n");
		m_code_buf.push_back("\tADD R8, R12, R8\n");
		m_code_buf.push_back("\tMOVE A3 R8\n");
		m_code_buf.push_back("\tLOAD R0, A3\n");
		break;
	}
	}
}

void Compiler::CodeGenerator::generateRightOperandCode(TokenInfPtr & rightOperand, std::deque<std::string>& m_code_buf)
{
	switch (rightOperand->e_tokType) {
	case function:
	{
		m_code_buf.push_back("\tPUSH R0\n");
		std::vector<Node*>& funcArgs = dynamic_cast<FuncTokenInf*>(rightOperand)->args;
		for (int i = 0; i < funcArgs.size(); ++i) {
			GenerateAssemblyCode(funcArgs[i], m_code_buf);
			m_code_buf.push_back("\tPUSH R0\n");
			R0_is_bizy = false;
		}
		m_code_buf.push_back("\tASSIGN A2 " + rightOperand->nameOrValue + "\n");
		m_code_buf.push_back("\tCALL A2\n");
		size_t SPOffset = m_varFuncMap.find(rightOperand->nameOrValue)->second.argCount * 4;
		if (SPOffset != 0) {
			std::string SPOffsetString = std::to_string(SPOffset);
			m_defineBuf.push_back("\tADD SP, " + SPOffsetString + "\n");
		}
		m_code_buf.push_back("\tMOVE R4, R0\n");
		m_code_buf.push_back("\tPOP R0\n");
		break;
	}
	case variable:
	{
		if (rightOperand->nameOrValue != "R0") {
			m_code_buf.push_back("\tASSIGN A2, " + rightOperand->nameOrValue + "\n");
			m_code_buf.push_back("\tLOAD R4, A2\n");
		}
		break;
	}
	case number:
	{
		m_code_buf.push_back("\tASSIGN R4, " + rightOperand->nameOrValue + "\n");
		break;
	}
	case argument:
	{
		std::vector<std::string>& funcArgs = m_varFuncMap.find(funcName)->second.arguments;
		m_code_buf.push_back("\tMOVE R8, SF\n");
		size_t offset = funcArgs.size() - (find(funcArgs.begin(), funcArgs.end(), rightOperand->nameOrValue) - funcArgs.begin()) - 1;
		std::string offsetString = std::to_string((offset + 1) * 4 + 8);
		m_code_buf.push_back("\tASSIGN R12 " + offsetString + "\n");
		m_code_buf.push_back("\tADD R8, R12, R8\n");
		m_code_buf.push_back("\tMOVE A3 R8\n");
		m_code_buf.push_back("\tLOAD R4, A3\n");
		break;
	}
	}
}
