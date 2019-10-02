#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "utils.h"

extern std::string $PATH;
extern int pipefd[2];

void changeDirectory(std::vector<std::string> arguments)
{
    chdir(arguments[0].c_str());
}  

void printWorkingDirectory(std::vector<std::string> arguments)
{
    char* workingDirectory = new char[100 * sizeof(char)];

    getcwd(workingDirectory, 100 * sizeof(char));

    printf("%s\n", workingDirectory);
}

void exitShell(std::vector<std::string> arguments)
{
}

void addAddressToPath(std::string address)
{
    std::string pathVariableName = "$PATH";
    if(pathVariableName.compare(address.substr(0, 5)) == 0)
    {
        address.erase(0,6);
        $PATH.append(std::string(":").append(address));
    }
    else 
    {
        $PATH = address;
    }
}

void showPath(std::vector<std::string> arguments)
{
    printf("%s\n", $PATH.c_str());
}

bool checkIfFileExists(const char* fileName)
{
    struct stat buffer;
    int exists = stat(fileName, &buffer);

    if(exists == 0) return true;
    else return false;
}

void executeExternalProgram(std::vector<std::string> commands, int inputfd, int outputfd)
{

    dup2(inputfd, STDIN_FILENO);
    dup2(outputfd, STDOUT_FILENO);

    char* path = (char*) commands[0].c_str();
    const char* delimin = ":";

    //leave room for null terminator
    char * argv[commands.size() + 1] = {};
    char *const envp[] = {NULL};

    for (int i = 0; i < commands.size(); i++)
    {
        argv[i] = (char *) commands[i].c_str();
        (char* const) argv[i];
    }

    argv[commands.size()] = NULL;
    
    std::vector<std::string> pathsInPATH = tokenize($PATH, delimin);

    for(int i = 0; i < pathsInPATH.size(); i++)
    {
        const char* fullyQualifiedFilename = pathsInPATH[i].append(commands[0]).c_str();

        if(access(fullyQualifiedFilename, F_OK) == 0)
        {
            execve(fullyQualifiedFilename, (char**) argv, envp);

            return;
        } 
    }

    printf("file not found in path");

    return;
}