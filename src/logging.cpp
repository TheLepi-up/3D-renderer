#include "logging.h"
#include <iostream>

const std::string LogLevels[5]{
  "[FATAL]",
  "[ERROR]",
  "[WARNING]",
  "[INFO]",
  "[TRACE]"
};

void log(const std::string &logLevel, const std::string &file, uint line, const std::string &msg, const std::string& component){
  if(component.empty())
    std::cout << file << ':' << line << '\n' << logLevel << ' ' << msg << "\n";
  else
    std::cout << file << ':' << line << '\n' << logLevel << " [" << component << "] " << msg << "\n";
}