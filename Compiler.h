#ifndef COMPILER_H
#define COMPILER_H

#include "declarations.h"

class Compiler {
public:
	Compiler(std::string funcFile, std::string assemblyFile);
	~Compiler();
	void compile();
private:

	class Tokenizer {
	private:
		std::vector<TokenInfPtr> m_tokenPool;
		tokenizerState m_currentState;
		std::string m_token;
		bool b_firstInLine;
		bool b_funcvarOrNumber;
		bool b_varOrFunc;
		bool b_expressionPart;
		std::string funcName;
		std::map<std::string, func_map_inf>& varFuncMap;
	public:
		//cultivatestate

		void cultivateInitialState(char symbol);
		void cultivateDigitState(char symbol);
		void cultivateIndentifireState(char symbol);
		void cultivateFunctionState(char symbol);
		void cultivateOperationBracketCommaAssignState(char symbol);

		//from_init_to
		void cultivateFromInitToOpBracket();
		void cultivateFromInitToClBracket();
		void cultivateFromInitToAssign();
		void cultivateFromInitToFinal();
		void cultivate_from_init_to_comma();
		void cultivate_from_init_to_operation(const char&);
		void cultivate_from_init_to_digit(const char&);
		void cultivate_from_init_to_indentifire(const char&);

		//from_digit_to	
		void cultivate_from_digit_to_init();
		void cultivate_from_digit_to_op_bracket();
		void cultivate_from_digit_to_cl_bracket();
		void cultivate_from_digit_to_assign();
		void cultivate_from_digit_to_final();
		void cultivate_from_digit_to_comma();
		void cultivate_from_digit_to_operation(const char&);
		void cultivate_from_digit_to_digit(const char&);

		//from_indentifire_to
		void cultivate_from_indentifire_to_init();
		void cultivate_from_indentifire_to_op_bracket();
		void cultivate_from_indentifire_to_cl_bracket();
		void cultivate_from_indentifire_to_assign();
		void cultivate_from_indentifire_to_final();
		void cultivate_from_indentifire_to_comma();
		void cultivate_from_indentifire_to_operation(const char&);
		void cultivate_from_indentifire_to_indentifire(const char&);

		//from_func_to
		void cultivate_from_func_to_cl_bracket(std::string funcName);
		void cultivate_in_func_to_comma(std::string funcName);
		//from_others_to
		void cultivate_to_op_bracket();
		void cultivate_to_cl_bracket();
		void cultivate_to_assign();
		void cultivate_to_final();
		void cultivate_to_comma();
		void cultivate_to_operation(const char& symbol);
		void cultivate_to_indentifire(const char& symbol);
		void cultivate_to_digit(const char & symbol);

	public:
		Tokenizer(std::map<std::string, func_map_inf>& varfunc_map);
		void tokenize(std::ifstream& func_file);
		std::vector<TokenInf*>& getTokenPool() { return m_tokenPool; };
	};

	class Tree
	{
	private:
		std::deque<TokenInfPtr> m_expression;
		std::stack<Node*> Expression_Tree;
		int getPrioritet(const char &ch) const;
		void infixToExpressionTree();
		Node* m_root;
	public:
		Node * root() {
			return m_root;
		}
		static bool isOperator(const char& symbol);
		static int bracketValidator(std::deque<TokenInfPtr>);
		void setExpression(const std::deque<TokenInfPtr> &expression);
	};

	class Parser {

	private:

		TokenInfPtr m_tokenForExpressin;
		std::deque<TokenInfPtr> m_command;
		std::map<std::string, func_map_inf>& var_or_func_map;
		std::vector<TokenInfPtr>& tokensPool;
		ParserStateStruct currentState;
		std::stack<ParserStateStruct> stateStack;
		std::string funcName;
		Tree expression_tree;
		bool b_expression_part;
	public:
		Parser(std::map<std::string, func_map_inf>& var_func, std::vector<TokenInfPtr>& t_pool);
		void parse();

		void cultivateInitialState(const std::vector<TokenInfPtr>::const_iterator&);
		void cultivateVarFuncStates(const std::vector<TokenInfPtr>::const_iterator& i);
		void cultivateIdentifireState(std::vector<TokenInfPtr>::iterator  &);
		void cultivateClosingBracketState(const std::vector<TokenInfPtr>::const_iterator& i);
		void cultivateNumberState(const std::vector<TokenInfPtr>::const_iterator& );

		void cultivateExpressionState(std::vector<TokenInfPtr>::iterator &);
		void cultivateOperationsState(std::vector<TokenInfPtr>::iterator& i);
		void cultivateOpeningBracketState(std::vector<TokenInfPtr>::iterator& i);

		//cultivate_from_identifire_to

		void cultivateFromIdentifireToOpeningBracket();
		void cultivateFromIdentifireToOperation(std::vector<TokenInfPtr>::iterator& i);
		void cultivateFromIdentifireToClosingBracket(std::vector<TokenInfPtr>::iterator& i);
		void cultivateFromIdentifireToComma();
		void cultivateFromIdentifireToSemicolon();

		//cultivate_to_state
		void cultivateToStateComma();

		void cultivateSemicolonState();
		void addZero();
		void checkValidation(std::vector<TokenInfPtr>::const_iterator& i);
	};

	class CodeGenerator {
	public:
		static std::deque<std::string> m_defineBuf;
		static std::deque<std::string> m_codeBuf;
		static std::deque<std::string> m_dataBuf;
		std::map<std::string, func_map_inf>& m_varFuncMap;
		CodeGenerator(Tree& expression_tree, std::map<std::string, func_map_inf>&);
		void GenerateFromTree(Node*, std::deque<std::string>& code_buf);
		void GenerateAssemblyCode(Node*, std::deque<std::string>& code_buf);
		void generateFunctionCode(Tree& expression_tree);
		void generateVariableCode(Tree& expression_tree);
	private:
		std::string generateOperationCode(const char &operation) const;
		void generateOnlyArgumentCode(TokenInfPtr& rightToken, Tree& expression_tree);
		void generateOnlyFunctionCallCode(TokenInfPtr& rightToken, Tree& expression_tree);
		void generateRightFunctionCallCode(TokenInfPtr& rightToken, Tree& expression_tree);

		void generateLeafCode(Node* tree_node, std::deque<std::string>& m_code_buf);
		void generateLeftOperandCode(TokenInfPtr& leftOperand, TokenInfPtr& rightOperand, std::deque<std::string>& m_code_buf);
		void generateRightOperandCode(TokenInfPtr& rightOperand, std::deque<std::string>& m_code_buf);

		bool R0_is_bizy;
		bool R0_is_pushed;
		std::string funcName;
	};
	void output();
	std::ifstream m_funcFile;
	std::ofstream m_assemblyFile;
	std::map<std::string, func_map_inf> m_varFuncMap;
public:
	Tokenizer tokenizer;
};

#endif

