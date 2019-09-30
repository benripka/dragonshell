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

bool shellRunning = true;

std::string $PATH = "/usr/bin/:/bin/";

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

void processInput(std::string input);
void executeCommandInBackground(std::vector<std::string> commands);
void executeCommand(std::vector<std::string> commands);

const char* LineDeliminator = ";";
const char* CommandDeliminator = " ";
const char* ProcessDeliminator = "|";
const char* prompt = ">>";
const int MaxOutputBufferSize = 300;

int pipefd[2];

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop

  char *buffer;
  size_t maxBufferSize = 200 * sizeof(char);

  struct sigaction exitSignalAction;
  exitSignalAction.sa_handler = handleExitSignal;
  exitSignalAction.sa_flags = 0;

  sigaction(SIGINT, &exitSignalAction, NULL); 


  while(shellRunning)
  {

    printf(">>");

    std::string input = std::string();

    std::getline(std::cin, input);

    processInput(input);

  }

  return 0;
}

void processInput(std::string input)
{
  std::vector<std::string> lines = tokenize(input, LineDeliminator);

  for (int i = 0; i < lines.size(); i++)
  {
    std::vector<std::string> commands = tokenize(lines[i], CommandDeliminator);
    if(commands.back().compare("&") == 0)
    {
      executeCommandInBackground(commands);
    }
    else
    {
      executeCommand(commands);
    }
  }
}

void executeCommandInBackground(std::vector<std::string> commands)
{
  printf("executing command in background");
}

void executeCommand(std::vector<std::string> commands)
{
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
    
    if(pipe(pipefd) < 0) printf("Pipe failed");
    if(pid = fork() < 0) printf("Fork Failed");

    //child
    if(pid == 0)
    {
      executeExternalProgram(commands);
    }
    //parent
    else
    {
      printf("parent is waiting");

      char buffer[MaxOutputBufferSize];

      close(pipefd[1]); //no need to write

      ssize_t characterCount;

      while ((characterCount = read(pipefd[0], buffer, MaxOutputBufferSize)) > 0)
      {
        write(STDOUT_FILENO, buffer, characterCount);
      }

      close(pipefd[0]);

      wait(&childStatus);
    }
  }
}

