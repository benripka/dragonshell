#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "commands.h"
#include "utils.h"

bool shellRunning;
bool runningInBackground;
std::string $PATH;

int pipefd[2];

const char* LineDeliminator = ";";
const char* CommandDeliminator = " ";
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

void executeCommand(std::vector<std::string> commands, bool outputPiped, bool inputPiped)
{
  if(commands.back().compare("&") == 0)
  {
    runningInBackground = true;
  }

  std::vector<std::string> arguments = commands;
  arguments.erase(arguments.begin());

  if(commands[0].compare("cd") == 0)
  {
    changeDirectory(arguments);
  }
  else if(commands[0].compare("pwd") == 0)
  {
    printWorkingDirectory(arguments);
  }
  else if(commands[0].compare("exit") == 0)
  {
    exitShell(arguments);
  }
  else if(commands[0].compare("a2path") == 0)
  {
    addAddressToPath(commands[1]);
  }
  else if(commands[0].compare("$PATH") == 0)
  {
    showPath(arguments);
  }
  else
  {

    pid_t pid;
    int childStatus = 1;

    pipe(pipefd);
    pid = fork();

    //child
    if(pid == 0)
    {
      if(!inputPiped && !outputPiped)
      {
        executeExternalProgram(commands, STDIN_FILENO, STDOUT_FILENO);
      }
      else if (inputPiped && !outputPiped)
      {
        executeExternalProgram(commands, pipefd[0], STDOUT_FILENO);
      }
      else if(!inputPiped && outputPiped)
      {
        executeExternalProgram(commands, STDIN_FILENO, pipefd[1]);
      }
      else
      {
        executeExternalProgram(commands, pipefd[0], pipefd[1]);
      }
      
    }
    //parent
    else if(!runningInBackground)
    {
      wait(&childStatus);
    }
  }
}

void processLine(std::string line)
{
  std::vector<std::string> pipedCommands = tokenize(line, PipeDeliminator);


    //Single command, no need to pipe
    if(pipedCommands.size() == 1)
    {
      executeCommand(tokenize(pipedCommands[0], CommandDeliminator), false, false);
      return;
    }

    for(int j = 0; j <  pipedCommands.size(); j++)
    {
      if(j == 0)
      {
        executeCommand(tokenize(pipedCommands[j], CommandDeliminator), true, false);
      }
      else if(j == pipedCommands.size())
      {
        executeCommand(tokenize(pipedCommands[j], CommandDeliminator), false, true);
      }
      else
      {
        executeCommand(tokenize(pipedCommands[j], CommandDeliminator), true, true);
      }
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

