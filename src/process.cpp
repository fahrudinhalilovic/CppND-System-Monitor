#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

Process::Process(int pid) : pid_{pid}, prevTotalTime_{0}, prevUpTime_{0} {}

int Process::Pid() const { return pid_; }

void Process::CalculateCpuUtilization() {
  const float currTotalTime =
      static_cast<float>(LinuxParser::ActiveJiffies(pid_)) /
      static_cast<float>(sysconf(_SC_CLK_TCK));
  const float currUpTime = UpTime();

  if (currUpTime - prevUpTime_ < 0.00001) {
    cpuUtilization_ = 0;
  } else {
    cpuUtilization_ =
        (currTotalTime - prevTotalTime_) / (currUpTime - prevUpTime_);
  }

  prevTotalTime_ = currTotalTime;
  prevUpTime_ = currUpTime;
}

float Process::CpuUtilization() { return cpuUtilization_; }

float Process::CpuUtilization() const { return cpuUtilization_; }

std::string Process::Command() { return LinuxParser::Command(pid_); }

std::string Process::Ram() { return LinuxParser::Ram(pid_); }

std::string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& other) const {
  if (CpuUtilization() != other.CpuUtilization()) {
    // sort them according to the CPU utilization
    return CpuUtilization() > other.CpuUtilization();
  } else {
    // use PID as a fallback option
    return Pid() < other.Pid();
  }
}
