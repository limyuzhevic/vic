#include "PerformanceMonitor.hpp"
#include <sys/resource.h> // For getrusage() on Unix-like systems
#include <unistd.h>      // For getpid() on Unix-like systems

namespace nlm {

// Helper function to get memory usage
static size_t getMemoryUsageImpl() {
#ifdef __linux__
    // Get memory usage from /proc/self/status on Linux
    std::ifstream statusFile("/proc/self/status");
    if (statusFile.is_open()) {
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.compare(0, 6, "VmRSS:") == 0) {
                // Extract memory usage in KB
                size_t pos = line.find('k');
                if (pos != std::string::npos) {
                    std::string value = line.substr(6, pos - 6);
                    // Trim whitespace
                    size_t start = value.find_first_not_of(" \t");
                    size_t end = value.find_last_not_of(" \t");
                    if (start != std::string::npos && end != std::string::npos) {
                        value = value.substr(start, end - start + 1);
                        return std::stoull(value) * 1024; // Convert KB to bytes
                    }
                }
            }
        }
    }
#elif defined(__APPLE__) || defined(__MACH__)
    // Get memory usage using getrusage() on macOS
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024; // Convert KB to bytes
#elif defined(_WIN32) || defined(_WIN64)
    // Windows implementation using PSAPI or Performance Counters
    // For now, return 0
#endif
    return 0;
}

size_t PerformanceMonitor::getMemoryUsage() {
    return getMemoryUsageImpl();
}

} // namespace nlm
