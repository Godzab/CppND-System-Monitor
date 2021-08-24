#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <numeric> 

#include "linux_parser.h"

#define Hertz sysconf(_SC_CLK_TCK)

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "PRETTY_NAME") {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
    string os, kernel, version;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }
    return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
    vector<int> pids;
    DIR *directory = opendir(kProcDirectory.c_str());
    struct dirent *file;
    while ((file = readdir(directory)) != nullptr) {
        // Is this a directory?
        if (file->d_type == DT_DIR) {
            // Is every character of the name a digit?
            string filename(file->d_name);
            if (std::all_of(filename.begin(), filename.end(), isdigit)) {
                int pid = stoi(filename);
                pids.push_back(pid);
            }
        }
    }
    closedir(directory);
    return pids;
}

float LinuxParser::MemoryUtilization() {
    string line;
    string key;
    string type;
    double value, total_memory, free_memory;
    std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value >> type) {
                if (key == "MemTotal:") {
                    total_memory = value;
                }
                if (key == "MemFree:") {
                    free_memory = value;
                }
            }
        }
    }
    return static_cast<float>(total_memory - free_memory) / total_memory;
}

long LinuxParser::UpTime() {
    string line;
    float start_time, end_time;
    std::ifstream filestream(kProcDirectory + kUptimeFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream line_stream(line);
            line_stream >> start_time >> end_time;
        }
    }
    return static_cast<long>(start_time);
}

long LinuxParser::Jiffies() {
    vector<long> cpu_values = LinuxParser::CpuUtilization();
    long total_capacity = std::accumulate(cpu_values.begin(), cpu_values.end(), 0);
    return total_capacity;
}

long LinuxParser::ActiveJiffies(int pid) {
    std::ifstream p_file;
    p_file.open(kProcDirectory + std::to_string(pid) + kStatFilename);
    long stat_item;
    vector<long> stat_data{};
    string str_line, tmp;
    long u_time, s_time, cu_time, cs_time, total_time = 0;

    if (p_file.is_open()) {
        getline(p_file, str_line);
        std::istringstream stream_line(str_line);
        stream_line >> stat_item >> tmp >> tmp;
        while (stream_line >> stat_item) {
            stat_data.push_back(stat_item);
        }
    }

    if(stat_data.size() >= 14){
        //Positions are offset (-3) because vector skipped first 3 elements.
        u_time = stat_data.at(11);
        s_time = stat_data.at(12);
        cu_time = stat_data.at(13);
        cs_time = stat_data.at(14);

        total_time = u_time + s_time;
        total_time += cu_time + cs_time;
        total_time /= Hertz;
    }

    return total_time;
}

long LinuxParser::ActiveJiffies() {
    vector<long> cpu_values = LinuxParser::CpuUtilization();
    long active_jiffies = cpu_values.at(LinuxParser::CPUStates::kUser_) +
                          cpu_values.at(CPUStates::kNice_) +
                          cpu_values.at(CPUStates::kSteal_) +
                          cpu_values.at(CPUStates::kIRQ_) +
                          cpu_values.at(CPUStates::kSoftIRQ_) +
                          cpu_values.at(CPUStates::kGuest_) +
                          cpu_values.at(CPUStates::kGuestNice_) +
                          cpu_values.at(CPUStates::kSystem_);
    return active_jiffies;
}

long LinuxParser::IdleJiffies() {
    vector<long> cpu_values = LinuxParser::CpuUtilization();
    long idle_jiffies = cpu_values.at(CPUStates::kIdle_) +
                        cpu_values.at(CPUStates::kIOwait_);
    return idle_jiffies;
}

vector<long> LinuxParser::CpuUtilization() {
    vector<long> cpu_units{};
    std::ifstream cpu_stats_file;
    cpu_stats_file.open(kProcDirectory + kStatFilename);
    string line, tmp;
    long itr_line;
    if (cpu_stats_file.is_open()) {
        std::getline(cpu_stats_file, line);
        std::istringstream stream_line(line);
        stream_line >> tmp; // Skip the first column with cpu label.
        while (stream_line >> itr_line) {
            cpu_units.push_back(itr_line);
        }
    }
    return cpu_units;
}

