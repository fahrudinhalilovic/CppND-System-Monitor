#include "linux_parser.h"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
  std::ifstream filestream { osPath };
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

std::string LinuxParser::Kernel() {
  std::string os, kernel, version;
  std::string line;
  std::ifstream stream {versionPath};
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;

  std::filesystem::directory_iterator dirEntry{proc};

  for (const auto& entry : dirEntry) {
    if (!entry.is_directory()) {
      continue;
    }

    std::string filename{entry.path().filename()};
    if (std::all_of(filename.begin(), filename.end(), isdigit)) {
      pids.push_back(std::stoi(filename));
    }
  }

  return pids;
}

float LinuxParser::MemoryUtilization() {
  const auto MemTotal{"MemTotal:"};
  const auto MemFree{"MemFree:"};

  if (!std::filesystem::is_regular_file(memInfoPath)) {
    throw std::runtime_error{"Missing file: " + memInfoPath.string()};
  }

  std::ifstream input{memInfoPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string line;
  float memTotal = 0;
  float memFree = 0;

  while (std::getline(input, line)) {
    std::stringstream sstream{line};
    std::string property;
    float val;
    sstream >> property >> val;
    if (property == MemTotal) {
      memTotal = val;
    } else if (property == MemFree) {
      memFree = val;
    }
  }

  return memTotal - memFree;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  if (!std::filesystem::is_regular_file(upTimePath)) {
    throw std::runtime_error{"Missing file " + upTimePath.string()};
  }

  std::ifstream input{upTimePath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  long upTime = 0u;
  input >> upTime;
  return upTime;
}

struct OSJiffies {
  long user;
  long nice;
  long system;
  long idle;
  long iowait;
  long irq;
  long softirg;
  long steal;
  long guest;
  long guestNice;

  long Idle() { return idle + iowait; }
  long NonIdle() { return user + nice + system + irq + softirg + steal; }
  long Total() { return Idle() + NonIdle(); }
};

OSJiffies getOSJiffies()
{
  std::ifstream input { LinuxParser::statPath };
  if ( !input ) {
    throw std::runtime_error { "There was en error while opening input stream!" };
  }

  OSJiffies j;

  const auto cpu { "cpu" };
  std::string line;
  while ( std::getline(input, line) ) {
    std::string desc;
    std::istringstream sstream { line };
    sstream >> desc;
    if ( desc == cpu ) {
      sstream >> j.user >> j.nice >> j.system >> j.idle >> j.iowait >> j.irq >> j.softirg >> j.steal >> j.guest >> j.guestNice;
    }
  }

  return j;
}

long LinuxParser::Jiffies() { return getOSJiffies().Total(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

long LinuxParser::ActiveJiffies() { return getOSJiffies().NonIdle(); }

long LinuxParser::IdleJiffies() { return getOSJiffies().Idle(); }

// TODO: Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() { return {}; }

struct ProcessesInfo {
  size_t all = 0;
  size_t running = 0;
};

ProcessesInfo parseProcessesFile() {
  const auto allProcess = "processes";
  const auto runningProcesses = "procs_running";

  std::ifstream input{LinuxParser::statPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  ProcessesInfo procInfo;

  std::string line;
  while (std::getline(input, line)) {
    std::string propertyName;
    std::stringstream sstream{line};
    sstream >> propertyName;

    if (propertyName == allProcess) {
      sstream >> procInfo.all;
    } else if (propertyName == runningProcesses) {
      sstream >> procInfo.running;
    }
  }

  return procInfo;
}

int LinuxParser::TotalProcesses() { return parseProcessesFile().all; }

int LinuxParser::RunningProcesses() { return parseProcessesFile().running; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Command(int pid [[maybe_unused]]) {
  return std::string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Ram(int pid [[maybe_unused]]) { return std::string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Uid(int pid [[maybe_unused]]) { return std::string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::User(int pid [[maybe_unused]]) {
  return std::string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
