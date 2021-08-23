#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <iostream>

using std::vector;

//( user + nice + system + idle + iowait + irq + softirq )
float Processor::Utilization() {
    this->total_capacity_ = LinuxParser::Jiffies();
    float unused = static_cast<float>(LinuxParser::IdleJiffies());
  
    float total = this->total_capacity_ - this->cpu_utilization[0];
    float idle = unused - this->cpu_utilization[1];
    float cpuPercentage = (total - idle)/total;

    this->cpu_utilization[0] = this->total_capacity_;
    this->cpu_utilization[1] = unused;
    return cpuPercentage;
}