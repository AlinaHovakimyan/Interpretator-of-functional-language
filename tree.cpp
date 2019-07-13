#include "Compiler.h"

Node::Node() {
	value = new TokenInf();
	left = NULL;
	right = NULL;
	parent = NULL;
}

Node::Node(TokenInfPtr val)
	: value(val)
{
	left = NULL;
	right = NULL;
	parent = NULL;
}

FunctionNode::FunctionNode(TokenInfPtr val) :
	Node(val)
{}

void Compiler::Tree::setExpression(const std::deque<TokenInfPtr> &expression)
{
		m_expression = expression;
		infixToExpressionTree();
		m_root = Expression_Tree.top();
		Expression_Tree.pop();
}
bool Compiler::Tree::isOperator(const char &symbol)
{
	switch (symbol)
	{
	case '+':
	case '-':
	case '*':
	case '/':
		return true;
	}

	return false;
}

int Compiler::Tree::bracketValidator(std::deque<TokenInfPtr> expression)
{
	int retVal = 0;
	std::stack<char> stack;

	for (size_t i = 0; i < expression.size(); i++)
	{
		if (expression[i]->e_tokType == opening_bracket)
			stack.push('(');

		if (expression[i]->e_tokType == closing_bracket)
		{
			if (!stack.empty())
			{
				stack.pop();
			}
			else
			{
				return 1;
			}
		}
	}

	if (!stack.empty())
		return -1;

	return 0;
}

int Compiler::Tree::getPrioritet(const char &ch) const
{
	switch (ch)
	{
	case '=':
		return 1;
	case '+':
	case '-':
		return 2;
	case '*':
	case '/':
		return 3;
	}

	return -1;
}

void Compiler::Tree::infixToExpressionTree()
{
	std::stack<TokenInfPtr> operation_stack;
	Node* operand;
	Node* left_operand;
	Node* right_operand;

	for (size_t i = 0; i < m_expression.size(); i++)
	{
		if (m_expression[i]->e_tokType == number || m_expression[i]->e_tokType == variable || m_expression[i]->e_tokType == argument)
		{
			operand = new Node(m_expression[i]);
			Expression_Tree.push(operand);
		}
		else if (m_expression[i]->e_tokType == function) {
			operand = new FunctionNode(m_expression[i]);
			Expression_Tree.push(operand);
		}
		else if (m_expression[i]->e_tokType == operation || m_expression[i]->e_tokType == assignment)
		{
			while (
				(!operation_stack.empty()) &&
				(operation_stack.top()->nameOrValue != "(") &&
				(getPrioritet(m_expression[i]->nameOrValue.at(0)) <= getPrioritet(operation_stack.top()->nameOrValue.at(0)))
				)
			{
				right_operand = Expression_Tree.top();
				Expression_Tree.pop();
				left_operand = Expression_Tree.top();
				Expression_Tree.pop();
				operand = new Node(operation_stack.top());
				operation_stack.pop();
				operand->left = left_operand;
				left_operand->parent = operand;
				operand->right = right_operand;
				right_operand->parent = operand;
				Expression_Tree.push(operand);
			}

			operation_stack.push(m_expression[i]);
		}
		else if (m_expression[i]->e_tokType == closing_bracket)
		{
			while ((!operation_stack.empty()) && (operation_stack.top()->nameOrValue != "("))
			{
				right_operand = Expression_Tree.top();
				Expression_Tree.pop();
				left_operand = Expression_Tree.top();
				Expression_Tree.pop();
				operand = new Node(operation_stack.top());
				operation_stack.pop();
				operand->left = left_operand;
				left_operand->parent = operand;
				operand->right = right_operand;
				right_operand->parent = operand;
				Expression_Tree.push(operand);
			}
		}
		else if (m_expression[i]->e_tokType == opening_bracket)
		{
			operation_stack.push(m_expression[i]);
		}
	}

	while (!operation_stack.empty())
	{
		if (operation_stack.top()->nameOrValue != "(")
		{
			right_operand = Expression_Tree.top();
			Expression_Tree.pop();
			left_operand = Expression_Tree.top();
			Expression_Tree.pop();
			operand = new Node(operation_stack.top());
			operation_stack.pop();
			operand->left = left_operand;
			left_operand->parent = operand;
			operand->right = right_operand;
			right_operand->parent = operand;
			Expression_Tree.push(operand);
		}
		else {
			operation_stack.pop();
		}
	}
}
