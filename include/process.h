#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
*/
class Process {
 public:
  explicit Process(int pid);
  int Pid() const;
  std::string User();
  std::string Command();
  void CalculateCpuUtilization();
  float CpuUtilization();
  float CpuUtilization() const;
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_;
  float prevTotalTime_;
  float prevUpTime_;
  float cpuUtilization_;
};

#endif
