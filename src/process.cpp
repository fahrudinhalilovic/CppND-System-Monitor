#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

Process::Process(int pid) : pid_{pid} {}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const {
  const float totalTime =
      LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);

  const float seconds = LinuxParser::UpTime() - UpTime();

  return 100 * (totalTime / seconds);
}

std::string Process::Command() const { return LinuxParser::Command(pid_); }

std::string Process::Ram() const { return LinuxParser::Ram(pid_); }

std::string Process::User() const { return LinuxParser::User(pid_); }

long int Process::UpTime() const {
  const auto startTime = LinuxParser::UpTime(pid_);
  return startTime / sysconf(_SC_CLK_TCK);
}

bool Process::operator<(Process const& other) const {
  if (CpuUtilization() != other.CpuUtilization()) {
    // sort them according to the CPU utilization
    return CpuUtilization() > other.CpuUtilization();
  } else if (Ram() != other.Ram()) {
    // then try to sort according to the RAM utilization
    return Ram() > other.Ram();
  } else {
    // use PID as a fallback option
    return Pid() < other.Pid();
  }
}
