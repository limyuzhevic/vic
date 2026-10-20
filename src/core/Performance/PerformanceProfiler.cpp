// PerformanceProfiler implementation

#include "PerformanceProfiler.hpp"

namespace nlm {

// Platform-specific implementations

// Windows implementations
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

size_t MemoryProfiler::getWindowsMemoryUsage() const {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PrivateUsage;
    }
    return 0;
}

void CPUMProfiler::getWindowsCPUTimes(CPUSample& sample) const {
    FILETIME idleTime, kernelTime, userTime;
    
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        sample.systemTime = (kernelTime.dwHighDateTime * 4294967296.0 + kernelTime.dwLowDateTime) / 10000000.0 - 462355200.0;
        sample.userTime = (userTime.dwHighDateTime * 4294967296.0 + userTime.dwLowDateTime) / 10000000.0 - 462355200.0;
        sample.idleTime = (idleTime.dwHighDateTime * 4294967296.0 + idleTime.dwLowDateTime) / 10000000.0 - 462355200.0;
        
        // Convert to milliseconds
        sample.systemTime *= 1000.0;
        sample.userTime *= 1000.0;
        sample.idleTime *= 1000.0;
        
        // Calculate other time components
        double total = sample.systemTime + sample.userTime + sample.idleTime;
        if (total > 0.0) {
            sample.niceTime = 0.0;
            sample.iowaitTime = 0.0;
            sample.irqTime = 0.0;
            sample.softirqTime = 0.0;
            sample.stealTime = 0.0;
            sample.guestTime = 0.0;
            sample.guestNiceTime = 0.0;
        }
    }
}
#endif

// Linux implementations
#ifdef __linux__
#include <fstream>
#include <sstream>

size_t MemoryProfiler::getLinuxMemoryUsage() const {
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        return 0;
    }
    
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("VmRSS:") == 0) {
            std::stringstream ss(line);
            std::string key;
            size_t value;
            ss >> key >> value;
            return value * 1024; // Convert KB to bytes
        }
    }
    return 0;
}

void CPUMProfiler::getLinuxCPUTimes(CPUSample& sample) const {
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        return;
    }
    
    std::string line;
    if (std::getline(statFile, line) && line.find("cpu  ") == 0) {
        std::stringstream ss(line);
        std::string cpuLabel;
        ss >> cpuLabel;
        
        ss >> sample.systemTime;
        ss >> sample.userTime;
        ss >> sample.niceTime;
        ss >> sample.idleTime;
        ss >> sample.iowaitTime;
        ss >> sample.irqTime;
        ss >> sample.softirqTime;
        ss >> sample.stealTime;
        ss >> sample.guestTime;
        ss >> sample.guestNiceTime;
        
        // Convert to milliseconds (these are clock ticks)
        clock_t clk_tck = sysconf(_SC_CLK_TCK);
        if (clk_tck > 0) {
            sample.systemTime *= 1000.0 / clk_tck;
            sample.userTime *= 1000.0 / clk_tck;
            sample.niceTime *= 1000.0 / clk_tck;
            sample.idleTime *= 1000.0 / clk_tck;
            sample.iowaitTime *= 1000.0 / clk_tck;
            sample.irqTime *= 1000.0 / clk_tck;
            sample.softirqTime *= 1000.0 / clk_tck;
            sample.stealTime *= 1000.0 / clk_tck;
            sample.guestTime *= 1000.0 / clk_tck;
            sample.guestNiceTime *= 1000.0 / clk_tck;
        }
    }
}
#endif

// macOS implementations
#ifdef __APPLE__
#include <mach/mach_host.h>
#include <sys/resource.h>
#include <sys/sysctl.h>

size_t MemoryProfiler::getMacMemoryUsage() const {
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vmStats;
    kern_return_t kernReturn = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmStats, &count);
    
    if (kernReturn == KERN_SUCCESS) {
        // Calculate resident set size in bytes
        return (vmStats.internal_pages + vmStats.external_pages) * getPageSize();
    }
    
    return 0;
}

void CPUMProfiler::getMacCPUTimes(CPUSample& sample) const {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        sample.systemTime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0;
        sample.userTime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
        sample.niceTime = 0.0; // Not available on macOS
        sample.idleTime = 0.0; // Not directly available
        sample.iowaitTime = 0.0;
        sample.irqTime = 0.0;
        sample.softirqTime = 0.0;
        sample.stealTime = 0.0;
        sample.guestTime = 0.0;
        sample.guestNiceTime = 0.0;
        
        // Convert to milliseconds
        sample.systemTime *= 1000.0;
        sample.userTime *= 1000.0;
    }
}

size_t MemoryProfiler::getPageSize() const {
    return sysconf(_SC_PAGESIZE);
}
#endif

// Generic implementations

size_t MemoryProfiler::getGenericMemoryUsage() const {
    // Try to use malloc_info if available
    #ifdef __GLIBC__
        // glibc implementation
        return getGenericMemoryUsageGLIBC();
    #else
        // Fallback implementation
        return 0;
    #endif
}

void CPUMProfiler::getGenericCPUTimes(CPUSample& sample) const {
    // Try to use clock_gettime if available
    #ifdef CLOCK_MONOTONIC
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
            sample.systemTime = ts.tv_sec + ts.tv_nsec / 1000000000.0;
            sample.userTime = 0.0; // Not available generically
            sample.niceTime = 0.0;
            sample.idleTime = 0.0;
            sample.iowaitTime = 0.0;
            sample.irqTime = 0.0;
            sample.softirqTime = 0.0;
            sample.stealTime = 0.0;
            sample.guestTime = 0.0;
            sample.guestNiceTime = 0.0;
        }
    #endif
}

#ifdef __GLIBC__
size_t MemoryProfiler::getGenericMemoryUsageGLIBC() const {
    // Use malloc_info from glibc
    mallinfo info;
    malloc_info(0, &info);
    return info.uordblks; // Total allocated memory
}
#endif

} // namespace nlm