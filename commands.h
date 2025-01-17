#include <vector>
#include <string>
#include "utils.h"

void changeDirectory(std::vector<std::string> arguments);
void printWorkingDirectory(std::vector<std::string> arguments);
void exitShell(std::vector<std::string> arguments);
void addAddressToPath(std::string address);
void showPath(std::vector<std::string> arguments);
bool checkIfFileExists(char* fileName);
int tryBuiltins(std::vector<std::string> commands);
void executeExternalProgram(std::vector<std::string> commands);