#include <iostream>
#include <ostream>
#include <list>

using namespace std;

#pragma region Define Structs
struct variable {
	string name;
	string value;
};

struct block {
	int id;
	string name;
	list<variable> variables;
};
#pragma endregion

#pragma region Print Variable Function
void printVariable(variable const& var) {
	cout << "Name: " << var.name << '\n';
	cout << "Value: " << var.value << '\n';
}
#pragma endregion

#pragma region Print Block Function
int printBlock(block block) {
	cout << "Id: " << block.id << '\n';
	cout << "Nome: " << block.name << '\n';
	cout << "Variaveis: \n" << block.variables.size() << '\n';
	for (auto it = block.variables.cbegin(); it != block.variables.cend(); it++) {
		printVariable(*it);
	}


	return 1;
}
#pragma endregion

int main() {
	// Initialize variables
	variable var1 = { "Input1", "[1213213,123123,123213]" };
	variable var2 = { "Input2", "[121,23213]" };

	// Initialize variable list
	list<variable> varList;
	varList.push_back(var1);
	varList.push_back(var2);

	// Initialize a block
	block bloco = {
		1,
		"VGL_Convolution1",
		varList
	};

	// Print the content of a block in console
	printBlock(bloco);
}

