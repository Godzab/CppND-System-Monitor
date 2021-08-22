#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
public:
    float Utilization();
    Processor() : cpu_utilization{0, 0} {};

private:
    long total_capacity_;
    long cpu_utilization[2];
};

#endif