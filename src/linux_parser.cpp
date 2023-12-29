#include "linux_parser.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>

std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
  std::ifstream filestream{osPath};
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
  std::ifstream stream{versionPath};
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

  std::ifstream input{memInfoPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string line;
  float memTotal = 0;
  float memFree = 0;

  while (std::getline(input, line)) {
    std::istringstream linestream{line};
    std::string property;
    float val;
    linestream >> property >> val;
    if (property == MemTotal) {
      memTotal = val;
    } else if (property == MemFree) {
      memFree = val;
    }
  }

  return memTotal - memFree;
}

long LinuxParser::UpTime() {
  std::ifstream input{upTimePath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  long upTime = 0u;
  input >> upTime;
  return upTime;
}

struct OSJiffies {
  explicit OSJiffies(const std::vector<std::string>& input) {
    assert(input.size() == 10);

    user = std::stol(input[0]);
    nice = std::stol(input[1]);
    system = std::stol(input[2]);
    idle = std::stol(input[3]);
    iowait = std::stol(input[4]);
    irq = std::stol(input[5]);
    softirg = std::stol(input[6]);
    steal = std::stol(input[7]);
    guest = std::stol(input[8]);
    guestNice = std::stol(input[9]);
  }

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

long LinuxParser::Jiffies() {
  OSJiffies osJiffies{CpuUtilization()};
  return osJiffies.Total();
}

long LinuxParser::ActiveJiffies(int pid) {
  const auto statPath = proc / std::to_string(pid) / statName;

  std::ifstream input{statPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string line;
  std::getline(input, line);
  std::istringstream linestream{line};

  long uTime;
  long sTime;
  long cuTime;
  long csTime;
  long startTime;

  for (auto idx = 1u; idx <= 17; ++idx) {
    std::string val;
    linestream >> val;
    if (idx == 14) {
      uTime = std::stol(val);
    } else if (idx == 15) {
      sTime = std::stol(val);
    } else if (idx == 16) {
      cuTime = std::stol(val);
    } else if (idx == 17) {
      csTime = std::stol(val);
    }
  }

  return uTime + sTime + cuTime + csTime;
}

long LinuxParser::ActiveJiffies() {
  OSJiffies osJiffies{CpuUtilization()};
  return osJiffies.NonIdle();
}

long LinuxParser::IdleJiffies() {
  OSJiffies osJiffies{CpuUtilization()};
  return osJiffies.Idle();
}

std::vector<std::string> LinuxParser::CpuUtilization() {
  const auto statPath = LinuxParser::proc / LinuxParser::statName;
  std::ifstream input{statPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  const auto numberOfProps = 10u;
  std::vector<std::string> res{numberOfProps};
  const auto cpu{"cpu"};

  std::string line;
  while (std::getline(input, line)) {
    std::string desc;
    std::istringstream linestream{line};
    linestream >> desc;
    if (desc == cpu) {
      for (auto idx = 0u; idx < numberOfProps; ++idx) {
        linestream >> res[idx];
      }
    }
  }

  return res;
}

struct ProcessesInfo {
  size_t all = 0;
  size_t running = 0;
};

ProcessesInfo parseProcessesFile() {
  const auto allProcess = "processes";
  const auto runningProcesses = "procs_running";

  const auto statPath = LinuxParser::proc / LinuxParser::statName;
  std::ifstream input{statPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  ProcessesInfo procInfo;

  std::string line;
  while (std::getline(input, line)) {
    std::string propertyName;
    std::istringstream linestream{line};
    linestream >> propertyName;

    if (propertyName == allProcess) {
      linestream >> procInfo.all;
    } else if (propertyName == runningProcesses) {
      linestream >> procInfo.running;
    }
  }

  return procInfo;
}

int LinuxParser::TotalProcesses() { return parseProcessesFile().all; }

int LinuxParser::RunningProcesses() { return parseProcessesFile().running; }

std::string LinuxParser::Command(int pid) {
  const auto cmdPath = proc / std::to_string(pid) / cmdName;
  std::ifstream input{cmdPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string cmd;
  std::getline(input, cmd);
  return cmd;
}

std::string parseProcessStatusFile(int pid, std::string property) {
  const auto statusPath{LinuxParser::proc / std::to_string(pid) /
                        LinuxParser::statusName};
  std::ifstream input{statusPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string line;
  std::string val;

  while (std::getline(input, line)) {
    std::istringstream linestream{line};
    std::string propertyName;
    linestream >> propertyName;
    if (propertyName == property) {
      linestream >> val;
      break;
    }
  }

  return val;
}

std::string LinuxParser::Ram(int pid) {
  const auto ramInKB = parseProcessStatusFile(pid, "VmSize:");
  if (ramInKB.empty()) {
    return "0";
  }

  const auto ram = std::stoi(ramInKB);
  return std::to_string(ram / 1024);
}

std::string LinuxParser::Uid(int pid) {
  return parseProcessStatusFile(pid, "Uid:");
}

std::string LinuxParser::User(int pid) {
  const auto uid = Uid(pid);

  std::ifstream input{passwordPath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string line;
  while (std::getline(input, line)) {
    std::replace(std::begin(line), std::end(line), ':', ' ');
    std::istringstream linestream{line};
    std::string username;
    std::string passwd;
    std::string tempUid;
    linestream >> username >> passwd >> tempUid;
    if (uid == tempUid) {
      return username;
    }
  }

  return std::string{};
}

long LinuxParser::UpTime(int pid) {
  const auto propertyPos = 22u;
  const auto upTimePath = proc / std::to_string(pid) / LinuxParser::statName;

  std::ifstream input{upTimePath};
  if (!input) {
    throw std::runtime_error{"There was en error while opening input stream!"};
  }

  std::string line;
  std::getline(input, line);
  std::istringstream linestream{line};

  for (auto idx = 1u; idx < propertyPos; ++idx) {
    std::string val;
    linestream >> val;
  }

  long startTime;
  linestream >> startTime;
  return startTime;
}
