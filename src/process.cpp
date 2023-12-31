#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

Process::Process(int pid) : pid_{pid}, prevTotalTime_{0}, prevSeconds_{0} {
  CalculateCpuUtilization();
}

int Process::Pid() const { return pid_; }

void Process::CalculateCpuUtilization() {
  const float currTotalTime =
      LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  const float currSeconds = LinuxParser::UpTime() - UpTime();

  cpuUtilization_ =
      (currTotalTime - prevTotalTime_) / (currSeconds - prevSeconds_);

  prevTotalTime_ = currTotalTime;
  prevSeconds_ = currSeconds;
}

float Process::CpuUtilization() { return cpuUtilization_; }

float Process::CpuUtilization() const { return cpuUtilization_; }

std::string Process::Command() { return LinuxParser::Command(pid_); }

std::string Process::Ram() { return LinuxParser::Ram(pid_); }

std::string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() {
  const auto startTime = LinuxParser::UpTime(pid_);
  return startTime / sysconf(_SC_CLK_TCK);
}

bool Process::operator<(Process const& other) const {
  if (CpuUtilization() != other.CpuUtilization()) {
    // sort them according to the CPU utilization
    return CpuUtilization() > other.CpuUtilization();
  } else {
    // use PID as a fallback option
    return Pid() < other.Pid();
  }
}
