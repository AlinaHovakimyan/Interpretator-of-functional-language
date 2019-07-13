#include "Compiler.h"
#include <iostream>


int main() {
	try {
		std::string func_file;
		std::string assembly_file;
		std::cout << "Please enter file name, that you want to compile" << std::endl;
		std::cin >> func_file;
		std::cout << "Please enter generated assembly file name" << std::endl;
		std::cin >> assembly_file;
		Compiler assembly_compiler(func_file, assembly_file);
		assembly_compiler.compile();
	}
	catch (ExceptionError exception) {
		std::cout << "Error: " << exception.message() << std::endl;
	}
	return 0;
}