#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <filesystem>
#include <string>
#include <vector>

namespace LinuxParser {

// Paths
const std::filesystem::path proc{"/proc/"};
const std::string cmdName{"cmdline"};
const std::filesystem::path cpuInfoPath{proc / "cpuinfo"};
const std::string statusName{"status"};
const std::string statName{"stat"};
const std::filesystem::path upTimePath{proc / "uptime"};
const std::filesystem::path memInfoPath{proc / "meminfo"};
const std::filesystem::path versionPath{proc / "version"};
const std::filesystem::path osPath{"/etc/os-release"};
const std::filesystem::path passwordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum class CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif
