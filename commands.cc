#include <string>
#include <vector>
#include <stdio.h>

void executeExternalProgram(std::vector<std::string> commands)
{
    printf("executing external command");
}

void changeDirectory(std::vector<std::string> arguments)
{
    printf("changing directory");
}

void printWorkingDirectory(std::vector<std::string> arguments)
{
    printf("printing working directory");
}

void exitShell(std::vector<std::string> arguments)
{
    printf("exiting shell");
}

void addAddressToPath(std::vector<std::string> arguments)
{
    printf("adding address to path");
}

void showPath(std::vector<std::string> arguments)
{
    printf("showing path");
}

void changeDirectory(std::vector<std::string> arguments)
{
    printf("changing directory");
}