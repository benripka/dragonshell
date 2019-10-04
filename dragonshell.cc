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

void executeNonPiped(std::vector<std::string> command)
{
  bool runningInBackground = false;

  if(command.back().compare("&") == 0)
  {
    runningInBackground = true;
  }
  if(tryBuiltins(command) == 0)
  {
    return;
  }
  else 
  {
    pid_t pid = fork();

    if(pid == 0)
    {
      executeExternalProgram(command);
    }
    else if (!runningInBackground)
    {
      wait(NULL);
    }
  }
}

void executePiped(std::vector<std::string> pipedCommands)
{
  pid_t pid1, pid2;
  int pipeFd[2];

  if(pipe(pipeFd) < 0) printf("Pipe failed");
  
  if((pid1 = fork()) < 0) printf("fork failed");

  //In the child
  if(pid1 == 0)
  {
    close(pipeFd[0]);
    dup2(pipeFd[1], STDOUT_FILENO);
    close(pipeFd[1]);
    executeExternalProgram(tokenize(pipedCommands[0], CommandDeliminator));
  }
  else 
  {
    if((pid2 = fork()) < 0) printf("fork failed");

    //Second child
    if(pid2 == 0)
    {
      close(pipeFd[1]);
      dup2(pipeFd[0], STDIN_FILENO);
      close(pipeFd[0]);
      executeExternalProgram(tokenize(pipedCommands[1], CommandDeliminator));
    }
    else 
    {
      close(pipeFd[0]);
      close(pipeFd[1]);
      wait(NULL);
      wait(NULL);
    }
  }
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
      dup2(outputFd, STDOUT_FILENO);
      close(outputFd);
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

