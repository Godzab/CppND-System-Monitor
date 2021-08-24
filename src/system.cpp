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

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
    vector<int> pids = LinuxParser::Pids();
    while(!pids.empty()){
        Process p(pids.back());
        processes_.push_back(p);
        pids.pop_back();
    }
    std::sort(processes_.begin(), processes_.end(),[](Process &a, Process &b) {
        return (b < a);
    });
    return processes_;
}

std::string System::Kernel() { 
	return LinuxParser::Kernel();
}

float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() { 
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