int LinuxParser::TotalProcesses() {
    std::ifstream stats_file;
    stats_file.open(kProcDirectory + kStatFilename);
    string line, itr_line;
    int target_line;

    if (stats_file.is_open()) {
        while (getline(stats_file, line)) {
            std::istringstream stream_line(line);
            stream_line >> itr_line;
            if (itr_line == "processes") {
                stream_line >> target_line;
                break;
            }
        }
        stats_file.close();
    }
    return target_line;
}

int LinuxParser::RunningProcesses() {
    std::ifstream stats_file;
    stats_file.open(kProcDirectory + kStatFilename);
    string line, itr_line;
    int target_line;

    if (stats_file.is_open()) {
        while (getline(stats_file, line)) {
            std::istringstream stream_line(line);
            stream_line >> itr_line;
            if (itr_line == "procs_running") {
                stream_line >> target_line;
                break;
            }
        }
        stats_file.close();
    }
    return target_line;
}

string LinuxParser::Command(int pid) {
    std::ifstream p_file;
    p_file.open(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    string cmdline = "";
    if (p_file.is_open()) {
        getline(p_file, cmdline);
    }
    return cmdline;
}

string LinuxParser::Ram(int pid) {
    std::ifstream p_file;
    p_file.open(kProcDirectory + std::to_string(pid) + kStatusFilename);
    long memory_usage = 0;
    string str_line, label, result;

    if (p_file.is_open()) {
        while (getline(p_file, str_line)) {
            std::istringstream stream_line(str_line);
            stream_line >> label;
            if (label == "VmSize:") {
                stream_line >> memory_usage;
                break;
            }
        }
        memory_usage /= 1024;
    }

    return std::to_string(memory_usage);
}

string LinuxParser::Uid(int pid) {
    std::ifstream p_file;
    p_file.open(kProcDirectory + std::to_string(pid) + kStatusFilename);
    int uid;
    string str_line, label;

    if (p_file.is_open()) {
        while (getline(p_file, str_line)) {
            std::istringstream stream_line(str_line);
            stream_line >> label;
            if (label == "Uid:") {
                stream_line >> uid;
                break;
            }
        }
    }
    return std::to_string(uid);
}

string LinuxParser::User(int pid) {
    string uid = LinuxParser::Uid(pid);
    std::ifstream p_file;
    p_file.open(kPasswordPath);
    string str_line, label;
    vector <string> user_data{};

    if (p_file.is_open()) {
        while (getline(p_file, str_line, ':')) {
            if (str_line == uid) {
                break;
            }

            user_data.push_back(str_line);
        }
    }
    user_data.pop_back(); //First value disregarded.
    return user_data.back();
}

long LinuxParser::UpTime(int pid) {
    std::ifstream p_file;
    p_file.open(kProcDirectory + std::to_string(pid) + kStatFilename);
    long stat_item;
    long uptime = LinuxParser::UpTime();
    vector<long> stat_data{};
    string str_line, tmp;
    long start_time, seconds = 0;


    if (p_file.is_open()) {
        getline(p_file, str_line);
        std::istringstream stream_line(str_line);
        stream_line >> stat_item >> tmp >> tmp;
        while (stream_line >> stat_item) {
            stat_data.push_back(stat_item);
        }
    }

    if(stat_data.size()>=18){
        start_time = stat_data.at(18);
        seconds = uptime - (start_time / Hertz);
    }

    return seconds;
}

float LinuxParser::CpuUtilization(int pid) {
    long process_uptime = LinuxParser::UpTime(pid);
    long process_jiffies = LinuxParser::ActiveJiffies(pid);
    if(process_jiffies > 0){
        return static_cast<float>(process_jiffies) / process_uptime;
    }
    return 0.0;
}