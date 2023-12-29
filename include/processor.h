#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  float prevTotalUtilization = 0;
  float prevIdleUtilization = 0;
};

#endif
