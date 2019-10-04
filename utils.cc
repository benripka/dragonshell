#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>

extern std::string $PATH;

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

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
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

  if((fd = open(outputFile.c_str(), O_CREAT | O_RDWR, 0644)) != -1)
  {
    return fd;
  }
  return -1;
}