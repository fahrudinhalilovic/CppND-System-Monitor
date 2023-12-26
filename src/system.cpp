#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { return processes_; }

std::string System::Kernel() {
    const std::filesystem::path KernelPath { "/proc/version" };

    if ( !std::filesystem::is_regular_file(KernelPath) ) {
        throw std::runtime_error { "Missing file " + KernelPath.string() };
    }

    std::ifstream input { KernelPath };
    if ( !input ) {
        throw std::runtime_error { "There was en error while opening input stream!" };
    }

    std::string line;
    std::string kernel;

    input.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    input.ignore(std::numeric_limits<std::streamsize>::max(), ' ');

    input >> kernel;

    return kernel;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return 0.0; }

std::string System::OperatingSystem() {
    const std::filesystem::path OSInfoPath { "/etc/os-release" };

    if ( !std::filesystem::is_regular_file(OSInfoPath) ) {
        throw std::runtime_error { "Missing file " + OSInfoPath.string() };
    }

    const auto propertyName { "PRETTY_NAME" };
    
    std::ifstream input { OSInfoPath };
    if ( !input ) {
        throw std::runtime_error { "There was en error while opening input stream!" };
    }

    std::string line;

    while( std::getline(input, line) ) {
        if ( line.find(propertyName) != std::string::npos ) {
            const auto separator = '"';
            if ( std::count(std::begin(line), std::end(line), separator) != 2 ) {
                throw std::runtime_error { "Expected 2 \" in the line: " + line };
            }

            const auto start = line.find(separator);
            const auto end = line.find(separator, start + 1);
            return line.substr(start + 1, end - start - 1);
        }
    }

    return std::string { "N/A" };

}

struct ProcessesInfo {
    size_t all = 0;
    size_t running = 0;
};

ProcessesInfo parseProcessesFile()
{
    const std::filesystem::path processesPath { "/proc/stat" };

    if ( !std::filesystem::is_regular_file(processesPath) ) {
        throw std::runtime_error { "Missing file " + processesPath.string() };
    }

    const auto allProcess = "processes";
    const auto runningProcesses = "procs_running";

    std::ifstream input { processesPath };
    if ( !input ) {
        throw std::runtime_error { "There was en error while opening input stream!" };
    }

    ProcessesInfo procInfo;

    std::string line;
    while ( std::getline(input, line) ) {
        std::string propertyName;
        std::stringstream sstream { line };
        sstream >> propertyName;

        if ( propertyName == allProcess ) {
            sstream >> procInfo.all;
        }
        else if ( propertyName == runningProcesses ) {
            sstream >> procInfo.running;
        }
    }

    return procInfo;
}


int System::RunningProcesses() {
    return parseProcessesFile().running;
}

int System::TotalProcesses() {
    return parseProcessesFile().all;
}

long int System::UpTime() {
    const std::filesystem::path upTimePath { "/proc/uptime" };

    if ( !std::filesystem::is_regular_file(upTimePath) ) {
        throw std::runtime_error { "Missing file " + upTimePath.string() };
    }

    std::ifstream input { upTimePath };
    if ( !input ) {
        throw std::runtime_error { "There was en error while opening input stream!" };
    }

    long int upTime = 0u;
    input >> upTime;
    return upTime;
}
