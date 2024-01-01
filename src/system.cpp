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

std::vector<Process>& System::Processes() {
  // obtain all processes
  const auto pids = LinuxParser::Pids();

  // add new processes
  for (const auto& pid : pids) {
    auto wasCreated{false};
    for (const auto& p : processes_) {
      if (pid == p.Pid()) {
        wasCreated = true;
        break;
      }
    }

    if (!wasCreated) {
      processes_.emplace_back(Process{pid});
    }
  }

  // remove finished processes
  for (auto idx = 0; idx < processes_.size(); ++idx) {
    auto isActive{false};
    for (const auto& pid : pids) {
      if (processes_[idx].Pid() == pid) {
        isActive = true;
        break;
      }
    }

    if (!isActive) {
      processes_.erase(std::begin(processes_) + idx);
    }
  }

  // refresh CPU utilization values
  for (auto& p : processes_) {
    p.CalculateCpuUtilization();
  }

  std::sort(std::begin(processes_), std::end(processes_));
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
