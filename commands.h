#include <vector>
#include <string>

void executeExternalProgram(std::vector<std::string> commands);
void changeDirectory(std::vector<std::string> arguments);
void printWorkingDirectory(std::vector<std::string> arguments);
void exitShell(std::vector<std::string> arguments);
void addAddressToPath(std::vector<std::string> arguments);
void showPath(std::vector<std::string> arguments);