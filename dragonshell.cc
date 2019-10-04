#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include "commands.h"
#include "utils.h"

bool shellRunning;
bool runningInBackground;
std::string $PATH;

const char* LineDeliminator = ";";
const char* CommandDeliminator = " ";
const char* RedirectDeliminator = ">";
const char* PipeDeliminator = "|";
const char* prompt = ">>";
const int MaxOutputBufferSize = 300;

/**
 * @brief Tokenize a string 
 * 
 * @param str - The string to tokenize
 * @param delim - The string containing delimiter character(s)
 * @return std::vector<std::string> - The list of tokenized strings. Can be empty
 */

void handleExitSignal(int signumber)
{
  printf("Caught yo signal foo");
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

int tryOpenFile(std::string outputFile)
{
  int fd;
  const char * fileName;

  std::vector<std::string> pathsInPath = tokenize($PATH, ":");
  for(int i = 0; i < pathsInPath.size(); i++)
  {
    fileName = pathsInPath[i].append(outputFile).c_str();
    if((fd = open(fileName, 0644)) != -1)
    {
      return fd;
    }
  }

  if((fd = open(outputFile.c_str(), O_CREAT | O_RDONLY, 0644)) != -1)
  {
    return fd;
  }
  return -1;
}

void executeNonPiped(std::vector<std::string> commands)
{

  if(tryBuiltins(commands) == 0)
  {
    return;
  }
  else 
  {
    pid_t pid = fork();

    if(pid == 0)
    {
      
      executeExternalProgram(commands);
    }
    else 
    {
      wait(NULL);
    }
  }
}

void executePiped(std::vector<std::string> pipedCommands)
{

}

void executeRedirect(std::vector<std::string> redirectCommands)
{
  std::string outputFileName = trim(redirectCommands[1]);

  std::vector<std::string> commandToRedirect = tokenize(redirectCommands[0], CommandDeliminator);

  int outputFd;

  pid_t pid;

  if((outputFd = tryOpenFile(outputFileName)) < 0)
  {
    printf("Could not open / create file");
  }
  else
  {
    pid = fork();
    if(pid == 0)
    {
      dup2(outputFd, STDIN_FILENO);
      close(outputFd);
      tryBuiltins(commandToRedirect);
      executeExternalProgram(commandToRedirect);
    }
    else 
    {
      wait(NULL);
      close(outputFd);
      return;
    }
  }
}

void processLine(std::string line)
{
  if(line.find("|") != -1)
  {
    std::vector<std::string> pipedCommands = tokenize(line, PipeDeliminator);
    executePiped(pipedCommands);
  }
  else if(line.find(">") != -1)
  {
    std::vector<std::string> redirectedCommands = tokenize(line, RedirectDeliminator);
    executeRedirect(redirectedCommands);
  }
  else
  {
    std::vector<std::string> normalCommands = tokenize(line, CommandDeliminator);
    executeNonPiped(normalCommands);
  }
}


void processInput(std::string input)
{
  std::vector<std::string> lines = tokenize(input, LineDeliminator);

  for (int i = 0; i < lines.size(); i++)
  {
    processLine(lines[i]);
  }
}

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop

  char *buffer;
  size_t maxBufferSize = 200 * sizeof(char);

  $PATH = "/usr/bin/:/bin/";
  shellRunning = true;
  runningInBackground = false;

  struct sigaction exitSignalAction;
  exitSignalAction.sa_handler = handleExitSignal;
  exitSignalAction.sa_flags = 0;
  
  while(shellRunning)
  {

    printf(">>");

    std::string input = std::string();

    std::getline(std::cin, input);

    processInput(input);

  }

  return 0;
}

