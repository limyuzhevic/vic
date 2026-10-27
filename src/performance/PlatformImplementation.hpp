// NLM Performance Benchmarking Module - Platform Implementation
// Platform-specific implementations for memory and CPU monitoring
// Supports Windows, Linux, and macOS

#ifndef NLM_PERFORMANCE_IMPLEMENTATION_H
#define NLM_PERFORMANCE_IMPLEMENTATION_H

#include <system_error>

namespace nlm {
namespace performance {

// Platform-specific memory tracking implementations
inline size_t getSystemMemoryUsage() {
#ifdef _WIN32
    // Windows implementation
    PROCESS_MEMORY_COUNTERS memCounters;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounters, sizeof(memCounters))) {
        return memCounters.WorkingSetSize;
    }
    return 0;
#elif defined(__linux__) || defined(__APPLE__)
    // Linux/macOS implementation
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // On Linux, ru_maxrss is in KB, on macOS it's in bytes
#ifdef __linux__
        return usage.ru_maxrss * 1024; // Convert KB to bytes
#else
        return usage.ru_maxrss; // Already in bytes on macOS
#endif
    }
    return 0;
#else
    // Default implementation for unknown platforms
    return 0;
#endif
}

// Platform-specific CPU usage implementation
inline double getSystemCPUUsage() {
#ifdef _WIN32
    // Windows implementation (simplified)
    return 0.0; // Windows CPU usage requires different approach
#elif defined(__linux__) || defined(__APPLE__)
    // Linux/macOS implementation
    return getLinuxCPUUsage();
#else
    return 0.0;
#endif
}

// Linux-specific CPU usage implementation
inline double getLinuxCPUUsage() {
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        return 0.0;
    }
    
    std::string line;
    std::getline(statFile, line);
    
    if (line.find("cpu") != 0) {
        return 0.0;
    }
    
    std::istringstream iss(line);
    std::string cpuLabel;
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    
    if (!(iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice)) {
        return 0.0;
    }
    
    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    long idleTotal = idle + iowait;
    
    if (total <= 0) {
        return 0.0;
    }
    
    return (total - idleTotal) * 100.0 / total;
}

// Function to get available CPU cores
inline int getSystemCPUCount() {
    return std::thread::hardware_concurrency();
}

// Function to get system information
inline std::string getSystemInfo() {
    std::stringstream info;
    
#ifdef _WIN32
    info << "Windows" << std::endl;
#elif defined(__linux__)
    info << "Linux" << std::endl;
#elif defined(__APPLE__)
    info << "macOS" << std::endl;
#else
    info << "Unknown OS" << std::endl;
#endif
    
    info << "CPU Cores: " << getSystemCPUCount() << std::endl;
    info << "Memory Implementation: ";
#ifdef _WIN32
    info << "Windows Native" << std::endl;
#elif defined(__linux__) || defined(__APPLE__)
    info << "POSIX getrusage" << std::endl;
#else
    info << "Placeholder" << std::endl;
#endif
    
    return info.str();
}

} // namespace performance
} // namespace nlm

#endif // NLM_PERFORMANCE_IMPLEMENTATION_H
