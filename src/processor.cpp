#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
    const auto currTotalUtilization = LinuxParser::Jiffies();
    const auto currIdleUtilization = LinuxParser::IdleJiffies();

    float totalUtilization = currTotalUtilization - prevTotalUtilization;
    float idleUtilization = currIdleUtilization - prevIdleUtilization;

    prevTotalUtilization = currTotalUtilization;
    prevIdleUtilization = currIdleUtilization;

    return (totalUtilization - idleUtilization) / totalUtilization;
}
