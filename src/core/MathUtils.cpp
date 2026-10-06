#include "MathUtils.hpp"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// Memory tracking implementation
namespace {
    // Platform-specific memory tracking
    #if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    size_t MemoryUtils::getCurrentMemoryUsage() {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
        return 0;
    }
    
    size_t MemoryUtils::getPeakMemoryUsage() {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.PeakWorkingSetSize;
        }
        return 0;
    }
    #elif defined(__linux__) || defined(__APPLE__)
    #include <sys/types.h>
    #include <sys/sysinfo.h>
    size_t MemoryUtils::getCurrentMemoryUsage() {
        struct sysinfo info;
        if (sysinfo(&info) == 0) {
            return info.totalram - info.freeram; // Rough estimate
        }
        return 0;
    }
    
    size_t MemoryUtils::getPeakMemoryUsage() {
        // On Linux, peak memory is in /proc/self/status
        std::ifstream file("/proc/self/status");
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("VmPeak:") != std::string::npos) {
                size_t pos = line.find(":");
                if (pos != std::string::npos) {
                    std::string value = line.substr(pos + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    return static_cast<size_t>(std::stoll(value)) * 1024; // Convert kB to bytes
                }
            }
        }
        return 0;
    }
    #else
    size_t MemoryUtils::getCurrentMemoryUsage() {
        return 0; // Not implemented on this platform
    }
    
    size_t MemoryUtils::getPeakMemoryUsage() {
        return 0; // Not implemented on this platform
    }
    #endif
}

// Initialize static member
std::unordered_map<std::string, size_t> MemoryUtils::LeakDetector::allocations;

} // namespace nlm
