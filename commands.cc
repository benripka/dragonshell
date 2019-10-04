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

int tryBuiltins(std::vector<std::string> commands)
{
  std::vector<std::string> arguments = commands;
  arguments.erase(arguments.begin());

  if(commands[0].compare("cd") == 0)
  {
    changeDirectory(arguments);
    return 0;
  }
  else if(commands[0].compare("pwd") == 0)
  {
    printWorkingDirectory(arguments);
    return 0;
  }
  else if(commands[0].compare("exit") == 0)
  {
    exitShell(arguments);
    return 0;
  }
  else if(commands[0].compare("a2path") == 0)
  {
    addAddressToPath(commands[1]);
    return 0;
  }
  else if(commands[0].compare("$PATH") == 0)
  {
    showPath(arguments);
    return 0;
  }
  return -1;
}

void executeExternalProgram(std::vector<std::string> commands)
{
    if(commands.back().compare("&") == 0)
    {
        commands.pop_back();
    }
    char* path = (char*) commands[0].c_str();
    const char* delimin = ":";

    //leave room for null terminator
    char * argv[commands.size() + 1] = {};
    char *const envp[] = {NULL};

    for (int i = 0; i < commands.size(); i++)
    {
        argv[i] = (char *) trim(commands[i]).c_str();
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
          _exit(0);
        } 
    }
    printf("file not found in path");
    _exit(0);
}



