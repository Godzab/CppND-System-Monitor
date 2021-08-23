#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() const{
    return this->pid_;
}

float Process::CpuUtilization() {
    return LinuxParser::CpuUtilization(this->Pid());
}

string Process::Command() {
    return LinuxParser::Command(this->Pid());
}

string Process::Ram() {
    return LinuxParser::Ram(this->Pid());
}

string Process::User() {
    return LinuxParser::User(this->Pid());
}

long int Process::UpTime() {
    return LinuxParser::UpTime(this->Pid());
}

bool Process::operator<(Process const& a) const {
    return Pid() < a.Pid();
}

Process::Process(int pid):pid_(pid){}