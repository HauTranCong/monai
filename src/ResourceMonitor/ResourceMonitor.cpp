#include <iostream>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <string>
#include <sstream>

void monitor_hardware_usage() {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) == 0) {
        std::cout << "\n--- Hardware Usage ---" << std::endl;
        std::cout << "Uptime: " << sys_info.uptime << " seconds" << std::endl;
        std::cout << "Total RAM: " << sys_info.totalram / 1024 / 1024 << " MB" << std::endl;
        std::cout << "Used RAM: " << (sys_info.totalram - sys_info.freeram) / 1024 / 1024 << " MB" << std::endl;
        std::cout << "Free RAM: " << sys_info.freeram / 1024 / 1024 << " MB" << std::endl;
        std::cout << "Processes: " << sys_info.procs << std::endl;

        // Read CPU usage from /proc/stat
        std::ifstream stat_file("/proc/stat");
        std::string line;
        if (stat_file.is_open()) {
            while (std::getline(stat_file, line)) {
                if (line.compare(0, 3, "cpu") == 0) {
                    std::istringstream ss(line);
                    std::string cpu;
                    long user, nice, system, idle, iowait, irq, softirq, steal;
                    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
                    long total = user + nice + system + idle + iowait + irq + softirq + steal;
                    long active = total - idle;
                    double cpu_usage = (double)active / total * 100;
                    std::cout << "CPU Usage: " << cpu_usage << "%" << std::endl;
                    break;
                }
            }
            stat_file.close();
        } else {
            std::cerr << "Failed to open /proc/stat" << std::endl;
        }
    } else {
        std::cerr << "Failed to retrieve system info" << std::endl;
    }
}

int main() {
    while (true) {
        monitor_hardware_usage();
        sleep(5);
    }
    return 0;
}
