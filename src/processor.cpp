#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <iostream>

using std::vector;

//( user + nice + system + idle + iowait + irq + softirq )
double Processor::Utilization() {
    this->total_capacity_ = LinuxParser::Jiffies();
    double unused = static_cast<double>(LinuxParser::IdleJiffies());

    double total = this->total_capacity_ - this->cpu_utilization[0];
    double idle = unused - this->cpu_utilization[1];
    double cpuPercentage = (total - idle)/total;

    this->cpu_utilization[0] = this->total_capacity_;
    this->cpu_utilization[1] = unused;
    return cpuPercentage;
}