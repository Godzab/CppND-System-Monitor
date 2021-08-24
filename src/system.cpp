#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"
#include <algorithm>

using namespace std;

Processor &System::Cpu() { return cpu_; }

vector <Process> &System::Processes() {
    processes_ = {};
    const vector <int> &pids = LinuxParser::Pids();
    for (const int &pid: pids) {
        processes_.emplace_back(pid);
    }
    sort(processes_.begin(), processes_.end(), [](Process &a, Process &b) {
        return (b < a);
    });
    return processes_;
}

string System::Kernel() {
    return LinuxParser::Kernel();
}

float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

long int System::UpTime() {
    return LinuxParser::UpTime();
}