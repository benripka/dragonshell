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
    printf("\n");
}

void exitShell(std::vector<std::string> arguments)
{
    printf("exiting shell");
}

void addAddressToPath(std::string address)
{
    printf("adding address to path");
    $PATH.append(std::string(":").append(address));
}

void showPath(std::vector<std::string> arguments)
{
    printf("%s", $PATH.c_str());
}

bool checkIfFileExists(const char* fileName)
{
    struct stat buffer;
    int exists = stat(fileName, &buffer);

    if(exists == 0) return true;
    else return false;
}

void executeExternalProgram(std::vector<std::string> commands)
{
    //do not need to read, only write
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);

    const char* path = commands[0].c_str();

    //leave room for null terminator
    const char** argv = new const char* [commands.size() + 1];

    for (int i = 0; i < commands.size(); i++)
    {
        argv[i] = commands[i].c_str();
    }

    argv[commands.size()] = NULL;

    char* const* envp = {NULL};

    printf("Going to try the execve");

    //check is the path to a file

    if(checkIfFileExists(path))
    {
        if(execve(path, (char**) argv, envp) < 0) 
        {
            printf("Could not execute external program");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("file not found");
    }
    
    std::vector<std::string> pathsInPATH = tokenize($PATH, ":");

    for(int i = 0; i < pathsInPATH.size(); i++)
    {
        const char* fullyQualifiedFilename = pathsInPATH[i].append(commands[0]).c_str();

        //Found the file in PATH variable
        if(checkIfFileExists(fullyQualifiedFilename))
        {
            if(execve(fullyQualifiedFilename, (char**) argv, envp) < 0) 
            {
                printf("Could not execute external program");
                exit(EXIT_FAILURE);
            }
        }
        else 
        {
            printf("File not founds");
        }
    }
    printf("file not found in path");
}