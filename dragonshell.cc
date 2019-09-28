#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "commands.h"


/**
 * @brief Tokenize a string 
 * 
 * @param str - The string to tokenize
 * @param delim - The string containing delimiter character(s)
 * @return std::vector<std::string> - The list of tokenized strings. Can be empty
 */
std::vector<std::string> tokenize(const std::string &str, const char *delim) {
  char* cstr = new char[str.size() + 1];
  std::strcpy(cstr, str.c_str());

  char* tokenized_string = strtok(cstr, delim);

  std::vector<std::string> tokens;
  while (tokenized_string != NULL)
  {
    tokens.push_back(std::string(tokenized_string));
    tokenized_string = strtok(NULL, delim);
  }
  delete[] cstr;

  return tokens;
}

void processInput(std::string input);
void executeCommandInBackground(std::vector<std::string> commands);
void executeCommand(std::vector<std::string> commands);

const char* LineDeliminator = ";";
const char* CommandDeliminator = " ";
const char* ProcessDeliminator = "|";
const char* prompt = ">>";

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop

  bool shellRunning = true;

  char *buffer;
  size_t maxBufferSize = 200 * sizeof(char);

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
      if(!commands.back().compare("&"))
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

  pid_t pid;
  int childStatus = 1;

  if(pid = fork() < 0) printf("Fork Failed");

  //child
  if(pid == 0)
  {
    if(commands[0].compare("cd") == 0)
    {
      changeDirectory(arguments);
    }
    else if(commands[0].compare("pwd") == 0)
    {
      printWorkingDirectory(arguments);
    }
    else if(!commands[0].compare("exit") == 0)
    {
      exitShell(arguments);
    }
    else if(!commands[0].compare("a2path") == 0)
    {
      addAddressToPath(arguments);
    }
    else if(!commands[0].compare("$PATH") == 0)
    {
      showPath(arguments);
    }
    else 
    {
      executeExternalProgram(commands);
    }
  }
  //parent
  else
  {
    printf("parent is waiting");
    wait(&childStatus);
  }
  
}