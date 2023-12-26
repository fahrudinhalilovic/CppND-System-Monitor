#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

/*
You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.
*/

Processor& System::Cpu() { return cpu_; }

std::vector<Process>& System::Processes() { return processes_; }

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() {
  const std::filesystem::path OSInfoPath{"/etc/os-release"};

  if (!std::filesystem::is_regular_file(OSInfoPath)) {
    throw std::runtime_error{"Missing file " + OSInfoPath.string()};
  }

  const auto propertyName{"PRETTY_NAME"};

  std::ifstream input{OSInfoPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string line;

  while (std::getline(input, line)) {
    if (line.find(propertyName) != std::string::npos) {
      const auto separator = '"';
      if (std::count(std::begin(line), std::end(line), separator) != 2) {
        throw std::runtime_error{"Expected 2 \" in the line: " + line};
      }

      const auto start = line.find(separator);
      const auto end = line.find(separator, start + 1);
      return line.substr(start + 1, end - start - 1);
    }
  }

  return std::string{"N/A"};
}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
